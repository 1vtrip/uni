#include "people/Schedule.h"

Schedule::Schedule() {}

bool Schedule::hasConflict(const Shift& s) const {
    for (size_t i = 0; i < shifts_.size(); ++i)
        if (shifts_[i].overlaps(s)) return true;
    return false;
}

bool Schedule::add(const Shift& s) {
    if (!s.isValid()) return false;
    if (hasConflict(s)) return false;   // TODO(exc): ScheduleConflictException(дата смены)
    shifts_.push_back(s);
    return true;
}

int Schedule::totalHours() const {
    int sum = 0;
    for (size_t i = 0; i < shifts_.size(); ++i) sum += shifts_[i].hours();
    return sum;
}

int Schedule::shiftsOn(const Date& d) const {
    int n = 0;
    for (size_t i = 0; i < shifts_.size(); ++i)
        if (shifts_[i].getDate() == d) ++n;
    return n;
}

int Schedule::hoursOn(const Date& d) const {
    int sum = 0;
    for (size_t i = 0; i < shifts_.size(); ++i)
        if (shifts_[i].getDate() == d) sum += shifts_[i].hours();
    return sum;
}

const std::vector<Shift>& Schedule::getShifts() const { return shifts_; }

std::ostream& operator<<(std::ostream& os, const Schedule& s) {
    os << "График: смен " << s.shifts_.size() << ", часов " << s.totalHours();
    for (size_t i = 0; i < s.shifts_.size(); ++i)
        os << "\n  " << s.shifts_[i];
    return os;
}
