#pragma once
#include <string>
#include <iostream>
#include "devices/DeviceCategory.h"

/** @brief Специализация мастера: с какой категорией техники он работает и с какого уровня. */
class Specialization {
public:
    Specialization(const std::string& name, const DeviceCategory& category, int minLevel);

    /** @brief Подходит ли: та же категория (по названию) и уровень мастера не ниже минимального. */
    bool matches(const DeviceCategory& c, int techLevel) const;

    const std::string& getName() const;
    const DeviceCategory& getCategory() const;
    int getMinLevel() const;

    bool operator==(const Specialization& other) const;
    bool operator!=(const Specialization& other) const;
    friend std::ostream& operator<<(std::ostream& os, const Specialization& s);

private:
    std::string name_;
    DeviceCategory category_;
    int minLevel_;
};
