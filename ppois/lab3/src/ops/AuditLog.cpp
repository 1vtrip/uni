#include "ops/AuditLog.h"
#include "exceptions/Exceptions.h"

AuditLog::AuditLog(int maxRecords) : records_(), maxRecords_(maxRecords), nextId_(1) {
    if (maxRecords <= 0) throw InvalidOperationException("в журнале должна помещаться хотя бы 1 запись");
}

/** @brief Запись создаётся до изменения вектора: если конструктор бросит (пустое действие), журнал не меняется. */
void AuditLog::record(const Timestamp& t, int userId, const std::string& action, const std::string& target,
                      bool success, const std::string& details) {
    AuditRecord r(nextId_, t, userId, action, target, success, details);
    ++nextId_;
    records_.push_back(r);
    if ((int)records_.size() > maxRecords_) records_.erase(records_.begin());
}

std::vector<AuditRecord> AuditLog::byUser(int userId) const {
    std::vector<AuditRecord> result;
    for (size_t i = 0; i < records_.size(); ++i)
        if (records_[i].getUserId() == userId) result.push_back(records_[i]);
    return result;
}

std::vector<AuditRecord> AuditLog::failures() const {
    std::vector<AuditRecord> result;
    for (size_t i = 0; i < records_.size(); ++i)
        if (records_[i].isFailure()) result.push_back(records_[i]);
    return result;
}

int AuditLog::failedLoginsSince(int userId, const Timestamp& since) const {
    int count = 0;
    for (size_t i = 0; i < records_.size(); ++i) {
        const AuditRecord& r = records_[i];
        if (r.getUserId() == userId && r.getAction() == "login" && r.isFailure() && since <= r.getTime()) ++count;
    }
    return count;
}

/**
 * @brief Счётчики по пользователям хранятся в двух параллельных векторах (как ключи и значения в Metadata):
 * userIds[i] — пользователь, counts[i] — сколько у него неудачных действий.
 */
std::vector<int> AuditLog::suspiciousUsers(int threshold, const Timestamp& since) const {
    std::vector<int> userIds;
    std::vector<int> counts;
    for (size_t i = 0; i < records_.size(); ++i) {
        const AuditRecord& r = records_[i];
        if (!r.isFailure() || r.getUserId() == 0 || r.getTime() < since) continue;
        size_t j = 0;
        while (j < userIds.size() && userIds[j] != r.getUserId()) ++j;
        if (j == userIds.size()) {
            userIds.push_back(r.getUserId());
            counts.push_back(1);
        } else {
            ++counts[j];
        }
    }
    std::vector<int> result;
    for (size_t j = 0; j < userIds.size(); ++j)
        if (counts[j] > threshold) result.push_back(userIds[j]);
    return result;
}

int AuditLog::count() const { return (int)records_.size(); }
const std::vector<AuditRecord>& AuditLog::getRecords() const { return records_; }

std::ostream& operator<<(std::ostream& os, const AuditLog& log) {
    os << "Журнал аудита (" << log.records_.size() << " из " << log.maxRecords_ << "):";
    for (size_t i = 0; i < log.records_.size(); ++i) os << "\n  " << log.records_[i];
    return os;
}
