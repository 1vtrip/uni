#include "accounts/PasswordPolicy.h"
#include "exceptions/Exceptions.h"
#include <cctype>

PasswordPolicy::PasswordPolicy(int minLength, bool requireDigit, bool requireUpper, bool requireSpecial)
    : minLength_(minLength), requireDigit_(requireDigit), requireUpper_(requireUpper),
      requireSpecial_(requireSpecial) {
    if (minLength < 1) throw InvalidOperationException("минимальная длина пароля должна быть не меньше 1");
}

/** @brief Причины копятся в строку через "; ", исключение бросается один раз в конце. */
void PasswordPolicy::validate(const std::string& password, const std::string& login) const {
    std::string reasons;
    if (letterCount(password) < minLength_)
        reasons += "короче " + std::to_string(minLength_) + " символов; ";
    if (requireDigit_ && !hasDigit(password)) reasons += "нет цифры; ";
    if (requireUpper_ && !hasUpper(password)) reasons += "нет заглавной латинской буквы; ";
    if (requireSpecial_ && !hasSpecial(password)) reasons += "нет спецсимвола; ";
    if (containsLogin(password, login)) reasons += "содержит логин; ";
    if (!reasons.empty())
        throw WeakPasswordException(reasons.substr(0, reasons.size() - 2));   // без последнего "; "
}

int PasswordPolicy::strength(const std::string& password) const {
    int score = 0;
    if (letterCount(password) >= minLength_) ++score;
    if (hasDigit(password)) ++score;
    if (hasUpper(password)) ++score;
    if (hasSpecial(password)) ++score;
    return score;
}

/** @brief Байты продолжения UTF-8 имеют вид 10xxxxxx и не считаются, как в Tag::isValidName. */
int PasswordPolicy::letterCount(const std::string& s) {
    int letters = 0;
    for (size_t i = 0; i < s.size(); ++i)
        if (((unsigned char)s[i] & 0xC0) != 0x80) ++letters;
    return letters;
}

bool PasswordPolicy::hasDigit(const std::string& s) {
    for (size_t i = 0; i < s.size(); ++i)
        if (std::isdigit((unsigned char)s[i])) return true;
    return false;
}

bool PasswordPolicy::hasUpper(const std::string& s) {
    for (size_t i = 0; i < s.size(); ++i)
        if (std::isupper((unsigned char)s[i])) return true;
    return false;
}

/**
 * @brief Только ASCII (код меньше 128): для байтов русских букв isalnum вернёт false,
 * и без этой проверки "парольA1" считался бы паролем со спецсимволом.
 */
bool PasswordPolicy::hasSpecial(const std::string& s) {
    for (size_t i = 0; i < s.size(); ++i) {
        unsigned char c = (unsigned char)s[i];
        if (c < 128 && !std::isalnum(c) && !std::isspace(c)) return true;
    }
    return false;
}

/** @brief Обе строки переводятся в нижний регистр, потом ищется подстрока. */
bool PasswordPolicy::containsLogin(const std::string& password, const std::string& login) {
    if (login.empty()) return false;
    std::string p = password, l = login;
    for (size_t i = 0; i < p.size(); ++i) p[i] = (char)std::tolower((unsigned char)p[i]);
    for (size_t i = 0; i < l.size(); ++i) l[i] = (char)std::tolower((unsigned char)l[i]);
    return p.find(l) != std::string::npos;
}

int PasswordPolicy::getMinLength() const { return minLength_; }
bool PasswordPolicy::requiresDigit() const { return requireDigit_; }
bool PasswordPolicy::requiresUpper() const { return requireUpper_; }
bool PasswordPolicy::requiresSpecial() const { return requireSpecial_; }

std::ostream& operator<<(std::ostream& os, const PasswordPolicy& p) {
    os << "не короче " << p.minLength_;
    if (p.requireDigit_) os << ", цифра";
    if (p.requireUpper_) os << ", заглавная буква";
    if (p.requireSpecial_) os << ", спецсимвол";
    return os << ", без логина";
}
