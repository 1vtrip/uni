#include "devices/Brand.h"

Brand::Brand(const std::string& name, const std::string& country, bool authorized)
    : name_(name), country_(country), authorized_(authorized) {}

bool Brand::allowsWarrantyRepair() const {
    return authorized_;
}

const std::string& Brand::getName() const { return name_; }
const std::string& Brand::getCountry() const { return country_; }

bool Brand::operator==(const Brand& other) const {
    return name_ == other.name_ && country_ == other.country_ && authorized_ == other.authorized_;
}

bool Brand::operator!=(const Brand& other) const {
    return !(*this == other);
}

std::ostream& operator<<(std::ostream& os, const Brand& b) {
    return os << b.name_ << " (" << b.country_ << ")"
              << (b.authorized_ ? ", авторизованный сервис" : "");
}
