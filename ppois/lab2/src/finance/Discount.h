#pragma once
#include <string>
#include <iostream>
#include "core/Date.h"
#include "core/Money.h"

/** @brief Скидка по промокоду: процент, срок действия, минимальная сумма и одобрение менеджера. */
class Discount {
public:
    /** @brief Пустая скидка (0%, не одобрена): нужна счёту, пока скидку не применили. */
    Discount();
    Discount(const std::string& code, double percent, const Date& until, const Money& minSum);

    /** @brief Можно ли применить: одобрена, срок не истёк (until включительно), сумма не меньше минимальной. */
    bool isApplicable(const Money& sum, const Date& today) const;
    /** @brief Размер скидки в деньгах от суммы sum. */
    Money amount(const Money& sum) const;
    /** @brief Одобрить скидку (делает менеджер). */
    void approve();

    const std::string& getCode() const;
    double getPercent() const;
    const Date& getUntil() const;
    const Money& getMinSum() const;
    bool isApproved() const;

    /** @brief Скидки равны, если совпадает код. */
    bool operator==(const Discount& other) const;
    bool operator!=(const Discount& other) const;
    friend std::ostream& operator<<(std::ostream& os, const Discount& d);

private:
    std::string code_;
    double percent_;
    Date until_;
    Money minSum_;
    bool approved_;
};
