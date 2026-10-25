#pragma once
#include <string>
#include <iostream>

/** @brief Производитель техники. Гарантийный ремонт возможен только для авторизованных брендов. */
class Brand {
public:
    Brand(const std::string& name, const std::string& country, bool authorized);

    /** @brief Можно ли делать гарантийный ремонт: только если фирма авторизована брендом. */
    bool allowsWarrantyRepair() const;

    const std::string& getName() const;
    const std::string& getCountry() const;

    bool operator==(const Brand& other) const;
    bool operator!=(const Brand& other) const;
    friend std::ostream& operator<<(std::ostream& os, const Brand& b);

private:
    std::string name_;
    std::string country_;
    bool authorized_;
};
