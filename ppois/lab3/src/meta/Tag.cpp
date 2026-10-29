#include "meta/Tag.h"
#include <cctype>

Tag::Tag(const std::string& name, const std::string& color) : name_(name), color_(color) {}

/**
 * @brief Буквы считаются по UTF-8: байты продолжения имеют вид 10xxxxxx и не считаются,
 * поэтому "важное" — 6 букв, хотя size() равен 12.
 */
bool Tag::isValidName(const std::string& n) {
    int letters = 0;
    for (size_t i = 0; i < n.size(); ++i) {
        unsigned char c = (unsigned char)n[i];
        if (std::isspace(c)) return false;
        if ((c & 0xC0) != 0x80) ++letters;
    }
    return letters >= 1 && letters <= 30;
}

bool Tag::isValidColor(const std::string& c) {
    const int count = 7;
    const char* colors[count] = {"red", "orange", "yellow", "green", "blue", "purple", "gray"};
    for (int i = 0; i < count; ++i)
        if (c == colors[i]) return true;
    return false;
}

/** @brief Обе строки сравниваются посимвольно в нижнем регистре; tolower меняет только латиницу. */
bool Tag::matches(const std::string& query) const {
    if (query.size() != name_.size()) return false;
    for (size_t i = 0; i < name_.size(); ++i)
        if (std::tolower((unsigned char)name_[i]) != std::tolower((unsigned char)query[i])) return false;
    return true;
}

bool Tag::setColor(const std::string& color) {
    if (!isValidColor(color)) return false;     // TODO(exc): недопустимый цвет
    color_ = color;
    return true;
}

const std::string& Tag::getName() const { return name_; }
const std::string& Tag::getColor() const { return color_; }

bool Tag::operator==(const Tag& o) const { return matches(o.name_); }
bool Tag::operator!=(const Tag& o) const { return !(*this == o); }
std::ostream& operator<<(std::ostream& os, const Tag& t) { return os << '#' << t.name_ << '(' << t.color_ << ')'; }
