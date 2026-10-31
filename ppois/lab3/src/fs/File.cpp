#include "fs/File.h"
#include "core/Path.h"

File::File(int id, const std::string& name, int parentId, int ownerId,
           const Permission& permission, const Timestamp& now,
           const MimeType& mime, const DataSize& size)
    : FsEntry(id, name, parentId, ownerId, permission, now),
      size_(size), mime_(mime), checksum_() {}

DataSize File::size() const { return size_; }

void File::print(std::ostream& os) const {
    FsEntry::print(os);
    os << "  [" << mime_ << "]";
}

/** @brief Имя — одна часть пути, поэтому Path::parse не бросит: имя уже проверено при создании и rename. */
std::string File::extension() const { return Path::parse(name_).extension(); }

bool File::verify(const Checksum& expected) const { return checksum_.matches(expected); }

const MimeType& File::getMime() const { return mime_; }
const Checksum& File::getChecksum() const { return checksum_; }

void File::updateChecksum(const std::string& data) { checksum_ = Checksum::of(data); }
