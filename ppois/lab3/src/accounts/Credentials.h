#pragma once
#include <iostream>
#include <string>
#include "core/Checksum.h"

/**
 * @brief Данные для входа: логин, соль и хеш пароля. Сам пароль нигде не хранится.
 * Создать можно только через create(): так пароль сразу превращается в хеш.
 */
class Credentials {
public:
    /**
     * @brief Соль = login + "#fms", хеш = Checksum::of(соль + пароль).
     * @throws InvalidOperationException логин пустой.
     */
    static Credentials create(const std::string& login, const std::string& password);

    /** @brief Верен ли пароль: хеш соли и password совпадает с сохранённым. */
    bool verify(const std::string& password) const;
    /** @brief Заменить пароль: новый хеш с той же солью. Требования к паролю проверяет PasswordPolicy. */
    void change(const std::string& newPassword);

    const std::string& getLogin() const;
    const std::string& getSalt() const;
    const Checksum& getPasswordHash() const;

    /** @brief Данные равны, если совпадают логины. */
    bool operator==(const Credentials& o) const;
    bool operator!=(const Credentials& o) const;
    /** @brief "ivan (хеш 1a2b3c...)": сам пароль вывести нельзя, его нет. */
    friend std::ostream& operator<<(std::ostream& os, const Credentials& c);

private:
    Credentials(const std::string& login, const std::string& salt, const Checksum& passwordHash);
    /** @brief Хеш пароля с солью. */
    static Checksum hash(const std::string& salt, const std::string& password);

    std::string login_;
    std::string salt_;
    Checksum passwordHash_;
};
