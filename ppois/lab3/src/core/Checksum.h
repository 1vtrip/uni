#pragma once
#include <iostream>
#include <string>

/** @brief Контрольная сумма содержимого (хеш djb2). Нужна для проверки целостности файлов. */
class Checksum {
public:
    explicit Checksum(unsigned long long value = 0);

    /** @brief Посчитать сумму строки: h = 5381, для каждого байта h = h * 33 + c. */
    static Checksum of(const std::string& data);

    /** @brief Совпадает ли с другой суммой (содержимое не менялось). */
    bool matches(const Checksum& o) const;
    /** @brief Шестнадцатеричная запись из 16 цифр с ведущими нулями. */
    std::string toHex() const;

    unsigned long long getValue() const;

    bool operator==(const Checksum& o) const;
    bool operator!=(const Checksum& o) const;
    friend std::ostream& operator<<(std::ostream& os, const Checksum& c);

private:
    unsigned long long value_;
};
