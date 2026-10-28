#pragma once
#include <string>
#include "people/Employee.h"
#include "finance/Invoice.h"
#include "finance/Salary.h"

/** @brief Forward declaration: в заголовке нужны только как ссылки. */
class RepairOrder;
class PriceList;
class Payment;
class CashRegister;

/** @brief Бухгалтер: выставляет счета по заказам, начисляет зарплату, оформляет возвраты. */
class Accountant : public Employee {
public:
    Accountant(int id, const std::string& name, const ContactInfo& contacts, const Date& birth,
               const Date& hired, const Money& baseSalary, const std::string& certificate);

    std::string getRole() const override;
    /** @brief Вывод сотрудника + номер аттестата. */
    void print(std::ostream& os) const override;
    /** @brief Зарплата бухгалтера — просто оклад. */
    Money calculateSalary(int month) const override;

    /**
     * @brief Счёт по заказу: строка за диагностику, за работу каждой закрытой задачи
     * и за каждую установленную запчасть (по цене на момент установки).
     */
    Invoice makeInvoice(int invoiceId, const RepairOrder& o, const PriceList& p, const Date& today) const;
    /** @brief Зарплата сотрудника за месяц. Полиморфный вызов: формула зависит от настоящего типа e. */
    Salary makeSalary(const Employee& e, int month) const;
    /** @brief Вернуть клиенту деньги по платежу; ошибки приходят из Payment::refund. */
    void refund(Payment& p, CashRegister& cash) const;

    const std::string& getCertificate() const;

private:
    std::string certificate_;
};
