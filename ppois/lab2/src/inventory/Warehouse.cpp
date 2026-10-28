#include "inventory/Warehouse.h"
#include "exceptions/Exceptions.h"

Warehouse::Warehouse(const Address& address) : address_(address) {}

void Warehouse::add(const SparePart& part, int qty, int minQty) {
    int i = indexOf(part.getArticle());
    if (i != -1)
        items_[i].restock(qty);
    else
        items_.push_back(StockItem(part, qty, minQty));
}

bool Warehouse::contains(const std::string& article) const {
    return indexOf(article) != -1;
}

int Warehouse::indexOf(const std::string& article) const {
    for (size_t i = 0; i < items_.size(); ++i)
        if (items_[i].getPart().getArticle() == article) return (int)i;
    return -1;
}

StockItem& Warehouse::find(const std::string& article) {
    int i = indexOf(article);
    if (i == -1) throw OutOfStockException(article, 1, 0);
    return items_[i];
}

const StockItem& Warehouse::find(const std::string& article) const {
    int i = indexOf(article);
    if (i == -1) throw OutOfStockException(article, 1, 0);
    return items_[i];
}

/** @brief find и reserve бросают до каких-либо изменений, поэтому при ошибке склад не меняется. */
SparePart Warehouse::take(const std::string& article, int qty) {
    StockItem& item = find(article);
    item.reserve(qty);
    item.writeOff(qty);
    return item.getPart();
}

std::vector<StockItem> Warehouse::lowStock() const {
    std::vector<StockItem> result;
    for (size_t i = 0; i < items_.size(); ++i)
        if (items_[i].needsRestock()) result.push_back(items_[i]);
    return result;
}

Money Warehouse::totalValue() const {
    Money total;
    for (size_t i = 0; i < items_.size(); ++i)
        total += items_[i].getPart().getBuyPrice() * items_[i].getQty();
    return total;
}

const Address& Warehouse::getAddress() const { return address_; }
const std::vector<StockItem>& Warehouse::getItems() const { return items_; }

std::ostream& operator<<(std::ostream& os, const Warehouse& w) {
    os << "Склад: " << w.address_ << ", позиций " << w.items_.size();
    for (size_t i = 0; i < w.items_.size(); ++i)
        os << "\n  " << w.items_[i];
    return os;
}
