#include "accounts/AccessControlEntry.h"
#include "exceptions/Exceptions.h"

AccessControlEntry::AccessControlEntry(int principalId, bool isGroup, Permission::Access access, bool allow)
    : principalId_(principalId), isGroup_(isGroup), access_(access), allow_(allow) {
    if (principalId <= 0) throw InvalidOperationException("ID в правиле доступа должен быть положительным");
}

bool AccessControlEntry::appliesTo(int userId, const std::vector<int>& groupIds) const {
    if (!isGroup_) return principalId_ == userId;
    for (size_t i = 0; i < groupIds.size(); ++i)
        if (groupIds[i] == principalId_) return true;
    return false;
}

bool AccessControlEntry::matches(Permission::Access access, int userId, const std::vector<int>& groupIds) const {
    return access_ == access && appliesTo(userId, groupIds);
}

const char* AccessControlEntry::accessName(Permission::Access access) {
    if (access == Permission::Access::Read) return "чтение";
    if (access == Permission::Access::Write) return "запись";
    return "выполнение";
}

int AccessControlEntry::getPrincipalId() const { return principalId_; }
bool AccessControlEntry::isGroup() const { return isGroup_; }
Permission::Access AccessControlEntry::getAccess() const { return access_; }
bool AccessControlEntry::isAllow() const { return allow_; }

bool AccessControlEntry::operator==(const AccessControlEntry& o) const {
    return principalId_ == o.principalId_ && isGroup_ == o.isGroup_ && access_ == o.access_;
}
bool AccessControlEntry::operator!=(const AccessControlEntry& o) const { return !(*this == o); }

std::ostream& operator<<(std::ostream& os, const AccessControlEntry& e) {
    return os << (e.allow_ ? "разрешить " : "запретить ") << (e.isGroup_ ? "группе #" : "пользователю #")
              << e.principalId_ << ": " << AccessControlEntry::accessName(e.access_);
}
