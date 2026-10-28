#include "people/Schedule.h"
#include <sstream>
#include "exceptions/Exceptions.h"

Schedule::Schedule() {}

bool Schedule::hasConflict(const Shift& s) const {
    for (size_t i = 0; i < shifts_.size(); ++i)
        if (shifts_[i].overlaps(s)) return true;
    return false;
}

void Schedule::add(const Shift& s) {
    std::ostringstream what;
    what << "смена " << s;
    if (!s.isValid()) throw ScheduleConflictException(what.str() + " — некорректные часы");
    if (hasConflict(s)) throw ScheduleConflictException(what.str() + " пересекается с другой");
    shifts_.push_back(s);
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
