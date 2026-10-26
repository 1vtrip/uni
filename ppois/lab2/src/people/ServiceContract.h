#pragma once
#include <string>
#include <iostream>
#include "core/Date.h"
#include "core/Money.h"

/** @brief Договор на обслуживание с компанией: срок действия, скидка и гарантированный срок ремонта (SLA). */
class ServiceContract {
public:
    ServiceContract(const std::string& number, const Date& start, const Date& end,
                    double discountPercent, int slaDays);

    /** @brief Действует ли договор в день today (начало и конец включительно). */
    bool isActive(const Date& today) const;
    /** @brief К какому дню нужно закончить ремонт, принятый в день from. */
    Date deadline(const Date& from) const;
    /** @brief Продлить договор на months месяцев; false, если months <= 0. */
    bool prolong(int months);
    /** @brief Скидка по договору в деньгах от суммы sum. */
    Money discountFor(const Money& sum) const;

    const std::string& getNumber() const;
    const Date& getStart() const;
    const Date& getEnd() const;
    double getDiscountPercent() const;
    int getSlaDays() const;

    /** @brief Договоры равны, если совпадает номер. */
    bool operator==(const ServiceContract& other) const;
    bool operator!=(const ServiceContract& other) const;
    friend std::ostream& operator<<(std::ostream& os, const ServiceContract& c);

private:
    std::string number_;
    Date start_;
    Date end_;
    double discountPercent_;
    int slaDays_;
};
