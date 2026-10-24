#pragma once
#include <string>
#include <iostream>
#include "core/Money.h"

/** @brief Категория техники (смартфоны, ноутбуки, ...): цена диагностики и средний срок ремонта. */
class DeviceCategory {
public:
    DeviceCategory(const std::string& name, const Money& diagnosticPrice, int avgDays);

    /** @brief Срок ремонта в днях: средний срок + 2 дня за каждую ступень сложности выше первой. */
    int estimateDays(int complexity) const;

    const std::string& getName() const;
    const Money& getDiagnosticPrice() const;
    int getAvgDays() const;

    bool operator==(const DeviceCategory& other) const;
    bool operator!=(const DeviceCategory& other) const;
    friend std::ostream& operator<<(std::ostream& os, const DeviceCategory& c);

private:
    std::string name_;
    Money diagnosticPrice_;
    int avgDays_;
};
