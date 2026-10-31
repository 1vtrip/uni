#include "accounts/UserAccount.h"

UserAccount::UserAccount(int id, const std::string& login, const std::string& displayName,
                         const Credentials& credentials, const Timestamp& created, int homeDirId)
    : Account(id, login, displayName, credentials, created),
      homeDirId_(homeDirId), quota_(id, DataSize::fromGB(1)) {}

std::string UserAccount::role() const { return "Пользователь"; }

void UserAccount::print(std::ostream& os) const {
    Account::print(os);
    printUserPart(os);
}

void UserAccount::printUserPart(std::ostream& os) const {
    os << ", домашняя папка #" << homeDirId_ << ", квота " << quota_;
}

DataSize UserAccount::defaultQuota() const { return DataSize::fromGB(1); }
bool UserAccount::canShare() const { return true; }

int UserAccount::getHomeDirId() const { return homeDirId_; }
Quota& UserAccount::getQuota() { return quota_; }
const Quota& UserAccount::getQuota() const { return quota_; }

void UserAccount::resetQuota() { quota_.setLimit(defaultQuota()); }
