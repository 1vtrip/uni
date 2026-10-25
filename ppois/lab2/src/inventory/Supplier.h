#pragma once
#include <string>
#include <iostream>
#include "core/ContactInfo.h"
#include "core/Date.h"
#include "core/Money.h"
#include "inventory/SparePart.h"

/** @brief Поставщик запчастей: контакты, средний рейтинг и срок доставки. */
class Supplier {
public:
    Supplier(int id, const std::string& name, const ContactInfo& contacts, int deliveryDays);

    /** @brief Цена партии: закупочная цена * qty, от 10 шт. скидка 5%, от 50 шт. 10%. */
    Money quote(const SparePart& part, int qty) const;
    /** @brief Добавить оценку 1–5 и пересчитать средний рейтинг; false, если оценка вне диапазона. */
    bool rate(int score);
    /** @brief Ожидаемая дата поставки для заказа, сделанного в день ordered. */
    Date expectedDate(const Date& ordered) const;

    int getId() const;
    const std::string& getName() const;
    const ContactInfo& getContacts() const;
    double getRating() const;
    int getRatingsCount() const;
    int getDeliveryDays() const;

    /** @brief Поставщики равны, если совпадает id. */
    bool operator==(const Supplier& other) const;
    bool operator!=(const Supplier& other) const;
    friend std::ostream& operator<<(std::ostream& os, const Supplier& s);

private:
    int id_;
    std::string name_;
    ContactInfo contacts_;
    double rating_;
    int ratingsCount_;
    int deliveryDays_;
};
