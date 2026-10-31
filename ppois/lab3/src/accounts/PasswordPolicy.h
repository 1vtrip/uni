#pragma once
#include <iostream>
#include <string>

/**
 * @brief Требования к паролю: минимальная длина, цифра, заглавная буква, спецсимвол
 * и запрет на логин внутри пароля.
 */
class PasswordPolicy {
public:
    /** @brief @throws InvalidOperationException minLength меньше 1. */
    PasswordPolicy(int minLength = 8, bool requireDigit = true, bool requireUpper = true,
                   bool requireSpecial = false);

    /**
     * @brief Проверить пароль. Собираются все нарушения сразу, чтобы пользователь видел, что исправить.
     * @throws WeakPasswordException "короче 8 символов; нет цифры; содержит логин".
     */
    void validate(const std::string& password, const std::string& login) const;
    /** @brief Надёжность 0–4: сколько выполнено из «длина, цифра, заглавная, спецсимвол» (без учёта настроек). */
    int strength(const std::string& password) const;

    /** @brief Число букв, а не байтов: русская буква в UTF-8 занимает 2 байта, но считается за одну. */
    static int letterCount(const std::string& s);
    static bool hasDigit(const std::string& s);
    /** @brief Есть ли заглавная латинская буква (isupper не знает кириллицу). */
    static bool hasUpper(const std::string& s);
    /** @brief Есть ли спецсимвол: ASCII-символ, который не буква, не цифра и не пробел. */
    static bool hasSpecial(const std::string& s);
    /** @brief Содержит ли password логин без учёта регистра латиницы; пустой логин — false. */
    static bool containsLogin(const std::string& password, const std::string& login);

    int getMinLength() const;
    bool requiresDigit() const;
    bool requiresUpper() const;
    bool requiresSpecial() const;

    /** @brief "не короче 8, цифра, заглавная буква, без логина". */
    friend std::ostream& operator<<(std::ostream& os, const PasswordPolicy& p);

private:
    int minLength_;
    bool requireDigit_;
    bool requireUpper_;
    bool requireSpecial_;
};
