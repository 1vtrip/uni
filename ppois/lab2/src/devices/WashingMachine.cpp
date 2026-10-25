#include "devices/WashingMachine.h"

WashingMachine::WashingMachine(const std::string& serial, const std::string& model, const Brand& brand,
                               const DeviceCategory& category, const Date& purchased,
                               int power, double weight, bool builtIn,
                               double loadKg, const std::string& drum)
    : HomeAppliance(serial, model, brand, category, purchased, power, weight, builtIn),
      loadKg_(loadKg), drum_(drum) {}

std::string WashingMachine::typeName() const { return "Стиральная машина"; }

std::vector<Defect> WashingMachine::quickTest(const Date& today) const {
    std::vector<Defect> result = HomeAppliance::quickTest(today);
    if (!spinTest()) result.push_back(Defect("SPIN", "Не работает отжим", 4));
    return result;
}

void WashingMachine::print(std::ostream& os) const {
    HomeAppliance::print(os);
    os << ", загрузка " << loadKg_ << " кг, барабан: " << drum_;
}

bool WashingMachine::spinTest() const { return drum_ != "Повреждён"; }

double WashingMachine::getLoadKg() const { return loadKg_; }

const std::string& WashingMachine::getDrum() const { return drum_; }
