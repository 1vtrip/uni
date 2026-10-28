#pragma once
#include <string>
#include <vector>
#include <iostream>
#include "core/Money.h"
#include "finance/ServiceItem.h"

/** @brief Прайс-лист: ставка за час работы, список услуг и наценка за срочный ремонт. */
class PriceList {
public:
    PriceList(const Money& hourRate, double urgentMarkup);

    /** @brief Добавить услугу; false, если услуга с таким кодом уже есть. */
    bool addItem(const ServiceItem& item);
    /** @brief Услуга по коду; нет такой -> RepairFirmException. */
    const ServiceItem& find(const std::string& code) const;
    /** @brief Стоимость работы: ставка * часы, при срочности + наценка. */
    Money laborCost(double hours, bool urgent) const;
    /** @brief Поднять ставку и цены всех услуг на percent процентов. */
    void raise(double percent);

    const Money& getHourRate() const;
    double getUrgentMarkup() const;
    const std::vector<ServiceItem>& getItems() const;

    friend std::ostream& operator<<(std::ostream& os, const PriceList& p);

private:
    /** @brief Индекс услуги по коду или -1 (внутренний помощник для addItem и find). */
    int indexOf(const std::string& code) const;

    Money hourRate_;
    std::vector<ServiceItem> items_;
    double urgentMarkup_;
};
