#pragma once
#include <string>
#include <vector>
#include <iostream>
#include "core/Money.h"

/** @brief Касса фирмы: остаток и журнал всех операций. */
class CashRegister {
public:
    explicit CashRegister(const Money& initial = Money());

    /** @brief Принять деньги в кассу и записать операцию в журнал. */
    void deposit(const Money& sum, const std::string& note);
    /** @brief Выдать деньги из кассы; в кассе меньше sum -> InsufficientFundsException. */
    void withdraw(const Money& sum, const std::string& note);
    /** @brief Отчёт: все записи журнала и текущий остаток одной строкой. */
    std::string report() const;

    const Money& getBalance() const;
    const std::vector<std::string>& getLog() const;

    friend std::ostream& operator<<(std::ostream& os, const CashRegister& c);

private:
    Money balance_;
    std::vector<std::string> log_;
};
