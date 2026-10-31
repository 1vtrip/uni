#include "ops/Backup.h"
#include "exceptions/Exceptions.h"

Backup::Backup(int id, const Timestamp& created)
    : id_(id), created_(created), entryIds_(), totalSize_(), status_(Status::InProgress), failReason_() {}

/** @brief Без проверки можно было бы, например, сделать проваленную копию «рабочей» через complete(). */
void Backup::requireInProgress(const std::string& action) const {
    if (status_ != Status::InProgress)
        throw InvalidOperationException("нельзя " + action + ": копия #" + std::to_string(id_) + " уже "
                                        + (status_ == Status::Completed ? "завершена" : "провалена"));
}

void Backup::addEntry(int id, const DataSize& s) {
    requireInProgress("добавить файл");
    if (contains(id)) return;
    entryIds_.push_back(id);
    totalSize_ += s;
}

void Backup::complete() {
    requireInProgress("завершить");
    status_ = Status::Completed;
}

void Backup::fail(const std::string& reason) {
    requireInProgress("отметить ошибку");
    status_ = Status::Failed;
    failReason_ = reason.empty() ? "причина не указана" : reason;
}

bool Backup::contains(int entryId) const {
    for (size_t i = 0; i < entryIds_.size(); ++i)
        if (entryIds_[i] == entryId) return true;
    return false;
}

bool Backup::isUsable() const { return status_ == Status::Completed; }
DataSize Backup::size() const { return totalSize_; }
int Backup::entryCount() const { return (int)entryIds_.size(); }

const char* Backup::statusName(Status s) {
    if (s == Status::InProgress) return "выполняется";
    if (s == Status::Completed) return "готова";
    return "ошибка";
}

int Backup::getId() const { return id_; }
const Timestamp& Backup::getCreated() const { return created_; }
const std::vector<int>& Backup::getEntryIds() const { return entryIds_; }
Backup::Status Backup::getStatus() const { return status_; }
const std::string& Backup::getFailReason() const { return failReason_; }

bool Backup::operator==(const Backup& o) const { return id_ == o.id_; }
bool Backup::operator!=(const Backup& o) const { return !(*this == o); }

std::ostream& operator<<(std::ostream& os, const Backup& b) {
    os << "Копия #" << b.id_ << " от " << b.created_ << ": " << Backup::statusName(b.status_) << ", файлов "
       << b.entryIds_.size() << ", " << b.totalSize_;
    if (b.status_ == Backup::Status::Failed) os << " (" << b.failReason_ << ")";
    return os;
}
