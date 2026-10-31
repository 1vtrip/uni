#include "ops/AuditRecord.h"
#include "exceptions/Exceptions.h"
#include <sstream>

AuditRecord::AuditRecord(int id, const Timestamp& time, int userId, const std::string& action,
                         const std::string& target, bool success, const std::string& details)
    : id_(id), time_(time), userId_(userId), action_(action), target_(target), success_(success),
      details_(details) {
    if (action.empty()) throw InvalidOperationException("действие в записи аудита не может быть пустым");
}

bool AuditRecord::isFailure() const { return !success_; }

/** @brief Строка собирается через ostringstream, потому что у Timestamp есть только operator<<. */
std::string AuditRecord::toLine() const {
    std::ostringstream out;
    out << '[' << time_ << "] ";
    if (userId_ == 0) out << "неизвестный";
    else out << '#' << userId_;
    out << ' ' << action_;
    if (!target_.empty()) out << ' ' << target_;
    out << " — " << (success_ ? "успешно" : "ОШИБКА");
    if (!details_.empty()) out << ": " << details_;
    return out.str();
}

int AuditRecord::getId() const { return id_; }
const Timestamp& AuditRecord::getTime() const { return time_; }
int AuditRecord::getUserId() const { return userId_; }
const std::string& AuditRecord::getAction() const { return action_; }
const std::string& AuditRecord::getTarget() const { return target_; }
bool AuditRecord::isSuccess() const { return success_; }
const std::string& AuditRecord::getDetails() const { return details_; }

bool AuditRecord::operator==(const AuditRecord& o) const { return id_ == o.id_; }
bool AuditRecord::operator!=(const AuditRecord& o) const { return !(*this == o); }

std::ostream& operator<<(std::ostream& os, const AuditRecord& r) { return os << r.toLine(); }
