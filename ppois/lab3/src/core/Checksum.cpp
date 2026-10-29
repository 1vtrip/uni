#include "core/Checksum.h"
#include <iomanip>
#include <sstream>

Checksum::Checksum(unsigned long long value) : value_(value) {}

/**
 * @brief Переполнение unsigned определено стандартом (по модулю 2^64), поэтому оно не ошибка.
 * Байт приводится к unsigned char: иначе байты русских букв были бы отрицательными.
 */
Checksum Checksum::of(const std::string& data) {
    unsigned long long h = 5381;
    for (size_t i = 0; i < data.size(); ++i)
        h = h * 33 + (unsigned char)data[i];
    return Checksum(h);
}

bool Checksum::matches(const Checksum& o) const { return value_ == o.value_; }

std::string Checksum::toHex() const {
    std::ostringstream out;
    out << std::hex << std::setw(16) << std::setfill('0') << value_;
    return out.str();
}

unsigned long long Checksum::getValue() const { return value_; }

bool Checksum::operator==(const Checksum& o) const { return value_ == o.value_; }
bool Checksum::operator!=(const Checksum& o) const { return !(*this == o); }

std::ostream& operator<<(std::ostream& os, const Checksum& c) { return os << c.toHex(); }
