#include "people/Shift.h"

Shift::Shift(const Date& date, int startHour, int endHour)
    : date_(date), startHour_(startHour), endHour_(endHour) {}

int Shift::hours() const { return endHour_ - startHour_; }

/** @brief Отрезки [s1, e1) и [s2, e2) пересекаются, если s1 < e2 и s2 < e1. Смены 9-13 и 13-18 соседние. */
bool Shift::overlaps(const Shift& other) const {
    return date_ == other.date_ && startHour_ < other.endHour_ && other.startHour_ < endHour_;
}

bool Shift::isValid() const {
    return startHour_ >= 0 && endHour_ <= 24 && startHour_ < endHour_;
}

const Date& Shift::getDate() const { return date_; }
int Shift::getStartHour() const { return startHour_; }
int Shift::getEndHour() const { return endHour_; }

bool Shift::operator==(const Shift& other) const {
    return date_ == other.date_ && startHour_ == other.startHour_ && endHour_ == other.endHour_;
}

bool Shift::operator!=(const Shift& other) const { return !(*this == other); }

std::ostream& operator<<(std::ostream& os, const Shift& s) {
    return os << s.date_ << " " << s.startHour_ << ":00-" << s.endHour_ << ":00";
}
