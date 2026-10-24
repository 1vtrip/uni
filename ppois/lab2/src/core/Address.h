#pragma once
#include <string>
#include <iostream>

/** @brief Почтовый адрес: город, улица, дом и квартира (квартиры может не быть). */
class Address {
public:
    Address(const std::string& city, const std::string& street,
            const std::string& house, const std::string& flat = "");

    /** @brief Адрес одной строкой; квартира пишется, только если указана. */
    std::string toString() const;
    /** @brief Находятся ли оба адреса в одном городе. */
    bool sameCity(const Address& other) const;

    const std::string& getCity() const;
    const std::string& getStreet() const;
    const std::string& getHouse() const;
    const std::string& getFlat() const;

    bool operator==(const Address& other) const;
    bool operator!=(const Address& other) const;
    friend std::ostream& operator<<(std::ostream& os, const Address& a);

private:
    std::string city_;
    std::string street_;
    std::string house_;
    std::string flat_;
};
