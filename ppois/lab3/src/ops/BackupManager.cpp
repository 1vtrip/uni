#include "ops/BackupManager.h"
#include "exceptions/Exceptions.h"

BackupManager::BackupManager(const BackupPolicy& policy) : policy_(policy), backups_(), nextId_(1) {}

int BackupManager::latestUsableIndex() const {
    for (int i = (int)backups_.size() - 1; i >= 0; --i)
        if (backups_[i].isUsable()) return i;
    return -1;
}

bool BackupManager::isDue(const Timestamp& now) const {
    int i = latestUsableIndex();
    if (i < 0) return true;
    return policy_.isDue(backups_[i].getCreated(), now);
}

/** @brief Неудачная копия кладётся в историю до throw: иначе от попытки не осталось бы следа. */
int BackupManager::runBackup(const std::vector<int>& ids, const std::vector<DataSize>& sizes, const Timestamp& now) {
    if (ids.size() != sizes.size())
        throw InvalidOperationException("число файлов и размеров для резервной копии не совпадает");
    DataSize total;
    for (size_t i = 0; i < sizes.size(); ++i) total += sizes[i];
    Backup backup(nextId_++, now);
    if (policy_.getMaxSize() < total) {
        std::string reason = "размер " + total.toString() + " больше предела " + policy_.getMaxSize().toString();
        backup.fail(reason);
        backups_.push_back(backup);
        throw BackupFailedException(reason);
    }
    for (size_t i = 0; i < ids.size(); ++i) backup.addEntry(ids[i], sizes[i]);
    backup.complete();
    backups_.push_back(backup);
    cleanupOld();
    return backup.getId();
}

/**
 * @brief Обход с конца: erase сдвигает только элементы правее, а их мы уже прошли, поэтому ничего не пропускается.
 * Идём от новых к старым и считаем готовые копии: всё готовое сверх keepCount удаляется, а неудачная удаляется,
 * если новее неё уже есть готовая (ошибку «исправили»; запись о ней останется в журнале аудита).
 */
int BackupManager::cleanupOld() {
    int removed = 0;
    int kept = 0;
    bool newerUsable = false;
    for (int i = (int)backups_.size() - 1; i >= 0; --i) {
        bool remove;
        if (backups_[i].isUsable()) {
            ++kept;
            remove = kept > policy_.getKeepCount();
            newerUsable = true;
        } else {
            remove = newerUsable;
        }
        if (remove) {
            backups_.erase(backups_.begin() + i);
            ++removed;
        }
    }
    return removed;
}

const Backup& BackupManager::latest() const {
    int i = latestUsableIndex();
    if (i < 0) throw BackupFailedException("нет готовых резервных копий");
    return backups_[i];
}

const Backup& BackupManager::get(int id) const {
    for (size_t i = 0; i < backups_.size(); ++i)
        if (backups_[i].getId() == id) return backups_[i];
    throw EntryNotFoundException("резервная копия #" + std::to_string(id));
}

const Backup& BackupManager::forRestore(int id) const {
    const Backup& b = get(id);
    if (!b.isUsable())
        throw BackupFailedException("копия #" + std::to_string(id) + " не готова (" + Backup::statusName(b.getStatus())
                                    + "), восстановить из неё нельзя");
    return b;
}

DataSize BackupManager::storageUsed() const {
    DataSize total;
    for (size_t i = 0; i < backups_.size(); ++i)
        if (backups_[i].isUsable()) total += backups_[i].size();
    return total;
}

int BackupManager::count() const { return (int)backups_.size(); }
const BackupPolicy& BackupManager::getPolicy() const { return policy_; }
void BackupManager::setPolicy(const BackupPolicy& policy) { policy_ = policy; }
const std::vector<Backup>& BackupManager::getBackups() const { return backups_; }

std::ostream& operator<<(std::ostream& os, const BackupManager& m) {
    os << "Резервное копирование (" << m.policy_ << "), копий: " << m.backups_.size();
    for (size_t i = 0; i < m.backups_.size(); ++i) os << "\n  " << m.backups_[i];
    return os;
}
