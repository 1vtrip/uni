#include "fs/ArchiveFile.h"
#include "exceptions/Exceptions.h"

ArchiveFile::ArchiveFile(int id, const std::string& name, int parentId, int ownerId,
                         const Permission& permission, const Timestamp& now, const std::string& format)
    : File(id, name, parentId, ownerId, permission, now, MimeType::fromExtension(format)),
      entries_(), format_(format) {}

std::string ArchiveFile::typeName() const { return "Архив"; }

DataSize ArchiveFile::size() const {
    DataSize total;
    for (size_t i = 0; i < entries_.size(); ++i) total += entries_[i].getCompressedSize();
    return total;
}

/** @brief Сначала собирается полный список имён, потом он обрезается до maxChars. */
std::string ArchiveFile::preview(int maxChars) const {
    if (entries_.empty()) return "[Архив пуст]";
    std::string names;
    for (size_t i = 0; i < entries_.size(); ++i) {
        if (i > 0) names += ", ";
        names += entries_[i].getName();
    }
    if (maxChars < 0) maxChars = 0;
    if ((int)names.size() <= maxChars) return names;
    return names.substr(0, maxChars) + "...";
}

void ArchiveFile::addEntry(const ArchiveEntry& e, const Timestamp& now) {
    if (contains(e.getName())) throw EntryAlreadyExistsException(e.getName() + " в архиве " + name_);
    entries_.push_back(e);
    touch(now);
}

bool ArchiveFile::contains(const std::string& name) const {
    for (size_t i = 0; i < entries_.size(); ++i)
        if (entries_[i].getName() == name) return true;
    return false;
}

DataSize ArchiveFile::originalTotal() const {
    DataSize total;
    for (size_t i = 0; i < entries_.size(); ++i) total += entries_[i].getOriginalSize();
    return total;
}

/** @brief Деление в double: иначе 300 / 1000 у целых даст 0. Пустой архив — 0, а не деление на ноль. */
double ArchiveFile::compressionRatio() const {
    DataSize original = originalTotal();
    if (original.isZero()) return 0.0;
    return (double)size().getBytes() / original.getBytes();
}

int ArchiveFile::entryCount() const { return (int)entries_.size(); }
const std::vector<ArchiveEntry>& ArchiveFile::getEntries() const { return entries_; }
const std::string& ArchiveFile::getFormat() const { return format_; }
