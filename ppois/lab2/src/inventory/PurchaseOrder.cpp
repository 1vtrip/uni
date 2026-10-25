#include "inventory/PurchaseOrder.h"

PurchaseOrder::PurchaseOrder(int id, int supplierId, const Date& date)
    : id_(id), supplierId_(supplierId), date_(date), received_(false) {}

void PurchaseOrder::addLine(const SparePart& part, int qty) {
    for (size_t i = 0; i < parts_.size(); ++i) {
        if (parts_[i] == part) {
            quantities_[i] += qty;
            return;
        }
    }
    parts_.push_back(part);
    quantities_.push_back(qty);
}

Money PurchaseOrder::total() const {
    Money sum;
    for (size_t i = 0; i < parts_.size(); ++i)
        sum += parts_[i].getBuyPrice() * quantities_[i];
    return sum;
}

Money PurchaseOrder::total(const Supplier& supplier) const {
    Money sum;
    for (size_t i = 0; i < parts_.size(); ++i)
        sum += supplier.quote(parts_[i], quantities_[i]);
    return sum;
}

void PurchaseOrder::receive(Warehouse& w, int defaultMinQty) {
    if (received_) return;
    for (size_t i = 0; i < parts_.size(); ++i)
        w.add(parts_[i], quantities_[i], defaultMinQty);
    received_ = true;
}

int PurchaseOrder::totalQty() const {
    int n = 0;
    for (size_t i = 0; i < quantities_.size(); ++i) n += quantities_[i];
    return n;
}

int PurchaseOrder::getId() const { return id_; }
int PurchaseOrder::getSupplierId() const { return supplierId_; }
const Date& PurchaseOrder::getDate() const { return date_; }
bool PurchaseOrder::isReceived() const { return received_; }
const std::vector<SparePart>& PurchaseOrder::getParts() const { return parts_; }
const std::vector<int>& PurchaseOrder::getQuantities() const { return quantities_; }

bool PurchaseOrder::operator==(const PurchaseOrder& other) const { return id_ == other.id_; }
bool PurchaseOrder::operator!=(const PurchaseOrder& other) const { return !(*this == other); }

std::ostream& operator<<(std::ostream& os, const PurchaseOrder& o) {
    os << "Заказ поставщику №" << o.id_ << " от " << o.date_ << ", поставщик #" << o.supplierId_
       << (o.received_ ? ", получен" : ", ожидается");
    for (size_t i = 0; i < o.parts_.size(); ++i)
        os << "\n  " << o.parts_[i].getArticle() << " " << o.parts_[i].getName() << " x" << o.quantities_[i];
    return os;
}
