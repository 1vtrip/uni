#include "accounts/Account.h"
#include "accounts/PasswordPolicy.h"
#include "exceptions/Exceptions.h"

Account::Account(int id, const std::string& login, const std::string& displayName,
                 const Credentials& credentials, const Timestamp& created)
    : id_(id), login_(login), displayName_(displayName), credentials_(credentials), groupIds_(),
      created_(created), blocked_(false), failedLogins_(0) {
    if (credentials.getLogin() != login)
        throw InvalidOperationException("логин " + login + " не совпадает с логином в данных для входа ("
                                        + credentials.getLogin() + ")");
}

int Account::maxSessionMinutes() const { return 60; }
bool Account::canManageUsers() const { return false; }
bool Account::canReadAnyFile() const { return false; }

/** @brief role() виртуальный, поэтому даже базовый print печатает роль настоящего типа. */
void Account::print(std::ostream& os) const {
    os << '#' << id_ << ' ' << login_ << " (" << displayName_ << ") — " << role();
    if (blocked_) os << " [ЗАБЛОКИРОВАН]";
}

bool Account::checkPassword(const std::string& password) {
    if (blocked_) return false;
    if (credentials_.verify(password)) {
        failedLogins_ = 0;
        return true;
    }
    ++failedLogins_;
    if (failedLogins_ >= MAX_FAILED_LOGINS) blocked_ = true;
    return false;
}

/**
 * @brief Старый пароль проверяется через checkPassword, а не напрямую через credentials_:
 * иначе через смену пароля можно было бы подбирать пароль без ограничения попыток.
 */
void Account::changePassword(const std::string& oldPwd, const std::string& newPwd, const PasswordPolicy& policy) {
    if (blocked_) throw AuthenticationException(login_, "учётная запись заблокирована");
    if (!checkPassword(oldPwd)) throw AuthenticationException(login_);
    policy.validate(newPwd, login_);
    credentials_.change(newPwd);
}

void Account::joinGroup(int groupId) {
    if (!isInGroup(groupId)) groupIds_.push_back(groupId);
}

void Account::leaveGroup(int groupId) {
    for (size_t i = 0; i < groupIds_.size(); ++i) {
        if (groupIds_[i] == groupId) {
            groupIds_.erase(groupIds_.begin() + i);
            return;
        }
    }
}

bool Account::isInGroup(int groupId) const {
    for (size_t i = 0; i < groupIds_.size(); ++i)
        if (groupIds_[i] == groupId) return true;
    return false;
}

void Account::block() { blocked_ = true; }

void Account::unblock() {
    blocked_ = false;
    failedLogins_ = 0;
}

bool Account::isBlocked() const { return blocked_; }

int Account::getId() const { return id_; }
const std::string& Account::getLogin() const { return login_; }
const std::string& Account::getDisplayName() const { return displayName_; }
const Credentials& Account::getCredentials() const { return credentials_; }
const std::vector<int>& Account::getGroupIds() const { return groupIds_; }
const Timestamp& Account::getCreated() const { return created_; }
int Account::getFailedLogins() const { return failedLogins_; }

bool Account::operator==(const Account& o) const { return id_ == o.id_; }
bool Account::operator!=(const Account& o) const { return !(*this == o); }

std::ostream& operator<<(std::ostream& os, const Account& a) {
    a.print(os);
    return os;
}
