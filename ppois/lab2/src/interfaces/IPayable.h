#pragma once
#include "core/Money.h"

/** @brief То, что нужно оплатить: счёт клиента или зарплата сотрудника. */
class IPayable {
public:
    virtual ~IPayable() = default;
    /** @brief Сколько ещё нужно заплатить (0, если уже оплачено). */
    virtual Money amountDue() const = 0;
    virtual bool isPaid() const = 0;
    virtual void markPaid() = 0;
};
