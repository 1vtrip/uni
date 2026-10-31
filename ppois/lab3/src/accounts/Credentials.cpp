#include "accounts/Credentials.h"
#include "exceptions/Exceptions.h"

Credentials::Credentials(const std::string& login, const std::string& salt, const Checksum& passwordHash)
    : login_(login), salt_(salt), passwordHash_(passwordHash) {}

/**
 * @brief Соль добавляется перед паролем, чтобы у двух пользователей с одинаковым паролем
 * были разные хеши: по одному хешу нельзя узнать, у кого ещё такой же пароль.
 */
Checksum Credentials::hash(const std::string& salt, const std::string& password) {
    return Checksum::of(salt + password);
}

Credentials Credentials::create(const std::string& login, const std::string& password) {
    if (login.empty()) throw InvalidOperationException("логин не может быть пустым");
    std::string salt = login + "#fms";
    return Credentials(login, salt, hash(salt, password));
}

bool Credentials::verify(const std::string& password) const {
    return hash(salt_, password).matches(passwordHash_);
}

void Credentials::change(const std::string& newPassword) { passwordHash_ = hash(salt_, newPassword); }

const std::string& Credentials::getLogin() const { return login_; }
const std::string& Credentials::getSalt() const { return salt_; }
const Checksum& Credentials::getPasswordHash() const { return passwordHash_; }

bool Credentials::operator==(const Credentials& o) const { return login_ == o.login_; }
bool Credentials::operator!=(const Credentials& o) const { return !(*this == o); }

std::ostream& operator<<(std::ostream& os, const Credentials& c) {
    return os << c.login_ << " (хеш " << c.passwordHash_.toHex() << ")";
}
