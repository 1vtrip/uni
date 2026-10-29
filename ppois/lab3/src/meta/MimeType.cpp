#include "meta/MimeType.h"
#include <cctype>

MimeType::MimeType(const std::string& type, const std::string& subtype)
    : type_(type), subtype_(subtype) {}

/**
 * @brief Расширение приводится к нижнему регистру, точка в начале убирается,
 * затем ищется в таблице из трёх параллельных массивов.
 */
MimeType MimeType::fromExtension(const std::string& ext) {
    std::string e = ext;
    if (!e.empty() && e[0] == '.') e = e.substr(1);
    for (size_t i = 0; i < e.size(); ++i)
        e[i] = (char)std::tolower((unsigned char)e[i]);

    const int count = 11;
    const char* exts[count]     = {"txt",   "md",    "cpp",   "png",   "jpg",   "jpeg",
                                   "gif",   "zip",   "tar",   "exe",   "sh"};
    const char* types[count]    = {"text",  "text",  "text",  "image", "image", "image",
                                   "image", "application", "application", "application", "application"};
    const char* subtypes[count] = {"plain", "plain", "plain", "png",   "jpeg",  "jpeg",
                                   "gif",   "zip",   "zip",   "x-executable", "x-executable"};
    for (int i = 0; i < count; ++i)
        if (e == exts[i]) return MimeType(types[i], subtypes[i]);
    return MimeType();
}

bool MimeType::isText() const { return type_ == "text"; }
bool MimeType::isImage() const { return type_ == "image"; }
bool MimeType::isArchive() const { return subtype_ == "zip"; }
bool MimeType::isExecutable() const { return subtype_ == "x-executable"; }
std::string MimeType::toString() const { return type_ + "/" + subtype_; }

const std::string& MimeType::getType() const { return type_; }
const std::string& MimeType::getSubtype() const { return subtype_; }

bool MimeType::operator==(const MimeType& o) const { return type_ == o.type_ && subtype_ == o.subtype_; }
bool MimeType::operator!=(const MimeType& o) const { return !(*this == o); }
std::ostream& operator<<(std::ostream& os, const MimeType& m) { return os << m.toString(); }
