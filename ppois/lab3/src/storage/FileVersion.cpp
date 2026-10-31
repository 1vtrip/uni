#include "storage/FileVersion.h"

FileVersion::FileVersion(int number, const std::string& content, int authorId,
                         const Timestamp& created, const std::string& comment)
    : number_(number), content_(content), created_(created), authorId_(authorId),
      checksum_(Checksum::of(content)), comment_(comment) {}

DataSize FileVersion::size() const { return DataSize((long long)content_.size()); }
bool FileVersion::isIntact() const { return Checksum::of(content_).matches(checksum_); }
void FileVersion::corrupt(const std::string& garbage) { content_ = garbage; }

int FileVersion::getNumber() const { return number_; }
const std::string& FileVersion::getContent() const { return content_; }
const Timestamp& FileVersion::getCreated() const { return created_; }
int FileVersion::getAuthorId() const { return authorId_; }
const Checksum& FileVersion::getChecksum() const { return checksum_; }
const std::string& FileVersion::getComment() const { return comment_; }

bool FileVersion::operator==(const FileVersion& o) const { return number_ == o.number_; }
bool FileVersion::operator!=(const FileVersion& o) const { return !(*this == o); }

std::ostream& operator<<(std::ostream& os, const FileVersion& v) {
    os << 'v' << v.number_ << "  " << v.created_ << "  автор #" << v.authorId_ << "  " << v.size();
    if (!v.comment_.empty()) os << "  " << v.comment_;
    if (!v.isIntact()) os << "  [ПОВРЕЖДЕНА]";
    return os;
}
