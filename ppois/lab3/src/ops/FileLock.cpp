#include "ops/FileLock.h"
#include "exceptions/Exceptions.h"

FileLock::FileLock(int entryId, int userId, LockMode mode, const Timestamp& now, int minutes)
    : entryId_(entryId), userId_(userId), mode_(mode), acquiredAt_(now), expiresAt_(now.addMinutes(minutes)) {
    if (minutes <= 0) throw InvalidOperationException("срок блокировки должен быть положительным");
}

/** @brief Таблица совместимости: конфликтов нет только у своей блокировки и у пары Shared + Shared. */
bool FileLock::conflictsWith(LockMode wanted, int userId) const {
    if (userId == userId_) return false;
    if (mode_ == LockMode::Shared && wanted == LockMode::Shared) return false;
    return true;
}

bool FileLock::isExpired(const Timestamp& now) const { return expiresAt_ <= now; }
bool FileLock::isHeldBy(int userId) const { return userId_ == userId; }

void FileLock::refresh(const Timestamp& now, int minutes) {
    if (isExpired(now)) throw InvalidOperationException("блокировка файла #" + std::to_string(entryId_)
                                                        + " истекла, возьмите её заново");
    if (minutes <= 0) throw InvalidOperationException("срок блокировки должен быть положительным");
    expiresAt_ = now.addMinutes(minutes);
}

const char* FileLock::modeName(LockMode mode) { return mode == LockMode::Shared ? "общая" : "монопольная"; }

int FileLock::getEntryId() const { return entryId_; }
int FileLock::getUserId() const { return userId_; }
FileLock::LockMode FileLock::getMode() const { return mode_; }
const Timestamp& FileLock::getAcquiredAt() const { return acquiredAt_; }
const Timestamp& FileLock::getExpiresAt() const { return expiresAt_; }

bool FileLock::operator==(const FileLock& o) const { return entryId_ == o.entryId_ && userId_ == o.userId_; }
bool FileLock::operator!=(const FileLock& o) const { return !(*this == o); }

std::ostream& operator<<(std::ostream& os, const FileLock& l) {
    return os << '#' << l.entryId_ << ": " << FileLock::modeName(l.mode_) << ", пользователь #" << l.userId_
              << ", до " << l.expiresAt_;
}
