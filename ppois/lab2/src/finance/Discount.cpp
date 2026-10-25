#include "finance/Discount.h"

Discount::Discount() : code_(""), percent_(0), until_(), minSum_(), approved_(false) {}

Discount::Discount(const std::string& code, double percent, const Date& until, const Money& minSum)
    : code_(code), percent_(percent), until_(until), minSum_(minSum), approved_(false) {}

bool Discount::isApplicable(const Money& sum, const Date& today) const {
    return approved_ && !(until_ < today) && !(sum < minSum_);
}

Money Discount::amount(const Money& sum) const { return sum.percent(percent_); }

void Discount::approve() { approved_ = true; }

const std::string& Discount::getCode() const { return code_; }
double Discount::getPercent() const { return percent_; }
const Date& Discount::getUntil() const { return until_; }
const Money& Discount::getMinSum() const { return minSum_; }
bool Discount::isApproved() const { return approved_; }

bool Discount::operator==(const Discount& other) const { return code_ == other.code_; }
bool Discount::operator!=(const Discount& other) const { return !(*this == other); }

std::ostream& operator<<(std::ostream& os, const Discount& d) {
    return os << "Скидка " << d.code_ << " " << d.percent_ << "% до " << d.until_
              << ", от " << d.minSum_ << (d.approved_ ? ", одобрена" : ", не одобрена");
}
