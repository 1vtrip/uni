#include "meta/Permission.h"

Permission::Permission(int mode) : mode_(mode & 0777) {}
Permission Permission::forFile()      { return Permission(0644); }
Permission Permission::forDirectory() { return Permission(0755); }
Permission Permission::forPrivate()   { return Permission(0600); }

// Владелец — старшие 3 бита, группа — средние, остальные — младшие. В тройке: r=4, w=2, x=1.
int Permission::bit(int scope, int access) {
    int shift = scope == OWNER ? 6 : scope == GROUP ? 3 : 0;
    int value = access == READ ? 4 : access == WRITE ? 2 : 1;
    return value << shift;
}

bool Permission::has(int scope, int access) const { return (mode_ & bit(scope, access)) != 0; }
void Permission::grant(int scope, int access)     { mode_ |= bit(scope, access); }
void Permission::revoke(int scope, int access)    { mode_ &= ~bit(scope, access); }
int Permission::toOctal() const { return mode_; }

bool Permission::allows(int access, bool isOwner, bool inGroup) const {
    if (isOwner) return has(OWNER, access);
    if (inGroup) return has(GROUP, access);
    return has(OTHERS, access);
}

/** @brief Символ с номером i отвечает за бит 8 - i; на месте i допустима буква "rwx"[i % 3] или '-'. */
bool Permission::fromString(const std::string& rwx, Permission& result) {
    if (rwx.size() != 9) return false;                  // TODO(exc): FileSystemException
    const char letters[] = {'r', 'w', 'x'};
    int mode = 0;
    for (int i = 0; i < 9; ++i) {
        if (rwx[i] == letters[i % 3]) mode |= 1 << (8 - i);
        else if (rwx[i] != '-') return false;           // TODO(exc): FileSystemException
    }
    result = Permission(mode);
    return true;
}

std::string Permission::toString() const {
    std::string s = "rwxrwxrwx";
    for (int i = 0; i < 9; ++i)
        if (!(mode_ & (1 << (8 - i)))) s[i] = '-';
    return s;
}

bool Permission::operator==(const Permission& o) const { return mode_ == o.mode_; }
bool Permission::operator!=(const Permission& o) const { return !(*this == o); }
std::ostream& operator<<(std::ostream& os, const Permission& p) { return os << p.toString(); }
