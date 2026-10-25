#pragma once
#include <string>
#include <iostream>
#include "core/Money.h"

/** @brief Способ оплаты ("Наличные" / "Карта" / "Перевод") и комиссия банка в процентах. */
class PaymentMethod {
public:
    PaymentMethod(const std::string& type, double commission);

    /** @brief Комиссия с суммы sum. */
    Money commissionFor(const Money& sum) const;
    /** @brief Сколько дойдёт до кассы: сумма минус комиссия. */
    Money netAmount(const Money& sum) const;

    const std::string& getType() const;
    double getCommission() const;

    bool operator==(const PaymentMethod& other) const;
    bool operator!=(const PaymentMethod& other) const;
    friend std::ostream& operator<<(std::ostream& os, const PaymentMethod& m);

private:
    std::string type_;
    double commission_;
};
