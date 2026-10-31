#include "storage/RecycleBin.h"
#include "exceptions/Exceptions.h"
#include <string>

RecycleBin::RecycleBin(int retentionDays, const DataSize& maxSize)
    : items_(), retentionDays_(retentionDays), maxSize_(maxSize) {
    if (retentionDays <= 0) throw InvalidOperationException("срок хранения в корзине должен быть положительным");
}

int RecycleBin::indexOf(int entryId) const {
    for (size_t i = 0; i < items_.size(); ++i)
        if (items_[i].getEntryId() == entryId) return (int)i;
    return -1;
}

/** @brief Ищется по дате, а не берётся items_[0]: записи могли добавить не по порядку времени. */
int RecycleBin::oldestIndex() const {
    int oldest = -1;
    for (size_t i = 0; i < items_.size(); ++i)
        if (oldest < 0 || items_[i].getDeletedAt() < items_[oldest].getDeletedAt()) oldest = (int)i;
    return oldest;
}

/**
 * @brief Без возврата вытесненных ID объект исчез бы из корзины, но остался бы в FileSystem
 * и продолжал занимать квоту. Новая запись сама вытеснена быть не может: она меньше maxSize_,
 * а пока она не единственная, самой старой будет другая запись.
 */
std::vector<int> RecycleBin::add(const TrashItem& item) {
    if (contains(item.getEntryId()))
        throw EntryAlreadyExistsException("#" + std::to_string(item.getEntryId()) + " в корзине");
    if (maxSize_ < item.getSize())
        throw InvalidOperationException("элемент " + item.getOriginalPath() + " (" + item.getSize().toString()
                                        + ") больше всей корзины (" + maxSize_.toString() + ")");
    items_.push_back(item);
    std::vector<int> evicted;
    while (maxSize_ < size()) {
        int oldest = oldestIndex();
        evicted.push_back(items_[oldest].getEntryId());
        items_.erase(items_.begin() + oldest);
    }
    return evicted;
}

TrashItem RecycleBin::restore(int entryId) {
    int i = indexOf(entryId);
    if (i < 0) throw EntryNotFoundException("#" + std::to_string(entryId) + " в корзине");
    TrashItem item = items_[i];               // копия: после erase ссылка на элемент вектора недействительна
    items_.erase(items_.begin() + i);
    return item;
}

/** @brief После erase следующий элемент сдвигается на место i, поэтому индекс увеличивается только без удаления. */
std::vector<int> RecycleBin::purgeExpired(const Timestamp& now) {
    std::vector<int> removed;
    size_t i = 0;
    while (i < items_.size()) {
        if (items_[i].isExpired(now)) {
            removed.push_back(items_[i].getEntryId());
            items_.erase(items_.begin() + i);
        } else {
            ++i;
        }
    }
    return removed;
}

DataSize RecycleBin::size() const {
    DataSize total;
    for (size_t i = 0; i < items_.size(); ++i) total += items_[i].getSize();
    return total;
}

bool RecycleBin::contains(int entryId) const { return indexOf(entryId) >= 0; }

std::vector<TrashItem> RecycleBin::itemsOf(int userId) const {
    std::vector<TrashItem> result;
    for (size_t i = 0; i < items_.size(); ++i)
        if (items_[i].getDeletedBy() == userId) result.push_back(items_[i]);
    return result;
}

int RecycleBin::count() const { return (int)items_.size(); }
bool RecycleBin::isEmpty() const { return items_.empty(); }
int RecycleBin::getRetentionDays() const { return retentionDays_; }
const DataSize& RecycleBin::getMaxSize() const { return maxSize_; }
const std::vector<TrashItem>& RecycleBin::getItems() const { return items_; }

std::ostream& operator<<(std::ostream& os, const RecycleBin& b) {
    os << "Корзина: " << b.items_.size() << " эл., " << b.size() << " / " << b.maxSize_
       << ", срок " << b.retentionDays_ << " дн.";
    for (size_t i = 0; i < b.items_.size(); ++i) os << "\n  " << b.items_[i];
    return os;
}
