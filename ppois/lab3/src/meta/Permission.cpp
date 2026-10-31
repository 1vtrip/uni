#include "meta/Permission.h"
#include "exceptions/Exceptions.h"

Permission::Permission(int mode) : mode_(mode & 0777) {}
Permission Permission::forFile()      { return Permission(0644); }
Permission Permission::forDirectory() { return Permission(0755); }
Permission Permission::forPrivate()   { return Permission(0600); }

// Владелец — старшие 3 бита, группа — средние, остальные — младшие. В тройке: r=4, w=2, x=1.
int Permission::bit(Scope scope, Access access) {
    int shift = scope == Scope::Owner ? 6 : scope == Scope::Group ? 3 : 0;
    int value = access == Access::Read ? 4 : access == Access::Write ? 2 : 1;
    return value << shift;
}

bool Permission::has(Scope scope, Access access) const { return (mode_ & bit(scope, access)) != 0; }
void Permission::grant(Scope scope, Access access)     { mode_ |= bit(scope, access); }
void Permission::revoke(Scope scope, Access access)    { mode_ &= ~bit(scope, access); }
int Permission::toOctal() const { return mode_; }

bool Permission::allows(Access access, bool isOwner, bool inGroup) const {
    if (isOwner) return has(Scope::Owner, access);
    if (inGroup) return has(Scope::Group, access);
    return has(Scope::Others, access);
}

/** @brief Символ с номером i отвечает за бит 8 - i; на месте i допустима буква "rwx"[i % 3] или '-'. */
Permission Permission::fromString(const std::string& rwx) {
    if (rwx.size() != 9) throw FileSystemException("Права должны быть из 9 символов: " + rwx);
    const char letters[] = {'r', 'w', 'x'};
    int mode = 0;
    for (int i = 0; i < 9; ++i) {
        if (rwx[i] == letters[i % 3]) mode |= 1 << (8 - i);
        else if (rwx[i] != '-') throw FileSystemException("Недопустимый символ в правах: " + rwx);
    }
    return Permission(mode);
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
