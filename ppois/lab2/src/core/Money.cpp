#include "core/Money.h"
#include <cmath>

Money::Money(long long coins) {
    this->coins_ = coins;
}

long long Money::getCoins() const {
    return coins_;
}

Money Money::fromRubles(double rubles) {
    return Money(std::llround(rubles * 100));
}

double Money::toRubles() const {
    return coins_ / 100.0;
}

bool Money::isZero() const {
    return (this->coins_ == 0);
}

Money Money::percent(double p) const {
    return Money(std::llround(this->coins_ * p / 100.0));
}

Money Money::operator+(const Money& other) const {
    return Money(this->coins_ + other.coins_);
}

Money Money::operator-(const Money& other) const {
    return Money(this->coins_ - other.coins_);
}

Money Money::operator*(double k) const {
    return Money(std::llround(coins_ * k));
}

Money& Money::operator+=(const Money& other) {
    this->coins_ += other.coins_;
    return *this;
}

Money& Money::operator-=(const Money& other) {
    this->coins_ -= other.coins_;
    return *this;
}

bool Money::operator==(const Money& other) const {
    return this->coins_ == other.coins_;
}

bool Money::operator!=(const Money& other) const {
    return this->coins_ != other.coins_;
}

bool Money::operator<(const Money& other) const {
    return this->coins_ < other.coins_;
}

std::ostream& operator<<(std::ostream& os, const Money& m) {
    return os << m.coins_ / 100.0 << " rub";
}