#pragma once
#include <iostream>

/** @brief Календарная дата (день, месяц, год) с арифметикой дней и месяцев. */
class Date {
public:
    /** @brief Создать дату. Проверка корректности появится на этапе «Переход». */
    Date(int d = 1, int m = 1, int y = 2000);

    /** @brief Високосный ли год. */
    static bool isLeap(int y);
    /** @brief Число дней в месяце m года y. */
    static int daysInMonth(int m, int y);

    /** @brief Существует ли такая дата (месяц 1–12, день в пределах месяца). */
    bool isValid() const;
    /** @brief Порядковый номер дня, считая 01.01.0001 первым днём. */
    int toDays() const;
    /** @brief Сколько дней от этой даты до other (отрицательно, если other раньше). */
    int daysTo(const Date& other) const;
    /** @brief Дата через n дней (n >= 0). */
    Date addDays(int n) const;
    /** @brief Дата через n месяцев; если дня нет в новом месяце, берётся последний. */
    Date addMonths(int n) const;
    /** @brief Число полных месяцев от этой даты до other. */
    int monthsTo(const Date& other) const;

    int getDay() const;
    int getMonth() const;
    int getYear() const;

    bool operator<(const Date& other) const;
    bool operator==(const Date& other) const;
    bool operator!=(const Date& other) const;
    friend std::ostream& operator<<(std::ostream& os, const Date& d);

private:
    int day_;
    int month_;
    int year_;
};
