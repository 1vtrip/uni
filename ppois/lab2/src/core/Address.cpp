#include "core/Address.h"

Address::Address(const std::string& city, const std::string& street,
                 const std::string& house, const std::string& flat)
    : city_(city), street_(street), house_(house), flat_(flat) {}

std::string Address::toString() const {
    std::string result = "г. " + city_ + ", ул. " + street_ + ", д. " + house_;
    if (!flat_.empty()) result += ", кв. " + flat_;
    return result;
}

bool Address::sameCity(const Address& other) const {
    return city_ == other.city_;
}

const std::string& Address::getCity() const { return city_; }
const std::string& Address::getStreet() const { return street_; }
const std::string& Address::getHouse() const { return house_; }
const std::string& Address::getFlat() const { return flat_; }

bool Address::operator==(const Address& other) const {
    return city_ == other.city_ && street_ == other.street_
        && house_ == other.house_ && flat_ == other.flat_;
}

bool Address::operator!=(const Address& other) const {
    return !(*this == other);
}

std::ostream& operator<<(std::ostream& os, const Address& a) {
    return os << a.toString();
}
