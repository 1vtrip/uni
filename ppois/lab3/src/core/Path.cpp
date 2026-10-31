#include "core/Path.h"
#include "exceptions/Exceptions.h"
#include <cctype>
#include <sstream>

Path::Path() : parts_(), absolute_(true) {}

/** @brief Строка режется по '/' через getline, части по одной проверяются и складываются в path. */
Path Path::parse(const std::string& text) {
    if (text.empty()) throw InvalidPathException("пустая строка");
    Path path;
    path.absolute_ = text[0] == '/';
    std::istringstream in(text);
    std::string part;
    while (std::getline(in, part, '/')) {
        if (part.empty() || part == ".") continue;
        if (part == "..") {
            if (path.parts_.empty()) throw InvalidPathException(text + " (\"..\" выше начала пути)");
            path.parts_.pop_back();
            continue;
        }
        if (!isValidName(part)) throw InvalidPathException(text);
        path.parts_.push_back(part);
    }
    return path;
}

bool Path::isValidName(const std::string& name) {
    if (name.empty() || name.size() > 255) return false;
    if (name == "." || name == "..") return false;
    return name.find('/') == std::string::npos;
}

std::string Path::toString() const {
    if (parts_.empty()) return absolute_ ? "/" : ".";
    std::string text;
    for (size_t i = 0; i < parts_.size(); ++i) {
        if (i > 0 || absolute_) text += '/';
        text += parts_[i];
    }
    return text;
}

Path Path::parent() const {
    Path result = *this;
    if (!result.parts_.empty()) result.parts_.pop_back();
    return result;
}

std::string Path::fileName() const {
    if (parts_.empty()) return "";
    return parts_.back();
}

/** @brief Точка первым символом означает скрытый файл, а не расширение. */
std::string Path::extension() const {
    std::string name = fileName();
    size_t dot = name.rfind('.');
    if (dot == std::string::npos || dot == 0) return "";
    std::string ext = name.substr(dot + 1);
    for (size_t i = 0; i < ext.size(); ++i)
        ext[i] = (char)std::tolower((unsigned char)ext[i]);
    return ext;
}

Path Path::join(const std::string& name) const {
    Path result = *this;
    result.parts_.push_back(name);
    return result;
}

bool Path::startsWith(const Path& prefix) const {
    if (absolute_ != prefix.absolute_) return false;
    if (prefix.parts_.size() > parts_.size()) return false;
    for (size_t i = 0; i < prefix.parts_.size(); ++i)
        if (parts_[i] != prefix.parts_[i]) return false;
    return true;
}

bool Path::isRoot() const { return absolute_ && parts_.empty(); }
bool Path::isAbsolute() const { return absolute_; }
int Path::depth() const { return (int)parts_.size(); }
const std::vector<std::string>& Path::parts() const { return parts_; }

bool Path::operator==(const Path& o) const { return absolute_ == o.absolute_ && parts_ == o.parts_; }
bool Path::operator!=(const Path& o) const { return !(*this == o); }

std::ostream& operator<<(std::ostream& os, const Path& p) { return os << p.toString(); }
