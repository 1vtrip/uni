#pragma once
#include <string>
#include <iostream>
#include "core/Money.h"

/** @brief Карта лояльности: баллы за покупки и уровень ("Базовый" / "Серебряный" / "Золотой"). */
class LoyaltyCard {
public:
    /** @brief Новая пустая карта: 0 баллов, уровень "Базовый". */
    explicit LoyaltyCard(const std::string& number);

    /** @brief Начислить 1 балл за каждый полный рубль покупки и пересчитать уровень. */
    void addPoints(const Money& purchase);
    /** @brief Списать баллы и вернуть скидку (1 балл = 1 коп.); не хватает баллов -> InsufficientFundsException. */
    Money spendPoints(int points);
    /** @brief Уровень по всем когда-либо начисленным баллам: от 500 серебро, от 2000 золото. */
    void updateLevel();
    /** @brief Скидка по уровню: 0, 3 или 7 процентов. */
    double discountPercent() const;

    const std::string& getNumber() const;
    int getPoints() const;
    int getTotalEarned() const;
    const std::string& getLevel() const;

    /** @brief Карты равны, если совпадает номер. */
    bool operator==(const LoyaltyCard& other) const;
    bool operator!=(const LoyaltyCard& other) const;
    friend std::ostream& operator<<(std::ostream& os, const LoyaltyCard& c);

private:
    std::string number_;
    int points_;
    int totalEarned_;
    std::string level_;
};
