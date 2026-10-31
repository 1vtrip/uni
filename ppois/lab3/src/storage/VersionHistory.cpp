#include "storage/VersionHistory.h"
#include "exceptions/Exceptions.h"

VersionHistory::VersionHistory(int fileId, int maxVersions)
    : fileId_(fileId), versions_(), maxVersions_(maxVersions) {
    if (maxVersions <= 0) throw InvalidOperationException("в истории должна храниться хотя бы 1 версия");
}

int VersionHistory::indexOf(int number) const {
    for (size_t i = 0; i < versions_.size(); ++i)
        if (versions_[i].getNumber() == number) return (int)i;
    return -1;
}

/**
 * @brief Номер берётся от последней версии, а не от count(): после prune при лимите 10
 * остаются версии 4–13, и count() + 1 = 11 совпал бы с уже существующей.
 */
int VersionHistory::addVersion(const std::string& content, int authorId, const Timestamp& now,
                               const std::string& comment) {
    if (!versions_.empty() && Checksum::of(content).matches(versions_.back().getChecksum()))
        return versions_.back().getNumber();
    int number = versions_.empty() ? 1 : versions_.back().getNumber() + 1;
    versions_.push_back(FileVersion(number, content, authorId, now, comment));
    prune();
    return number;
}

const FileVersion& VersionHistory::get(int number) const {
    int i = indexOf(number);
    if (i < 0) throw VersionNotFoundException(number);
    return versions_[i];
}

FileVersion& VersionHistory::get(int number) {
    int i = indexOf(number);
    if (i < 0) throw VersionNotFoundException(number);
    return versions_[i];
}

const FileVersion& VersionHistory::latest() const {
    if (versions_.empty()) throw VersionNotFoundException(0);
    return versions_.back();
}

/** @brief История знает только ID файла, поэтому в исключении "#id"; путь подставит FileSystem. */
std::string VersionHistory::rollbackTo(int number) const {
    const FileVersion& v = get(number);
    if (!v.isIntact())
        throw CorruptedFileException("#" + std::to_string(fileId_) + ", версия " + std::to_string(number));
    return v.getContent();
}

/** @brief Самые старые версии лежат в начале вектора, поэтому удаляются первые лишние. */
int VersionHistory::prune() {
    int extra = (int)versions_.size() - maxVersions_;
    if (extra <= 0) return 0;
    versions_.erase(versions_.begin(), versions_.begin() + extra);
    return extra;
}

bool VersionHistory::hasVersion(int number) const { return indexOf(number) >= 0; }

DataSize VersionHistory::totalSize() const {
    DataSize total;
    for (size_t i = 0; i < versions_.size(); ++i) total += versions_[i].size();
    return total;
}

int VersionHistory::count() const { return (int)versions_.size(); }
int VersionHistory::getFileId() const { return fileId_; }
int VersionHistory::getMaxVersions() const { return maxVersions_; }
const std::vector<FileVersion>& VersionHistory::getVersions() const { return versions_; }

bool VersionHistory::operator==(const VersionHistory& o) const { return fileId_ == o.fileId_; }
bool VersionHistory::operator!=(const VersionHistory& o) const { return !(*this == o); }

std::ostream& operator<<(std::ostream& os, const VersionHistory& h) {
    os << "История файла #" << h.fileId_ << " (" << h.versions_.size() << " из " << h.maxVersions_ << "):";
    for (size_t i = 0; i < h.versions_.size(); ++i) os << "\n  " << h.versions_[i];
    return os;
}
