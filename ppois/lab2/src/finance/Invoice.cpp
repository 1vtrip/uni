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

Money Invoice::amountDue() const { return paid_ ? Money() : total(); }

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

void Invoice::print(std::ostream& os) const {
    os << "Счёт №" << id_ << " от " << date_ << " (заказ №" << orderId_
       << ", клиент #" << clientId_ << ")";
    for (size_t i = 0; i < lines_.size(); ++i)
        os << "\n  " << lines_[i];
    if (hasDiscount_)
        os << "\n  Скидка " << discount_.getCode() << " " << discount_.getPercent()
           << "%: -" << discount_.amount(subtotal());
    os << "\n  Итого: " << total() << (paid_ ? " [ОПЛАЧЕН]" : " [не оплачен]");
}
