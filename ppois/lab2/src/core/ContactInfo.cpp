#include "core/ContactInfo.h"

ContactInfo::ContactInfo(const std::string& phone, const std::string& email, const Address& address)
    : phone_(phone), email_(email), address_(address) {}

/** @brief Длина 13, начало "+375", остальные 9 символов — цифры от '0' до '9'. */
bool ContactInfo::isPhoneValid() const {
    if (phone_.size() != 13) return false;
    if (phone_.compare(0, 4, "+375") != 0) return false;
    for (size_t i = 4; i < phone_.size(); ++i)
        if (phone_[i] < '0' || phone_[i] > '9') return false;
    return true;
}

/**
 * @brief За один проход считается число @ и запоминается её позиция.
 * Затем проверяется, что @ одна и не на краю, после неё есть точка,
 * но не сразу после @, и строка не начинается и не заканчивается точкой.
 */
bool ContactInfo::isEmailValid() const {
    int atCount = 0;
    size_t atPos = 0;
    for (size_t i = 0; i < email_.size(); ++i) {
        if (email_[i] == '@') {
            ++atCount;
            atPos = i;
        }
    }
    if (atCount != 1) return false;
    if (atPos == 0 || atPos == email_.size() - 1) return false;

    size_t dotPos = email_.find('.', atPos);
    if (dotPos == std::string::npos) return false;
    if (dotPos == atPos + 1) return false;
    if (email_[0] == '.' || email_[email_.size() - 1] == '.') return false;
    return true;
}

/** @brief Телефон и e-mail проверяются по отдельности, чтобы было видно, что именно неверно. */
bool ContactInfo::validate() const {
    if (!isPhoneValid()) return false;   // TODO(exc): InvalidClientDataException("Неверный телефон: " + phone_)
    if (!isEmailValid()) return false;   // TODO(exc): InvalidClientDataException("Неверный e-mail: " + email_)
    return true;
}

const std::string& ContactInfo::getPhone() const { return phone_; }
const std::string& ContactInfo::getEmail() const { return email_; }
const Address& ContactInfo::getAddress() const { return address_; }

bool ContactInfo::operator==(const ContactInfo& other) const {
    return phone_ == other.phone_ && email_ == other.email_ && address_ == other.address_;
}

bool ContactInfo::operator!=(const ContactInfo& other) const {
    return !(*this == other);
}

std::ostream& operator<<(std::ostream& os, const ContactInfo& c) {
    return os << c.phone_ << ", " << c.email_ << ", " << c.address_;
}
