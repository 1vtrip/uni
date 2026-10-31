#include "accounts/AdminAccount.h"
#include "exceptions/Exceptions.h"

AdminAccount::AdminAccount(int id, const std::string& login, const std::string& displayName,
                           const Credentials& credentials, const Timestamp& created, int adminLevel)
    : Account(id, login, displayName, credentials, created), adminLevel_(adminLevel), privileges_() {
    if (adminLevel < MIN_LEVEL || adminLevel > MAX_LEVEL)
        throw InvalidOperationException("уровень администратора должен быть от 1 до 3, а не "
                                        + std::to_string(adminLevel));
}

std::string AdminAccount::role() const { return "Администратор"; }

void AdminAccount::print(std::ostream& os) const {
    Account::print(os);
    printAdminPart(os);
}

void AdminAccount::printAdminPart(std::ostream& os) const {
    os << ", уровень " << adminLevel_;
    if (privileges_.empty()) return;
    os << ", привилегии: ";
    for (size_t i = 0; i < privileges_.size(); ++i) {
        if (i > 0) os << ", ";
        os << privileges_[i];
    }
}

int AdminAccount::maxSessionMinutes() const { return 240; }
bool AdminAccount::canManageUsers() const { return true; }
bool AdminAccount::canReadAnyFile() const { return adminLevel_ >= 2; }

bool AdminAccount::hasPrivilege(const std::string& p) const {
    for (size_t i = 0; i < privileges_.size(); ++i)
        if (privileges_[i] == p) return true;
    return false;
}

void AdminAccount::grantPrivilege(const std::string& p) {
    if (p.empty()) throw InvalidOperationException("имя привилегии не может быть пустым");
    if (!hasPrivilege(p)) privileges_.push_back(p);
}

bool AdminAccount::revokePrivilege(const std::string& p) {
    for (size_t i = 0; i < privileges_.size(); ++i) {
        if (privileges_[i] == p) {
            privileges_.erase(privileges_.begin() + i);
            return true;
        }
    }
    return false;
}

bool AdminAccount::promote() {
    if (adminLevel_ >= MAX_LEVEL) return false;
    ++adminLevel_;
    return true;
}

bool AdminAccount::demote() {
    if (adminLevel_ <= MIN_LEVEL) return false;
    --adminLevel_;
    return true;
}

int AdminAccount::getAdminLevel() const { return adminLevel_; }
const std::vector<std::string>& AdminAccount::getPrivileges() const { return privileges_; }
