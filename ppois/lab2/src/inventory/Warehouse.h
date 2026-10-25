#pragma once
#include <string>
#include <vector>
#include <iostream>
#include "core/Address.h"
#include "core/Money.h"
#include "inventory/StockItem.h"

/** @brief Склад запчастей: адрес и список позиций с остатками. */
class Warehouse {
public:
    explicit Warehouse(const Address& address);

    /** @brief Принять запчасть: если позиция уже есть, пополнить её, иначе создать новую. */
    void add(const SparePart& part, int qty, int minQty);
    /** @brief Есть ли на складе позиция с таким артикулом. */
    bool contains(const std::string& article) const;
    /** @brief Индекс позиции по артикулу или -1, если её нет. */
    int indexOf(const std::string& article) const;   // TODO(exc): вместе с itemAt -> find + OutOfStockException
    /** @brief Позиция по индексу из indexOf. Ссылку не хранить: после add она может устареть. */
    StockItem& itemAt(int index);
    const StockItem& itemAt(int index) const;
    /** @brief Выдать qty штук: зарезервировать и сразу списать; false, если позиции нет или не хватает. */
    bool take(const std::string& article, int qty);   // TODO(exc): OutOfStockException
    /** @brief Позиции, которые пора дозаказать. */
    std::vector<StockItem> lowStock() const;
    /** @brief Стоимость склада по закупочным ценам. */
    Money totalValue() const;

    const Address& getAddress() const;
    const std::vector<StockItem>& getItems() const;

    friend std::ostream& operator<<(std::ostream& os, const Warehouse& w);

private:
    Address address_;
    std::vector<StockItem> items_;
};
