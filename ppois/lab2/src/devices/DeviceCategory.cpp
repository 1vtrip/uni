#include "devices/DeviceCategory.h"

DeviceCategory::DeviceCategory(const std::string& name, const Money& diagnosticPrice, int avgDays)
    : name_(name), diagnosticPrice_(diagnosticPrice), avgDays_(avgDays) {}

const std::string& DeviceCategory::getName() const { return name_; }

const Money& DeviceCategory::getDiagnosticPrice() const { return diagnosticPrice_; }

int DeviceCategory::getAvgDays() const { return avgDays_; }

int DeviceCategory::estimateDays(int complexity) const {
    return getAvgDays() + (complexity - 1) * 2;
}

bool DeviceCategory::operator==(const DeviceCategory& other) const {
    return this->getName() == other.getName() &&
        this->getAvgDays() == other.getAvgDays() &&
            this->getDiagnosticPrice() == other.getDiagnosticPrice();
}

bool DeviceCategory::operator!=(const DeviceCategory& other) const {
    return this->getName() != other.getName() ||
        this->getAvgDays() != other.getAvgDays() ||
            this->getDiagnosticPrice() != other.getDiagnosticPrice();
}

std::ostream& operator<<(std::ostream& os, const DeviceCategory& c) {
    return os << c.getName() << ": ~" << c.getAvgDays() << " дн., диагностика " << c.getDiagnosticPrice();
}