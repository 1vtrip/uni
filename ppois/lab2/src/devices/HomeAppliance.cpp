#include "devices/HomeAppliance.h"

HomeAppliance::HomeAppliance(const std::string& serial, const std::string& model, const Brand& brand,
                             const DeviceCategory& category, const Date& purchased,
                             int power, double weight, bool builtIn)
    : Device(serial, model, brand, category, purchased),
      power_(power), weight_(weight), builtIn_(builtIn) {}

std::string HomeAppliance::typeName() const { return "Бытовая техника"; }

bool HomeAppliance::needsHomeVisit() const {
    return builtIn_ || weight_ > 30;
}

int HomeAppliance::estimateRepairDays() const {
    return Device::estimateRepairDays() + 3;
}

Money HomeAppliance::diagnosticPrice() const {
    Money price = Device::diagnosticPrice();
    if (needsHomeVisit()) price += Money::fromRubles(20);
    return price;
}

void HomeAppliance::print(std::ostream& os) const {
    Device::print(os);
    os << ", " << power_ << " Вт, " << weight_ << " кг" << (builtIn_ ? ", встраиваемая" : "");
}

Money HomeAppliance::transportCost(const Address& from, const Address& to) const {
    if (from.sameCity(to))
        return Money::fromRubles(15 + 0.5 * weight_);
    return Money::fromRubles(50 + 1.0 * weight_);
}

int HomeAppliance::getPower() const { return power_; }

double HomeAppliance::getWeight() const { return weight_; }

bool HomeAppliance::isBuiltIn() const { return builtIn_; }