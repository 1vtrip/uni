#pragma once
#include <iostream>
#include "core/Money.h"
#include "interfaces/IPayable.h"

/** @brief Зарплата сотрудника за месяц: оклад, премии, удержания и подоходный налог 13%. */
class Salary : public IPayable {
public:
    Salary(int employeeId, int month, const Money& base);

    /** @brief Добавить премию; false, если сумма отрицательная. */
    bool addBonus(const Money& m);
    /** @brief Добавить удержание (штраф, аванс); false, если сумма отрицательная. */
    bool addDeduction(const Money& m);
    /** @brief Начислено: оклад + премии - удержания, не меньше 0. */
    Money gross() const;
    /** @brief Подоходный налог: 13% от начисленного. */
    Money tax() const;
    /** @brief К выплате: начислено минус налог. */
    Money net() const;
    /** @brief К выплате: net(), если зарплата не выдана, иначе 0 (из IPayable). */
    Money amountDue() const override;
    /** @brief Выдана ли зарплата (из IPayable). */
    bool isPaid() const override;
    /** @brief Отметить зарплату выданной (из IPayable). */
    void markPaid() override;

    int getEmployeeId() const;
    int getMonth() const;
    const Money& getBase() const;
    const Money& getBonus() const;
    const Money& getDeductions() const;

    /** @brief Зарплаты равны, если это один сотрудник и один месяц. */
    bool operator==(const Salary& other) const;
    bool operator!=(const Salary& other) const;
    friend std::ostream& operator<<(std::ostream& os, const Salary& s);

private:
    int employeeId_;
    int month_;
    Money base_;
    Money bonus_;
    Money deductions_;
    bool paid_;
};