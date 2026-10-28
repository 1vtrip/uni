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
    /** @brief Позиция по артикулу; нет такой -> OutOfStockException. Ссылку не хранить: после add она может устареть. */
    StockItem& find(const std::string& article);
    const StockItem& find(const std::string& article) const;
    /** @brief Выдать qty штук: зарезервировать и сразу списать, вернуть копию запчасти. Нет или мало -> OutOfStockException. */
    SparePart take(const std::string& article, int qty);
    /** @brief Позиции, которые пора дозаказать. */
    std::vector<StockItem> lowStock() const;
    /** @brief Стоимость склада по закупочным ценам. */
    Money totalValue() const;

    const Address& getAddress() const;
    const std::vector<StockItem>& getItems() const;

    friend std::ostream& operator<<(std::ostream& os, const Warehouse& w);

private:
    /** @brief Индекс позиции по артикулу или -1 (внутренний помощник для add, contains и find). */
    int indexOf(const std::string& article) const;

    Address address_;
    std::vector<StockItem> items_;
};
