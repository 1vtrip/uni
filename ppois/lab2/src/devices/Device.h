#pragma once
#include <string>
#include <vector>
#include <iostream>
#include "core/Date.h"
#include "core/Money.h"
#include "devices/Brand.h"
#include "devices/DeviceCategory.h"
#include "devices/Defect.h"

/** @brief Устройство, принятое в ремонт. Не абстрактное: хранится по значению в RepairOrder. */
class Device {
public:
    Device(const std::string& serial, const std::string& model, const Brand& brand,
           const DeviceCategory& category, const Date& purchased);
    virtual ~Device() = default;

    /** @brief Название типа устройства для вывода. */
    virtual std::string typeName() const;
    /** @brief Примерный срок ремонта в днях. */
    virtual int estimateRepairDays() const;
    /** @brief Стоимость диагностики. */
    virtual Money diagnosticPrice() const;
    /** @brief Нужен ли выезд мастера на дом. */
    virtual bool needsHomeVisit() const;
    /** @brief Быстрая проверка при приёмке, у каждого типа свои тесты. */
    virtual std::vector<Defect> quickTest(const Date& today) const;
    /** @brief Вывести устройство в поток (вызывается из operator<<). */
    virtual void print(std::ostream& os) const;

    /** @brief Возраст устройства в полных месяцах. */
    int ageMonths(const Date& today) const;
    /** @brief На заводской гарантии: младше 12 месяцев и бренд разрешает гарантийный ремонт. */
    bool underFactoryWarranty(const Date& today) const;

    const std::string& getSerial() const;
    const std::string& getModel() const;
    const Brand& getBrand() const;
    const DeviceCategory& getCategory() const;
    const Date& getPurchased() const;

    bool operator==(const Device& other) const;   // по серийному номеру
    bool operator!=(const Device& other) const;

protected:
    std::string serial_;
    std::string model_;
    Brand brand_;
    DeviceCategory category_;
    Date purchased_;
};

/** @brief Вывод любого устройства через виртуальный print(). */
std::ostream& operator<<(std::ostream& os, const Device& d);
