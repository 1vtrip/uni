#include "finance/PaymentMethod.h"

PaymentMethod::PaymentMethod(const std::string& type, double commission)
    : type_(type), commission_(commission) {}

Money PaymentMethod::commissionFor(const Money& sum) const { return sum.percent(commission_); }

Money PaymentMethod::netAmount(const Money& sum) const { return sum - commissionFor(sum); }

const std::string& PaymentMethod::getType() const { return type_; }
double PaymentMethod::getCommission() const { return commission_; }

bool PaymentMethod::operator==(const PaymentMethod& other) const {
    return type_ == other.type_ && commission_ == other.commission_;
}

bool PaymentMethod::operator!=(const PaymentMethod& other) const { return !(*this == other); }

std::ostream& operator<<(std::ostream& os, const PaymentMethod& m) {
    return os << m.type_ << " (комиссия " << m.commission_ << "%)";
}
