#pragma once
#include <string>
#include <vector>
#include "people/Person.h"
#include "core/Money.h"
#include "finance/LoyaltyCard.h"

/** @brief Forward declaration: в заголовке нужны только как const std::vector<...>&. */
class RepairOrder;
class Invoice;

/** @brief Клиент фирмы: карта лояльности и номера своих заказов. Поля protected — есть наследник. */
class Client : public Person {
public:
    /** @brief Новый клиент получает пустую карту с номером "LC-<id>". */
    Client(int id, const std::string& name, const ContactInfo& contacts, const Date& birth,
           const Date& registered);

    std::string getRole() const override;
    /** @brief Базовый вывод + уровень карты и число заказов. */
    void print(std::ostream& os) const override;

    /** @brief Сумма после скидки клиента: у обычного клиента — по уровню карты. */
    virtual Money applyDiscount(const Money& sum) const;
    /** @brief Сколько незавершённых заказов клиент может иметь одновременно. */
    virtual int maxActiveOrders() const;

    /** @brief Запомнить номер нового заказа (без повторов). */
    void addOrder(int orderId);
    /** @brief Сколько незавершённых заказов этого клиента среди всех заказов фирмы. */
    int activeOrders(const std::vector<RepairOrder>& all) const;
    /** @brief Можно ли принять ещё заказ. Невиртуальный, но внутри виртуальный maxActiveOrders(). */
    bool canPlaceOrder(const std::vector<RepairOrder>& all) const;
    /** @brief Сколько клиент заплатил по своим оплаченным счетам. */
    Money totalSpent(const std::vector<Invoice>& all) const;

    const Date& getRegistered() const;
    LoyaltyCard& getCard();
    const LoyaltyCard& getCard() const;
    const std::vector<int>& getOrderIds() const;

protected:
    Date registered_;
    LoyaltyCard card_;
    std::vector<int> orderIds_;
};
