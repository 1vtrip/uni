#include "ops/BackupPolicy.h"
#include "exceptions/Exceptions.h"
#include "fs/FsEntry.h"

BackupPolicy::BackupPolicy(int intervalDays, int keepCount, bool includeHidden, const DataSize& maxSize)
    : intervalDays_(intervalDays), keepCount_(keepCount), includeHidden_(includeHidden), maxSize_(maxSize) {
    if (intervalDays <= 0) throw InvalidOperationException("интервал резервного копирования должен быть положительным");
    if (keepCount <= 0) throw InvalidOperationException("хранить нужно хотя бы 1 резервную копию");
    if (maxSize.isZero()) throw InvalidOperationException("предел размера резервной копии должен быть положительным");
}

/** @brief daysTo считает полные сутки: при интервале 1 день через 23:59 копия ещё не нужна. */
bool BackupPolicy::isDue(const Timestamp& lastBackup, const Timestamp& now) const {
    return lastBackup.daysTo(now) >= intervalDays_;
}

Timestamp BackupPolicy::nextDue(const Timestamp& lastBackup) const { return lastBackup.addDays(intervalDays_); }

/** @brief Тип определяется виртуальным typeChar(), без dynamic_cast и указателей: '-' только у файлов. */
bool BackupPolicy::shouldInclude(const FsEntry& e) const {
    if (e.typeChar() != '-') return false;
    if (e.isHidden() && !includeHidden_) return false;
    return true;
}

int BackupPolicy::getIntervalDays() const { return intervalDays_; }
int BackupPolicy::getKeepCount() const { return keepCount_; }
bool BackupPolicy::includesHidden() const { return includeHidden_; }
const DataSize& BackupPolicy::getMaxSize() const { return maxSize_; }

std::ostream& operator<<(std::ostream& os, const BackupPolicy& p) {
    return os << "каждые " << p.intervalDays_ << " дн., хранить " << p.keepCount_ << ", до " << p.maxSize_
              << (p.includeHidden_ ? ", со скрытыми" : ", без скрытых");
}
