#pragma once
#include <iostream>
#include "core/Address.h"
#include "core/Money.h"

/** @brief Forward declaration: в заголовке Courier нужен только как const Courier&. */
class Courier;

/**
 * @brief Доставка техники по заказу курьером. Курьер хранится по id.
 * Список доставок у самого курьера (pickUp / deliver) обновляет RepairFirm.
 */
class Delivery {
public:
    Delivery(int id, int orderId, const Address& from, const Address& to);

    /** @brief Назначить курьера; false, если доставка уже выполнена. */
    bool assignCourier(const Courier& c);
    /** @brief Назначен ли курьер. */
    bool isAssigned() const;
    /** @brief Стоимость: в одном городе 10 руб., между городами 40 руб. */
    Money cost() const;
    /** @brief Отметить доставку выполненной; курьер не назначен или уже выполнено -> RepairFirmException. */
    void complete();

    int getId() const;
    int getOrderId() const;
    const Address& getFrom() const;
    const Address& getTo() const;
    int getCourierId() const;
    bool isDone() const;

    /** @brief Доставки равны, если совпадает id. */
    bool operator==(const Delivery& other) const;
    bool operator!=(const Delivery& other) const;
    friend std::ostream& operator<<(std::ostream& os, const Delivery& d);

private:
    int id_;
    int orderId_;
    Address from_;
    Address to_;
    int courierId_;
    bool done_;
};
