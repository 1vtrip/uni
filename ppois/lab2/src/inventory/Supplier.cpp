#include "inventory/Supplier.h"

Supplier::Supplier(int id, const std::string& name, const ContactInfo& contacts, int deliveryDays)
    : id_(id), name_(name), contacts_(contacts), rating_(0), ratingsCount_(0), deliveryDays_(deliveryDays) {}

Money Supplier::quote(const SparePart& part, int qty) const {
    Money sum = part.getBuyPrice() * qty;
    if (qty >= 50) return sum - sum.percent(10);
    if (qty >= 10) return sum - sum.percent(5);
    return sum;
}

bool Supplier::rate(int score) {
    if (score < 1 || score > 5) return false;
    rating_ = (rating_ * ratingsCount_ + score) / (ratingsCount_ + 1);
    ++ratingsCount_;
    return true;
}

Date Supplier::expectedDate(const Date& ordered) const {
    return ordered.addDays(deliveryDays_);
}

int Supplier::getId() const { return id_; }
const std::string& Supplier::getName() const { return name_; }
const ContactInfo& Supplier::getContacts() const { return contacts_; }
double Supplier::getRating() const { return rating_; }
int Supplier::getRatingsCount() const { return ratingsCount_; }
int Supplier::getDeliveryDays() const { return deliveryDays_; }

bool Supplier::operator==(const Supplier& other) const { return id_ == other.id_; }
bool Supplier::operator!=(const Supplier& other) const { return !(*this == other); }

std::ostream& operator<<(std::ostream& os, const Supplier& s) {
    return os << "Поставщик #" << s.id_ << " " << s.name_ << ", рейтинг " << s.rating_
              << " (" << s.ratingsCount_ << " оценок), доставка " << s.deliveryDays_ << " дн.";
}
