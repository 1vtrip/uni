#include "meta/Metadata.h"

Metadata::Metadata() : keys_(), values_(), tags_() {}

int Metadata::indexOf(const std::string& key) const {
    for (size_t i = 0; i < keys_.size(); ++i)
        if (keys_[i] == key) return (int)i;
    return -1;
}

int Metadata::tagIndexOf(const std::string& name) const {
    for (size_t i = 0; i < tags_.size(); ++i)
        if (tags_[i].matches(name)) return (int)i;
    return -1;
}

/** @brief Новый ключ добавляется сразу в оба вектора, чтобы они оставались одной длины. */
bool Metadata::set(const std::string& key, const std::string& value) {
    if (key.empty()) return false;          // TODO(exc): пустой ключ
    int i = indexOf(key);
    if (i >= 0) {
        values_[i] = value;
    } else {
        keys_.push_back(key);
        values_.push_back(value);
    }
    return true;
}

std::string Metadata::get(const std::string& key) const {
    int i = indexOf(key);
    return i >= 0 ? values_[i] : "";
}

/** @brief Удаляется по одному индексу из обоих векторов, иначе ключи и значения перепутаются. */
bool Metadata::remove(const std::string& key) {
    int i = indexOf(key);
    if (i < 0) return false;
    keys_.erase(keys_.begin() + i);
    values_.erase(values_.begin() + i);
    return true;
}

bool Metadata::has(const std::string& key) const { return indexOf(key) >= 0; }

bool Metadata::addTag(const Tag& t) {
    if (tagIndexOf(t.getName()) >= 0) return false;
    tags_.push_back(t);
    return true;
}

bool Metadata::removeTag(const std::string& name) {
    int i = tagIndexOf(name);
    if (i < 0) return false;
    tags_.erase(tags_.begin() + i);
    return true;
}

bool Metadata::hasTag(const std::string& name) const { return tagIndexOf(name) >= 0; }

int Metadata::size() const { return (int)keys_.size(); }
const std::vector<std::string>& Metadata::keys() const { return keys_; }
const std::vector<Tag>& Metadata::tags() const { return tags_; }

std::ostream& operator<<(std::ostream& os, const Metadata& m) {
    if (m.keys_.empty() && m.tags_.empty()) return os << "(пусто)";
    bool first = true;
    for (size_t i = 0; i < m.keys_.size(); ++i) {
        if (!first) os << "; ";
        os << m.keys_[i] << '=' << m.values_[i];
        first = false;
    }
    for (size_t i = 0; i < m.tags_.size(); ++i) {
        if (!first) os << "; ";
        os << m.tags_[i];
        first = false;
    }
    return os;
}
