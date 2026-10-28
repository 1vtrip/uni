#pragma once
#include <iostream>
#include "core/Date.h"
#include "core/Money.h"
#include "finance/PaymentMethod.h"
#include "finance/Invoice.h"
#include "finance/CashRegister.h"

/** @brief Платёж клиента по счёту: внесённая сумма, способ оплаты и возможный возврат. */
class Payment {
public:
    Payment(int invoiceId, const Money& amount, const PaymentMethod& method, const Date& date);

    /**
     * @brief Провести оплату: в кассу идёт сумма счёта минус комиссия, счёт помечается оплаченным.
     * Счёт не тот, уже оплачен или платёж проведён -> RepairFirmException; внесено мало -> InsufficientFundsException.
     */
    void process(Invoice& inv, CashRegister& cash);
    /** @brief Вернуть деньги из кассы. Платёж не проведён или уже возвращён -> RepairFirmException; в кассе мало -> InsufficientFundsException. */
    void refund(CashRegister& cash);
    /** @brief Сдача клиенту: внесено минус сумма счёта (0, если внесено не больше). */
    Money change(const Invoice& inv) const;

    int getInvoiceId() const;
    const Money& getAmount() const;
    const PaymentMethod& getMethod() const;
    const Date& getDate() const;
    bool isProcessed() const;
    bool isRefunded() const;
    const Money& getDeposited() const;

    bool operator==(const Payment& other) const;
    bool operator!=(const Payment& other) const;
    friend std::ostream& operator<<(std::ostream& os, const Payment& p);

private:
    int invoiceId_;
    Money amount_;
    PaymentMethod method_;
    Date date_;
    bool processed_;
    bool refunded_;
    Money deposited_;
};
