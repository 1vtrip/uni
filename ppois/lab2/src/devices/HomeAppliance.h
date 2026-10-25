#pragma once
#include "devices/Device.h"
#include "core/Address.h"

/** @brief Бытовая техника: мощность, вес и признак встраиваемой. Поля protected — есть наследник. */
class HomeAppliance : public Device {
public:
    HomeAppliance(const std::string& serial, const std::string& model, const Brand& brand,
                  const DeviceCategory& category, const Date& purchased,
                  int power, double weight, bool builtIn);

    std::string typeName() const override;
    /** @brief Выезд на дом нужен для встраиваемой техники или тяжелее 30 кг. */
    bool needsHomeVisit() const override;
    /** @brief Базовый срок + 3 дня. */
    int estimateRepairDays() const override;
    /** @brief Базовая цена + 20 руб. за выезд, если он нужен. */
    Money diagnosticPrice() const override;
    /** @brief Базовый вывод + мощность, вес и отметка о встраиваемой. */
    void print(std::ostream& os) const override;

    /** @brief Перевозка: в одном городе 15 руб. + 0,5 руб./кг, в разных 50 руб. + 1 руб./кг. */
    Money transportCost(const Address& from, const Address& to) const;

    int getPower() const;
    double getWeight() const;
    bool isBuiltIn() const;

protected:
    int power_;
    double weight_;
    bool builtIn_;
};
