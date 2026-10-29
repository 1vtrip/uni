#include "core/Timestamp.h"
#include <iomanip>

Timestamp::Timestamp(int day, int month, int year, int hour, int minute)
    : year_(year), month_(month), day_(day), hour_(hour), minute_(minute) {}

bool Timestamp::isLeap(int y) {
    return (y % 4 == 0 && y % 100 != 0) || y % 400 == 0;
}

int Timestamp::daysInMonth(int m, int y) {
    switch (m) {
        case 2:
            return isLeap(y) ? 29 : 28;
        case 4: case 6: case 9: case 11:
            return 30;
        default:
            return 31;
    }
}

bool Timestamp::isValid() const {
    if (month_ < 1 || month_ > 12) return false;
    if (day_ < 1 || day_ > daysInMonth(month_, year_)) return false;
    return hour_ >= 0 && hour_ <= 23 && minute_ >= 0 && minute_ <= 59;
}

/** @brief Сумма дней всех полных прошлых лет, прошлых месяцев текущего года и day_. */
int Timestamp::toDays() const {
    int days = 0;
    for (int y = 1; y < year_; ++y)
        days += isLeap(y) ? 366 : 365;
    for (int m = 1; m < month_; ++m)
        days += daysInMonth(m, year_);
    return days + day_;
}

long long Timestamp::toMinutes() const {
    return (long long)toDays() * 1440 + hour_ * 60 + minute_;
}

long long Timestamp::minutesTo(const Timestamp& o) const {
    return o.toMinutes() - toMinutes();
}

int Timestamp::daysTo(const Timestamp& o) const {
    return (int)(minutesTo(o) / 1440);
}

/**
 * @brief Время суток переводится в минуты от полуночи и к нему прибавляется n.
 * Целые сутки (carry) переносятся через addDays, остаток (rest) даёт часы и минуты.
 * Для отрицательного остатка занимаем одни сутки: -10 мин -> 23:50 предыдущего дня.
 */
Timestamp Timestamp::addMinutes(int n) const {
    int total = hour_ * 60 + minute_ + n;
    int carry = total / 1440;
    int rest = total % 1440;
    if (rest < 0) {
        rest += 1440;
        --carry;
    }
    Timestamp result = addDays(carry);
    result.hour_ = rest / 60;
    result.minute_ = rest % 60;
    return result;
}

/** @brief Вперёд: день за днём с переносом месяца и года. Назад: при day_ == 0 уходим в конец прошлого месяца. */
Timestamp Timestamp::addDays(int n) const {
    Timestamp result = *this;
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
    for (int i = 0; i > n; --i) {
        --result.day_;
        if (result.day_ < 1) {
            --result.month_;
            if (result.month_ < 1) {
                result.month_ = 12;
                --result.year_;
            }
            result.day_ = daysInMonth(result.month_, result.year_);
        }
    }
    return result;
}

int Timestamp::getDay() const { return day_; }
int Timestamp::getMonth() const { return month_; }
int Timestamp::getYear() const { return year_; }
int Timestamp::getHour() const { return hour_; }
int Timestamp::getMinute() const { return minute_; }

bool Timestamp::operator<(const Timestamp& o) const { return toMinutes() < o.toMinutes(); }
bool Timestamp::operator<=(const Timestamp& o) const { return toMinutes() <= o.toMinutes(); }
bool Timestamp::operator==(const Timestamp& o) const { return toMinutes() == o.toMinutes(); }
bool Timestamp::operator!=(const Timestamp& o) const { return !(*this == o); }

std::ostream& operator<<(std::ostream& os, const Timestamp& t) {
    char oldFill = os.fill('0');
    os << std::setw(2) << t.day_ << '.' << std::setw(2) << t.month_ << '.'
       << std::setw(4) << t.year_ << ' '
       << std::setw(2) << t.hour_ << ':' << std::setw(2) << t.minute_;
    os.fill(oldFill);
    return os;
}
