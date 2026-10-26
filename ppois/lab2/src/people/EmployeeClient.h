#pragma once
#include <string>
#include "people/Employee.h"
#include "people/Client.h"

/**
 * @brief Сотрудник, сдавший в ремонт свою технику. Ромб: Employee и Client наследуют
 * Person виртуально, поэтому Person (id, имя, контакты) в объекте один.
 * Конец иерархии (final).
 */
class EmployeeClient final : public Employee, public Client {
public:
    /** @brief Виртуальную базу Person создаёт этот, самый производный класс. */
    EmployeeClient(int id, const std::string& name, const ContactInfo& contacts, const Date& birth,
                   const std::string& position, const Date& hired, const Money& baseSalary,
                   const Date& registered);

    std::string getRole() const override;
    /** @brief Обязателен: у Employee и Client свои print. Имя выводится один раз, потом обе части. */
    void print(std::ostream& os) const override;
    /** @brief Обязателен: в Employee он чисто виртуальный. Зарплата — оклад. */
    Money calculateSalary(int month) const override;
    /** @brief Скидка сотрудника 30% (всегда больше скидки по карте, максимум 7%). */
    Money applyDiscount(const Money& sum) const override;
    /** @brief Сотрудник может держать в ремонте до 5 своих устройств. */
    int maxActiveOrders() const override;
};
