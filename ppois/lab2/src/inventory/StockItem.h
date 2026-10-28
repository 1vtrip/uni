#pragma once
#include <iostream>
#include "inventory/SparePart.h"

/** @brief Остаток одной запчасти на складе с учётом резерва под заказы. */
class StockItem {
public:
    StockItem(const SparePart& part, int qty, int minQty);

    /** @brief Сколько можно выдать сейчас: всего минус зарезервировано. */
    int available() const;
    /** @brief Зарезервировать qty штук под заказ; не хватает -> OutOfStockException. */
    void reserve(int qty);
    /** @brief Снять резерв (не больше, чем зарезервировано). */
    void release(int qty);
    /** @brief Списать зарезервированное: деталь установлена в устройство. */
    void writeOff(int qty);
    /** @brief Приход от поставщика. */
    void restock(int qty);
    /** @brief Пора дозаказать: доступно меньше минимального остатка. */
    bool needsRestock() const;

    const SparePart& getPart() const;
    int getQty() const;
    int getReserved() const;
    int getMinQty() const;

    /** @brief Позиции равны, если это одна и та же запчасть. */
    bool operator==(const StockItem& other) const;
    bool operator!=(const StockItem& other) const;
    friend std::ostream& operator<<(std::ostream& os, const StockItem& s);

private:
    SparePart part_;
    int qty_;
    int reserved_;
    int minQty_;
};
