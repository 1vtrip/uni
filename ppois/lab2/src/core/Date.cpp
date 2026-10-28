#include "core/Date.h"
#include <sstream>
#include "exceptions/Exceptions.h"

Date::Date(int d, int m, int y) : day_(d), month_(m), year_(y) {
    if (!isValid()) {
        std::ostringstream out;
        out << (d < 10 ? "0" : "") << d << "." << (m < 10 ? "0" : "") << m << "." << y;
        throw InvalidDateException(out.str());
    }
}

bool Date::isLeap(int y) {
    return (y % 4 == 0 && y % 100 != 0) || y % 400 == 0;
}

int Date::daysInMonth(int m, int y) {
    switch (m) {
        case 2:
            return isLeap(y) ? 29 : 28;
        case 4: case 6: case 9: case 11:
            return 30;
        default:
            return 31;
    }
}

bool Date::isValid() const {
    if (month_ < 1 || month_ > 12) return false;
    return day_ >= 1 && day_ <= daysInMonth(month_, year_);
}

/** @brief Сумма дней всех полных прошлых лет, прошлых месяцев текущего года и day_. */
int Date::toDays() const {
    int days = 0;
    for (int y = 1; y < year_; ++y)
        days += isLeap(y) ? 366 : 365;
    for (int m = 1; m < month_; ++m)
        days += daysInMonth(m, year_);
    return days + day_;
}

int Date::daysTo(const Date& other) const {
    return other.toDays() - toDays();
}

Date Date::addDays(int n) const {
    Date result = *this;
    for (int i = 0; i < n; ++i) {
        ++result.day_;
        if (result.day_ > daysInMonth(result.month_, result.year_)) {
            result.day_ = 1;
            ++result.month_;
            if (result.month_ > 12) {
                result.month_ = 1;
                ++result.year_;
            }
        }
    }
    return result;
}

/**
 * @brief Месяц переводится в одно число: год * 12 + (месяц - 1), месяцы от 0 до 11.
 * Если дня нет в новом месяце, ставится последний: 31.01 + 1 месяц -> 28.02.
 */
Date Date::addMonths(int n) const {
    int index = year_ * 12 + (month_ - 1) + n;
    int year = index / 12;
    int month = index % 12 + 1;
    int day = day_;
    int last = daysInMonth(month, year);
    if (day > last) day = last;
    return Date(day, month, year);
}

/** @brief Разница в месяцах; если последний месяц прошёл не полностью, он не считается. */
int Date::monthsTo(const Date& other) const {
    int months = (other.year_ - year_) * 12 + (other.month_ - month_);
    if (other.day_ < day_) --months;
    return months;
}

int Date::getDay() const { return day_; }
int Date::getMonth() const { return month_; }
int Date::getYear() const { return year_; }

bool Date::operator<(const Date& other) const {
    if (year_ != other.year_) return year_ < other.year_;
    if (month_ != other.month_) return month_ < other.month_;
    return day_ < other.day_;
}

bool Date::operator==(const Date& other) const {
    return day_ == other.day_ && month_ == other.month_ && year_ == other.year_;
}

bool Date::operator!=(const Date& other) const {
    return !(*this == other);
}

std::ostream& operator<<(std::ostream& os, const Date& d) {
    if (d.day_ < 10) os << '0';
    os << d.day_ << '.';
    if (d.month_ < 10) os << '0';
    return os << d.month_ << '.' << d.year_;
}
