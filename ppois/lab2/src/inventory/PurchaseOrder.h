#pragma once
#include <vector>
#include <iostream>
#include "core/Date.h"
#include "core/Money.h"
#include "inventory/SparePart.h"
#include "inventory/Supplier.h"
#include "inventory/Warehouse.h"

/** @brief Заказ запчастей у поставщика. Строка i: запчасть parts_[i] в количестве quantities_[i]. */
class PurchaseOrder {
public:
    PurchaseOrder(int id, int supplierId, const Date& date);

    /** @brief Добавить строку; если такая запчасть уже есть, увеличить её количество. */
    void addLine(const SparePart& part, int qty);
    /** @brief Сумма заказа по закупочным ценам, без скидок. */
    Money total() const;
    /** @brief Сумма заказа с оптовыми скидками поставщика (Supplier::quote по каждой строке). */
    Money total(const Supplier& supplier) const;
    /** @brief Оприходовать все строки на склад; новые позиции получают минимальный остаток defaultMinQty. Повторный вызов ничего не делает. */
    void receive(Warehouse& w, int defaultMinQty);
    /** @brief Общее число штук во всех строках. */
    int totalQty() const;

    int getId() const;
    int getSupplierId() const;
    const Date& getDate() const;
    bool isReceived() const;
    const std::vector<SparePart>& getParts() const;
    const std::vector<int>& getQuantities() const;

    /** @brief Заказы равны, если совпадает id. */
    bool operator==(const PurchaseOrder& other) const;
    bool operator!=(const PurchaseOrder& other) const;
    friend std::ostream& operator<<(std::ostream& os, const PurchaseOrder& o);

private:
    int id_;
    int supplierId_;
    std::vector<SparePart> parts_;
    std::vector<int> quantities_;
    Date date_;
    bool received_;
};
