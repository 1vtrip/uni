#pragma once
#include <string>
#include <iostream>
#include "core/Money.h"
#include "devices/DeviceCategory.h"

/** @brief Услуга из прайс-листа: код, название, цена и категория техники, к которой относится. */
class ServiceItem {
public:
    ServiceItem(const std::string& code, const std::string& name, const Money& price,
                const DeviceCategory& category);

    /** @brief Цена услуги; при срочном ремонте + urgentMarkup процентов. */
    Money priceFor(bool urgent, double urgentMarkup) const;

    const std::string& getCode() const;
    const std::string& getName() const;
    const Money& getPrice() const;
    const DeviceCategory& getCategory() const;
    void setPrice(const Money& price);

    /** @brief Услуги равны, если совпадает код. */
    bool operator==(const ServiceItem& other) const;
    bool operator!=(const ServiceItem& other) const;
    friend std::ostream& operator<<(std::ostream& os, const ServiceItem& s);

private:
    std::string code_;
    std::string name_;
    Money price_;
    DeviceCategory category_;
};
