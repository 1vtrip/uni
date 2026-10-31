#include "ops/LockManager.h"
#include "exceptions/Exceptions.h"
#include <string>

LockManager::LockManager() : locks_() {}

/**
 * @brief Конфликт проверяется по всем блокировкам файла: при повышении своей Shared до Exclusive
 * мешают чужие Shared. Новая блокировка создаётся до изменения вектора, поэтому при ошибке в
 * конструкторе FileLock (minutes <= 0) старая блокировка остаётся.
 */
void LockManager::acquire(int entryId, int userId, FileLock::LockMode mode, const Timestamp& now, int minutes) {
    cleanupExpired(now);
    for (size_t i = 0; i < locks_.size(); ++i)
        if (locks_[i].getEntryId() == entryId && locks_[i].conflictsWith(mode, userId))
            throw FileLockedException("#" + std::to_string(entryId), locks_[i].getUserId());
    FileLock lock(entryId, userId, mode, now, minutes);
    for (size_t i = 0; i < locks_.size(); ++i) {
        if (locks_[i] == lock) {
            locks_[i] = lock;
            return;
        }
    }
    locks_.push_back(lock);
}

bool LockManager::release(int entryId, int userId) {
    for (size_t i = 0; i < locks_.size(); ++i) {
        if (locks_[i].getEntryId() == entryId && locks_[i].isHeldBy(userId)) {
            locks_.erase(locks_.begin() + i);
            return true;
        }
    }
    return false;
}

/** @brief После erase индекс не увеличивается, иначе соседняя блокировка была бы пропущена. */
int LockManager::releaseAllFor(int entryId) {
    int removed = 0;
    size_t i = 0;
    while (i < locks_.size()) {
        if (locks_[i].getEntryId() == entryId) {
            locks_.erase(locks_.begin() + i);
            ++removed;
        } else {
            ++i;
        }
    }
    return removed;
}

bool LockManager::isLocked(int entryId, const Timestamp& now) const {
    for (size_t i = 0; i < locks_.size(); ++i)
        if (locks_[i].getEntryId() == entryId && !locks_[i].isExpired(now)) return true;
    return false;
}

bool LockManager::canWrite(int entryId, int userId, const Timestamp& now) const {
    for (size_t i = 0; i < locks_.size(); ++i)
        if (locks_[i].getEntryId() == entryId && !locks_[i].isHeldBy(userId) && !locks_[i].isExpired(now))
            return false;
    return true;
}

int LockManager::cleanupExpired(const Timestamp& now) {
    int removed = 0;
    size_t i = 0;
    while (i < locks_.size()) {
        if (locks_[i].isExpired(now)) {
            locks_.erase(locks_.begin() + i);
            ++removed;
        } else {
            ++i;
        }
    }
    return removed;
}

std::vector<FileLock> LockManager::locksOf(int userId) const {
    std::vector<FileLock> result;
    for (size_t i = 0; i < locks_.size(); ++i)
        if (locks_[i].isHeldBy(userId)) result.push_back(locks_[i]);
    return result;
}

int LockManager::count() const { return (int)locks_.size(); }
const std::vector<FileLock>& LockManager::getLocks() const { return locks_; }

std::ostream& operator<<(std::ostream& os, const LockManager& m) {
    os << "Блокировок: " << m.locks_.size();
    for (size_t i = 0; i < m.locks_.size(); ++i) os << "\n  " << m.locks_[i];
    return os;
}
