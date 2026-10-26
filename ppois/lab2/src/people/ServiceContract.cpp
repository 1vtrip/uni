#include "people/ServiceContract.h"

ServiceContract::ServiceContract(const std::string& number, const Date& start, const Date& end,
                                 double discountPercent, int slaDays)
    : number_(number), start_(start), end_(end), discountPercent_(discountPercent), slaDays_(slaDays) {}

bool ServiceContract::isActive(const Date& today) const {
    return !(today < start_) && !(end_ < today);
}

Date ServiceContract::deadline(const Date& from) const { return from.addDays(slaDays_); }

bool ServiceContract::prolong(int months) {
    if (months <= 0) return false;
    end_ = end_.addMonths(months);
    return true;
}

Money ServiceContract::discountFor(const Money& sum) const { return sum.percent(discountPercent_); }

const std::string& ServiceContract::getNumber() const { return number_; }
const Date& ServiceContract::getStart() const { return start_; }
const Date& ServiceContract::getEnd() const { return end_; }
double ServiceContract::getDiscountPercent() const { return discountPercent_; }
int ServiceContract::getSlaDays() const { return slaDays_; }

bool ServiceContract::operator==(const ServiceContract& other) const { return number_ == other.number_; }
bool ServiceContract::operator!=(const ServiceContract& other) const { return !(*this == other); }

std::ostream& operator<<(std::ostream& os, const ServiceContract& c) {
    return os << "Договор " << c.number_ << ": " << c.start_ << " - " << c.end_
              << ", скидка " << c.discountPercent_ << "%, ремонт за " << c.slaDays_ << " дн.";
}
