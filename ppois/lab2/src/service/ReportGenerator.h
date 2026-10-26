#
pragma once
#include <string>
#include <vector>
#include <iostream>
#include "core/Date.h"
#include "core/Money.h"

/** @brief Forward declaration: в заголовке нужны только как ссылки. */
class Invoice;
class RepairOrder;
class Technician;
class Employee;
class Warehouse;

/** @brief Отчёты за период [from, to]. Ничего не меняет и не печатает: возвращает суммы и строки. */
class ReportGenerator {
public:
    ReportGenerator(const Date& from, const Date& to);

    /** @brief Попадает ли дата в период (обе границы включительно). */
    bool inPeriod(const Date& d) const;
    /** @brief Выручка: сумма оплаченных счетов с датой в периоде. */
    Money revenue(const std::vector<Invoice>& all) const;
    /** @brief Статистика мастера: уровень, закрытые задачи и часы по заказам, принятым в периоде. */
    std::string technicianStats(const Technician& t, const std::vector<RepairOrder>& orders) const;
    /** @brief Отчёт по складу: стоимость и позиции, которые нужно дозаказать. */
    std::string stockReport(const Warehouse& w) const;
    /** @brief Зарплата сотрудника за месяц. Полиморфный вызов calculateSalary. */
    Money salaryOf(const Employee& e, int month) const;
    /** @brief Сколько заказов просрочено на дату today. */
    int overdueCount(const std::vector<RepairOrder>& orders, const Date& today) const;

    const Date& getFrom() const;
    const Date& getTo() const;

    friend std::ostream& operator<<(std::ostream& os, const ReportGenerator& r);

private:
    Date from_;
    Date to_;
};
