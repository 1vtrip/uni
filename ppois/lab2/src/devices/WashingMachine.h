#pragma once
#include "devices/HomeAppliance.h"

/** @brief Стиральная машина: загрузка и состояние барабана. Конец иерархии (final). */
class WashingMachine final : public HomeAppliance {
public:
    WashingMachine(const std::string& serial, const std::string& model, const Brand& brand,
                   const DeviceCategory& category, const Date& purchased,
                   int power, double weight, bool builtIn,
                   double loadKg, const std::string& drum);

    std::string typeName() const override final;
    /** @brief Базовая проверка + неисправный отжим, если барабан повреждён. */
    std::vector<Defect> quickTest(const Date& today) const override;
    /** @brief Вывод бытовой техники + загрузка и состояние барабана. */
    void print(std::ostream& os) const override;

    /** @brief Проходит ли проверка отжима: нет, если барабан повреждён. */
    bool spinTest() const;

    double getLoadKg() const;
    const std::string& getDrum() const;

private:
    double loadKg_;
    std::string drum_;
};
