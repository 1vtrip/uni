#include "people/Specialization.h"

Specialization::Specialization(const std::string& name, const DeviceCategory& category, int minLevel)
    : name_(name), category_(category), minLevel_(minLevel) {}

bool Specialization::matches(const DeviceCategory& c, int techLevel) const {
    return c.getName() == category_.getName() && techLevel >= minLevel_;
}

const std::string& Specialization::getName() const { return name_; }
const DeviceCategory& Specialization::getCategory() const { return category_; }
int Specialization::getMinLevel() const { return minLevel_; }

bool Specialization::operator==(const Specialization& other) const {
    return name_ == other.name_ && category_ == other.category_ && minLevel_ == other.minLevel_;
}

bool Specialization::operator!=(const Specialization& other) const { return !(*this == other); }

std::ostream& operator<<(std::ostream& os, const Specialization& s) {
    return os << s.name_ << " (" << s.category_.getName() << ", с уровня " << s.minLevel_ << ")";
}
