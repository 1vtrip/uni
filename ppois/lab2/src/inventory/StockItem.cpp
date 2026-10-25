#include "inventory/StockItem.h"
#include <algorithm>

StockItem::StockItem(const SparePart& part, int qty, int minQty)
    : part_(part), qty_(qty), reserved_(0), minQty_(minQty) {}

int StockItem::available() const { return qty_ - reserved_; }

bool StockItem::reserve(int qty) {
    if (qty > available()) return false;   // TODO(exc): OutOfStockException(part_.getArticle(), qty, available())
    reserved_ += qty;
    return true;
}

void StockItem::release(int qty) {
    reserved_ -= std::min(qty, reserved_);
}

void StockItem::writeOff(int qty) {
    int n = std::min(qty, reserved_);
    reserved_ -= n;
    qty_ -= n;
}

void StockItem::restock(int qty) { qty_ += qty; }

bool StockItem::needsRestock() const { return available() < minQty_; }

const SparePart& StockItem::getPart() const { return part_; }
int StockItem::getQty() const { return qty_; }
int StockItem::getReserved() const { return reserved_; }
int StockItem::getMinQty() const { return minQty_; }

bool StockItem::operator==(const StockItem& other) const { return part_ == other.part_; }
bool StockItem::operator!=(const StockItem& other) const { return !(*this == other); }

std::ostream& operator<<(std::ostream& os, const StockItem& s) {
    return os << s.part_.getArticle() << " " << s.part_.getName()
              << ": " << s.qty_ << " шт. (резерв " << s.reserved_ << ")"
              << (s.needsRestock() ? " [ДОЗАКАЗАТЬ]" : "");
}