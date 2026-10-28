#include "finance/LoyaltyCard.h"
#include "exceptions/Exceptions.h"

LoyaltyCard::LoyaltyCard(const std::string& number)
    : number_(number), points_(0), totalEarned_(0), level_("Базовый") {}

void LoyaltyCard::addPoints(const Money& purchase) {
    if (purchase.getCoins() <= 0) return;
    int earned = (int)(purchase.getCoins() / 100);
    points_ += earned;
    totalEarned_ += earned;
    updateLevel();
}

Money LoyaltyCard::spendPoints(int points) {
    if (points <= 0 || points > points_)
        throw InsufficientFundsException("на карте " + number_ + " " + std::to_string(points_) + " баллов");
    points_ -= points;
    return Money(points);
}

/** @brief Уровень считается по totalEarned_, поэтому трата баллов его не понижает. */
void LoyaltyCard::updateLevel() {
    if (totalEarned_ >= 2000) level_ = "Золотой";
    else if (totalEarned_ >= 500) level_ = "Серебряный";
    else level_ = "Базовый";
}

double LoyaltyCard::discountPercent() const {
    if (level_ == "Золотой") return 7;
    if (level_ == "Серебряный") return 3;
    return 0;
}

const std::string& LoyaltyCard::getNumber() const { return number_; }
int LoyaltyCard::getPoints() const { return points_; }
int LoyaltyCard::getTotalEarned() const { return totalEarned_; }
const std::string& LoyaltyCard::getLevel() const { return level_; }

bool LoyaltyCard::operator==(const LoyaltyCard& other) const { return number_ == other.number_; }
bool LoyaltyCard::operator!=(const LoyaltyCard& other) const { return !(*this == other); }

std::ostream& operator<<(std::ostream& os, const LoyaltyCard& c) {
    return os << "Карта " << c.number_ << ": " << c.level_ << ", " << c.points_ << " баллов";
}
