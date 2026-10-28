#pragma once
#include <string>
#include "people/Person.h"
#include "people/Schedule.h"
#include "core/Money.h"

/** @brief Forward declaration: в заголовке Device нужен только как const Device&. */
class Device;

/**
 * @brief Абстрактный сотрудник фирмы: должность, дата найма, оклад и график смен.
 * Наследование virtual: в EmployeeClient (ромб) часть Person будет одна.
 */
class Employee : public virtual Person {
public:
    Employee(int id, const std::string& name, const ContactInfo& contacts, const Date& birth,
             const std::string& position, const Date& hired, const Money& baseSalary);

    /**
     * @brief Зарплата за месяц. Чисто виртуальная, но с реализацией: возвращает оклад.
     * Каждый наследник обязан переопределить её и может вызвать Employee::calculateSalary.
     */
    virtual Money calculateSalary(int month) const = 0;
    /** @brief Может ли сотрудник ремонтировать это устройство; по умолчанию нет (только мастер). */
    virtual bool canWorkOn(const Device& d) const;
    /** @brief Базовый вывод + должность и оклад. */
    void print(std::ostream& os) const override;

    /** @brief Стаж в полных годах на дату today. */
    int experienceYears(const Date& today) const;
    /** @brief Добавить смену в график; ошибки (ScheduleConflictException) приходят из Schedule::add. */
    void addShift(const Shift& s);
    /** @brief Сколько всего часов в графике. */
    int hoursWorked() const;

    const std::string& getPosition() const;
    const Date& getHired() const;
    const Money& getBaseSalary() const;
    const Schedule& getSchedule() const;

protected:
    /** @brief Вывод только своих полей (без Person::print): нужен EmployeeClient, чтобы имя не повторялось. */
    void printEmployeePart(std::ostream& os) const;

    std::string position_;
    Date hired_;
    Money baseSalary_;
    Schedule schedule_;
};
