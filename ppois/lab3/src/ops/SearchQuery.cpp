#include "ops/SearchQuery.h"
#include "core/Path.h"
#include "exceptions/Exceptions.h"
#include "fs/FsEntry.h"
#include <cctype>
#include <climits>

/** @brief Максимум по умолчанию огромный: значение 0 не годится для «без ограничения», иначе не найти пустые файлы. */
SearchQuery::SearchQuery()
    : namePattern_(), extension_(), minSize_(0), maxSize_(LLONG_MAX), modifiedAfter_(),
      useDateFilter_(false), ownerId_(0), includeHidden_(false) {}

/**
 * @brief Жадное сравнение с откатом. Встретив *, запоминаем её место (star) и место в тексте (mark)
 * и сначала считаем, что она совпала с пустой строкой. При несовпадении откатываемся:
 * звёздочка «съедает» ещё один символ текста (++mark), сравнение идёт с символа после неё.
 */
bool SearchQuery::wildcardMatch(const std::string& p, const std::string& t) {
    size_t pi = 0, ti = 0, star = std::string::npos, mark = 0;
    while (ti < t.size()) {
        if (pi < p.size() && (p[pi] == '?' || p[pi] == t[ti])) { ++pi; ++ti; }
        else if (pi < p.size() && p[pi] == '*') { star = pi++; mark = ti; }
        else if (star != std::string::npos) { pi = star + 1; ti = ++mark; }
        else return false;
    }
    while (pi < p.size() && p[pi] == '*') ++pi;
    return pi == p.size();
}

std::string SearchQuery::lower(const std::string& s) {
    std::string result = s;
    for (size_t i = 0; i < result.size(); ++i) result[i] = (char)std::tolower((unsigned char)result[i]);
    return result;
}

/**
 * @brief Расширение берётся через Path, а не через File::extension(): у FsEntry такого метода нет,
 * а искать нужно по любым элементам. Имя элемента — допустимое имя, поэтому Path::parse не бросит.
 */
bool SearchQuery::matches(const FsEntry& e) const {
    if (!includeHidden_ && e.isHidden()) return false;
    if (ownerId_ != 0 && e.getOwnerId() != ownerId_) return false;
    if (!namePattern_.empty() && !wildcardMatch(lower(namePattern_), lower(e.getName()))) return false;
    if (!extension_.empty() && (e.getName().empty() || Path::parse(e.getName()).extension() != extension_))
        return false;
    DataSize size = e.size();
    if (size < minSize_ || maxSize_ < size) return false;
    if (useDateFilter_ && e.getModified() < modifiedAfter_) return false;
    return true;
}

SearchQuery& SearchQuery::byName(const std::string& pattern) {
    namePattern_ = pattern;
    return *this;
}

SearchQuery& SearchQuery::byExtension(const std::string& ext) {
    extension_ = lower(!ext.empty() && ext[0] == '.' ? ext.substr(1) : ext);
    return *this;
}

SearchQuery& SearchQuery::bySize(const DataSize& min, const DataSize& max) {
    if (max < min) throw InvalidOperationException("минимальный размер больше максимального");
    minSize_ = min;
    maxSize_ = max;
    return *this;
}

SearchQuery& SearchQuery::modifiedSince(const Timestamp& since) {
    modifiedAfter_ = since;
    useDateFilter_ = true;
    return *this;
}

SearchQuery& SearchQuery::byOwner(int ownerId) {
    ownerId_ = ownerId;
    return *this;
}

SearchQuery& SearchQuery::withHidden(bool include) {
    includeHidden_ = include;
    return *this;
}

const std::string& SearchQuery::getNamePattern() const { return namePattern_; }
const std::string& SearchQuery::getExtension() const { return extension_; }
const DataSize& SearchQuery::getMinSize() const { return minSize_; }
const DataSize& SearchQuery::getMaxSize() const { return maxSize_; }
int SearchQuery::getOwnerId() const { return ownerId_; }
bool SearchQuery::includesHidden() const { return includeHidden_; }

std::ostream& operator<<(std::ostream& os, const SearchQuery& q) {
    bool any = false;
    if (!q.namePattern_.empty()) { os << "имя " << q.namePattern_; any = true; }
    if (!q.extension_.empty()) { os << (any ? ", " : "") << "расширение " << q.extension_; any = true; }
    if (!q.minSize_.isZero() || q.maxSize_ != DataSize(LLONG_MAX)) {
        os << (any ? ", " : "") << "размер " << q.minSize_ << "..";
        if (q.maxSize_ == DataSize(LLONG_MAX)) os << "∞";
        else os << q.maxSize_;
        any = true;
    }
    if (q.useDateFilter_) { os << (any ? ", " : "") << "изменён с " << q.modifiedAfter_; any = true; }
    if (q.ownerId_ != 0) { os << (any ? ", " : "") << "владелец #" << q.ownerId_; any = true; }
    if (q.includeHidden_) { os << (any ? ", " : "") << "со скрытыми"; any = true; }
    if (!any) os << "всё";
    return os;
}
