#pragma once
#include <iostream>

/** @brief Момент времени с точностью до минуты: дата и время суток. */
class Timestamp {
public:
    /**
     * @brief Создать момент времени.
     * @throws InvalidOperationException такой даты или времени не существует (см. isValid).
     */
    Timestamp(int day = 1, int month = 1, int year = 2000, int hour = 0, int minute = 0);

    /** @brief Високосный ли год. */
    static bool isLeap(int y);
    /** @brief Число дней в месяце m года y. */
    static int daysInMonth(int m, int y);

    /** @brief Существует ли такой момент: месяц 1–12, день в пределах месяца, час 0–23, минута 0–59. */
    bool isValid() const;
    /** @brief Минуты от условного начала: дни × 1440 + hour × 60 + minute. */
    long long toMinutes() const;
    /** @brief Сколько минут от этого момента до o (отрицательно, если o раньше). */
    long long minutesTo(const Timestamp& o) const;
    /** @brief Сколько полных суток от этого момента до o. */
    int daysTo(const Timestamp& o) const;
    /** @brief Момент через n минут (n может быть отрицательным). */
    Timestamp addMinutes(int n) const;
    /** @brief Момент через n дней, время суток то же (n может быть отрицательным). */
    Timestamp addDays(int n) const;

    int getDay() const;
    int getMonth() const;
    int getYear() const;
    int getHour() const;
    int getMinute() const;

    bool operator<(const Timestamp& o) const;
    bool operator<=(const Timestamp& o) const;
    bool operator==(const Timestamp& o) const;
    bool operator!=(const Timestamp& o) const;
    /** @brief Вывод в формате ДД.ММ.ГГГГ ЧЧ:ММ. */
    friend std::ostream& operator<<(std::ostream& os, const Timestamp& t);

private:
    /** @brief Порядковый номер дня, считая 01.01.0001 первым (как Date::toDays из лабы 2). */
    int toDays() const;

    int year_;
    int month_;
    int day_;
    int hour_;
    int minute_;
};
