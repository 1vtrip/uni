#pragma once
#include <vector>
#include <iostream>
#include "core/Date.h"
#include "core/Money.h"
#include "finance/InvoiceLine.h"
#include "finance/Discount.h"
#include "interfaces/IPrintable.h"
#include "interfaces/IPayable.h"

/** @brief Счёт клиенту за заказ: строки, необязательная скидка и признак оплаты. */
class Invoice : public IPrintable, public IPayable {
public:
    Invoice(int id, int orderId, int clientId, const Date& date);

    /** @brief Добавить строку; false, если счёт уже оплачен. */
    bool addLine(const InvoiceLine& line);
    /** @brief Применить скидку (заменяет прежнюю); false, если счёт оплачен или скидка неприменима к сумме. */
    bool applyDiscount(const Discount& d, const Date& today);   // TODO(exc): InvalidDiscountException
    /** @brief Сумма всех строк без скидки. */
    Money subtotal() const;
    /** @brief Итог к оплате с учётом скидки. */
    Money total() const;
    /** @brief Просрочен: не оплачен, и с даты счёта прошло больше termDays дней. */
    bool isOverdue(const Date& today, int termDays) const;
    /** @brief Отметить счёт оплаченным (из IPayable). */
    void markPaid() override;
    /** @brief К оплате: итог, если счёт не оплачен, иначе 0 (из IPayable). */
    Money amountDue() const override;

    int getId() const;
    int getOrderId() const;
    int getClientId() const;
    const std::vector<InvoiceLine>& getLines() const;
    const Date& getDate() const;
    bool hasDiscount() const;
    const Discount& getDiscount() const;
    bool isPaid() const override;

    /** @brief Счета равны, если совпадает id. */
    bool operator==(const Invoice& other) const;
    bool operator!=(const Invoice& other) const;
    /** @brief Строки, скидка, итог и отметка об оплате (из IPrintable). */
    void print(std::ostream& os) const override;

private:
    int id_;
    int orderId_;
    int clientId_;
    std::vector<InvoiceLine> lines_;
    Date date_;
    Discount discount_;
    bool hasDiscount_;
    bool paid_;
};