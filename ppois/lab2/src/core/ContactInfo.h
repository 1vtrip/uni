#pragma once
#include <string>
#include <iostream>
#include "core/Address.h"

/** @brief Контакты человека: телефон, e-mail и адрес (адрес хранится внутри, композиция). */
class ContactInfo {
public:
    ContactInfo(const std::string& phone, const std::string& email, const Address& address);

    /** @brief Телефон в формате +375 и ровно 9 цифр после него. */
    bool isPhoneValid() const;
    /** @brief Ровно одна @, имя и домен непустые, после @ есть точка, точки нет на краях домена. */
    bool isEmailValid() const;
    /** @brief Проверить телефон и e-mail; неверный -> InvalidClientDataException с причиной. */
    void validate() const;

    const std::string& getPhone() const;
    const std::string& getEmail() const;
    const Address& getAddress() const;

    bool operator==(const ContactInfo& other) const;
    bool operator!=(const ContactInfo& other) const;
    friend std::ostream& operator<<(std::ostream& os, const ContactInfo& c);

private:
    std::string phone_;
    std::string email_;
    Address address_;
};
