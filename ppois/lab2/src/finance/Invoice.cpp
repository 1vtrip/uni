#include "finance/Invoice.h"

Invoice::Invoice(int id, int orderId, int clientId, const Date& date)
    : id_(id), orderId_(orderId), clientId_(clientId), date_(date),
      discount_(), hasDiscount_(false), paid_(false) {}

bool Invoice::addLine(const InvoiceLine& line) {
    if (paid_) return false;
    lines_.push_back(line);
    return true;
}

bool Invoice::applyDiscount(const Discount& d, const Date& today) {
    if (paid_) return false;
    if (!d.isApplicable(subtotal(), today))
        return false;   // TODO(exc): InvalidDiscountException(d.getCode())
    discount_ = d;
    hasDiscount_ = true;
    return true;
}

Money Invoice::subtotal() const {
    Money sum;
    for (size_t i = 0; i < lines_.size(); ++i) sum += lines_[i].sum();
    return sum;
}

Money Invoice::total() const {
    Money sum = subtotal();
    if (hasDiscount_) sum -= discount_.amount(sum);
    return sum;
}

bool Invoice::isOverdue(const Date& today, int termDays) const {
    return !paid_ && date_.daysTo(today) > termDays;
}

void Invoice::markPaid() { paid_ = true; }

int Invoice::getId() const { return id_; }
int Invoice::getOrderId() const { return orderId_; }
int Invoice::getClientId() const { return clientId_; }
const std::vector<InvoiceLine>& Invoice::getLines() const { return lines_; }
const Date& Invoice::getDate() const { return date_; }
bool Invoice::hasDiscount() const { return hasDiscount_; }
const Discount& Invoice::getDiscount() const { return discount_; }
bool Invoice::isPaid() const { return paid_; }

bool Invoice::operator==(const Invoice& other) const { return id_ == other.id_; }
bool Invoice::operator!=(const Invoice& other) const { return !(*this == other); }

std::ostream& operator<<(std::ostream& os, const Invoice& inv) {
    os << "Счёт №" << inv.id_ << " от " << inv.date_ << " (заказ №" << inv.orderId_
       << ", клиент #" << inv.clientId_ << ")";
    for (size_t i = 0; i < inv.lines_.size(); ++i)
        os << "\n  " << inv.lines_[i];
    if (inv.hasDiscount_)
        os << "\n  Скидка " << inv.discount_.getCode() << " " << inv.discount_.getPercent()
           << "%: -" << inv.discount_.amount(inv.subtotal());
    os << "\n  Итого: " << inv.total() << (inv.paid_ ? " [ОПЛАЧЕН]" : " [не оплачен]");
    return os;
}
