#pragma once
#include <vector>
#include <iostream>
#include "core/Date.h"
#include "people/Shift.h"

/** @brief График сотрудника: список смен без пересечений. */
class Schedule {
public:
    Schedule();

    /** @brief Пересекается ли смена s хотя бы с одной сменой графика. */
    bool hasConflict(const Shift& s) const;
    /** @brief Добавить смену; false, если она некорректна или пересекается с другой. */
    bool add(const Shift& s);   // TODO(exc): ScheduleConflictException
    /** @brief Сколько всего часов во всех сменах. */
    int totalHours() const;
    /** @brief Сколько смен в день d. */
    int shiftsOn(const Date& d) const;
    /** @brief Сколько часов в день d. */
    int hoursOn(const Date& d) const;

    const std::vector<Shift>& getShifts() const;

    friend std::ostream& operator<<(std::ostream& os, const Schedule& s);

private:
    std::vector<Shift> shifts_;
};
