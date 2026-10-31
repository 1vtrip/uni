#include "ops/SearchResult.h"
#include "fs/FsEntry.h"

SearchResult::SearchResult() : ids_(), sizes_(), totalSize_() {}

/** @brief Без проверки на повтор общий размер посчитал бы один файл дважды. */
void SearchResult::add(const FsEntry& e) {
    if (contains(e.getId())) return;
    DataSize size = e.size();
    ids_.push_back(e.getId());
    sizes_.push_back(size);
    totalSize_ += size;
}

bool SearchResult::contains(int id) const {
    for (size_t i = 0; i < ids_.size(); ++i)
        if (ids_[i] == id) return true;
    return false;
}

int SearchResult::count() const { return (int)ids_.size(); }
bool SearchResult::isEmpty() const { return ids_.empty(); }
const DataSize& SearchResult::totalSize() const { return totalSize_; }

/**
 * @brief Элемент i вынимается и сдвигается влево, пока слева стоит элемент строго меньше.
 * Строгое сравнение (<, а не <=) и делает сортировку устойчивой.
 */
void SearchResult::sortDesc(std::vector<int>& ids, std::vector<DataSize>& sizes) {
    for (size_t i = 1; i < sizes.size(); ++i) {
        int id = ids[i];
        DataSize size = sizes[i];
        size_t j = i;
        while (j > 0 && sizes[j - 1] < size) {
            sizes[j] = sizes[j - 1];
            ids[j] = ids[j - 1];
            --j;
        }
        sizes[j] = size;
        ids[j] = id;
    }
}

void SearchResult::sortBySizeDesc() { sortDesc(ids_, sizes_); }

std::vector<int> SearchResult::top(int n) const {
    std::vector<int> ids = ids_;
    std::vector<DataSize> sizes = sizes_;
    sortDesc(ids, sizes);
    if (n <= 0) return std::vector<int>();
    if (n < (int)ids.size()) ids.resize(n);
    return ids;
}

const std::vector<int>& SearchResult::getIds() const { return ids_; }
const std::vector<DataSize>& SearchResult::getSizes() const { return sizes_; }

std::ostream& operator<<(std::ostream& os, const SearchResult& r) {
    os << "Найдено: " << r.ids_.size() << ", всего " << r.totalSize_;
    for (size_t i = 0; i < r.ids_.size(); ++i) os << "\n  #" << r.ids_[i] << "  " << r.sizes_[i];
    return os;
}
