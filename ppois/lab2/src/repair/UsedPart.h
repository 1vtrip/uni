#pragma once
#include <string>
#include <iostream>
#include "core/Money.h"

/** @brief Запчасть, установленная при ремонте. Цена фиксируется на момент установки. */
class UsedPart {
public:
    UsedPart(const std::string& article, int qty, const Money& price);

    /** @brief Стоимость: цена на момент установки * количество. */
    Money sum() const;

    const std::string& getArticle() const;
    int getQty() const;
    const Money& getPrice() const;

    bool operator==(const UsedPart& other) const;
    bool operator!=(const UsedPart& other) const;
    friend std::ostream& operator<<(std::ostream& os, const UsedPart& p);

private:
    std::string article_;
    int qty_;
    Money price_;
};
