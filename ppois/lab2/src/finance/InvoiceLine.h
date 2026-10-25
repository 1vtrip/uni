#pragma once
#include <string>
#include <iostream>
#include "core/Money.h"

/** @brief Строка счёта: что продано или сделано, сколько и по какой цене. */
class InvoiceLine {
public:
    InvoiceLine(const std::string& name, int qty, const Money& price);

    /** @brief Сумма строки: цена * количество. */
    Money sum() const;

    const std::string& getName() const;
    int getQty() const;
    const Money& getPrice() const;

    bool operator==(const InvoiceLine& other) const;
    bool operator!=(const InvoiceLine& other) const;
    friend std::ostream& operator<<(std::ostream& os, const InvoiceLine& l);

private:
    std::string name_;
    int qty_;
    Money price_;
};
