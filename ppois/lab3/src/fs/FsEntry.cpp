#include "fs/FsEntry.h"
#include "core/Path.h"
#include "exceptions/Exceptions.h"
#include <iomanip>

FsEntry::FsEntry(int id, const std::string& name, int parentId, int ownerId,
                 const Permission& permission, const Timestamp& now)
    : id_(id), name_(name), parentId_(parentId), ownerId_(ownerId), permission_(permission),
      created_(now), modified_(now), metadata_() {}

bool FsEntry::isDirectory() const { return false; }
char FsEntry::typeChar() const { return '-'; }
std::string FsEntry::preview(int) const { return "[" + typeName() + "]"; }

/**
 * @brief typeChar() и size() виртуальные, поэтому у папки будет 'd' и 4 KB, у текста — '-' и длина текста.
 * std::left остаётся в потоке после вывода, поэтому флаги сохраняются и потом восстанавливаются.
 */
void FsEntry::print(std::ostream& os) const {
    std::ios::fmtflags oldFlags = os.flags();
    os << typeChar() << permission_ << "  "
       << std::left << std::setw(5) << ("#" + std::to_string(id_))
       << std::right << std::setw(9) << size().toString() << "  "
       << modified_ << "  " << name_;
    os.flags(oldFlags);
}

void FsEntry::rename(const std::string& newName, const Timestamp& now) {
    if (!Path::isValidName(newName)) throw InvalidPathException(newName);
    name_ = newName;
    touch(now);
}

void FsEntry::moveTo(int newParentId, const Timestamp& now) {
    parentId_ = newParentId;
    touch(now);
}

void FsEntry::touch(const Timestamp& now) { modified_ = now; }
void FsEntry::chmod(const Permission& p) { permission_ = p; }
void FsEntry::chown(int newOwnerId) { ownerId_ = newOwnerId; }

bool FsEntry::canAccess(Permission::Access a, int userId, bool inGroup) const {
    return permission_.allows(a, userId == ownerId_, inGroup);
}

bool FsEntry::isHidden() const { return !name_.empty() && name_[0] == '.'; }

int FsEntry::getId() const { return id_; }
const std::string& FsEntry::getName() const { return name_; }
int FsEntry::getParentId() const { return parentId_; }
int FsEntry::getOwnerId() const { return ownerId_; }
const Permission& FsEntry::getPermission() const { return permission_; }
const Timestamp& FsEntry::getCreated() const { return created_; }
const Timestamp& FsEntry::getModified() const { return modified_; }
Metadata& FsEntry::getMetadata() { return metadata_; }
const Metadata& FsEntry::getMetadata() const { return metadata_; }

bool FsEntry::operator==(const FsEntry& o) const { return id_ == o.id_; }
bool FsEntry::operator!=(const FsEntry& o) const { return !(*this == o); }

std::ostream& operator<<(std::ostream& os, const FsEntry& e) {
    e.print(os);
    return os;
}
