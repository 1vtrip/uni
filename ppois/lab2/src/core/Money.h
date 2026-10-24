#pragma once
#include <iostream>

/** @brief Денежная сумма. Хранится в копейках (coins), чтобы не было ошибок округления. */
class Money {
public:
    explicit Money(long long coins = 0);
    /** @brief Создать сумму из рублей (с округлением до копейки). */
    static Money fromRubles(double rubles);

    long long getCoins() const;
    double toRubles() const;
    bool isZero() const;
    /** @brief p процентов от суммы (округление до копейки). */
    Money percent(double p) const;

    Money operator+(const Money& other) const;
    Money operator-(const Money& other) const;
    Money operator*(double k) const;
    Money& operator+=(const Money& other);
    Money& operator-=(const Money& other);
    bool operator<(const Money& other) const;
    bool operator==(const Money& other) const;
    bool operator!=(const Money& other) const;
    friend std::ostream& operator<<(std::ostream& os, const Money& m);

private:
    long long coins_;
};
