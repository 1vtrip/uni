#pragma once
#include <iostream>
#include "core/Date.h"

/** @brief Рабочая смена: дата и часы с startHour до endHour. */
class Shift {
public:
    Shift(const Date& date, int startHour, int endHour);

    /** @brief Длительность смены в часах. */
    int hours() const;
    /** @brief Пересекается ли с другой сменой: тот же день и отрезки часов накладываются. */
    bool overlaps(const Shift& other) const;
    /** @brief Корректна ли смена: часы от 0 до 24 и начало раньше конца. */
    bool isValid() const;

    const Date& getDate() const;
    int getStartHour() const;
    int getEndHour() const;

    bool operator==(const Shift& other) const;
    bool operator!=(const Shift& other) const;
    friend std::ostream& operator<<(std::ostream& os, const Shift& s);

private:
    Date date_;
    int startHour_;
    int endHour_;
};
