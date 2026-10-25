#include "devices/Laptop.h"

Laptop::Laptop(const std::string& serial, const std::string& model, const Brand& brand,
               const DeviceCategory& category, const Date& purchased,
               const std::string& cpu, int ramGb, const std::string& storage)
    : Device(serial, model, brand, category, purchased),
      cpu_(cpu), ramGb_(ramGb), storage_(storage) {}

std::string Laptop::typeName() const { return "Ноутбук"; }

int Laptop::estimateRepairDays() const {
    return Device::estimateRepairDays() + (needsBackup() ? 2 : 0);
}

Money Laptop::diagnosticPrice() const {
    return Device::diagnosticPrice() * 1.5;
}

std::vector<Defect> Laptop::quickTest(const Date& today) const {
    std::vector<Defect> result = Device::quickTest(today);
    if (ramGb_ < 4) result.push_back(Defect("RAM", "Мало оперативной памяти", 2));
    if (needsBackup()) result.push_back(Defect("HDD", "Устаревший жёсткий диск", 2));
    return result;
}

void Laptop::print(std::ostream& os) const {
    Device::print(os);
    os << ", " << cpu_ << ", " << ramGb_ << " ГБ, " << storage_;
}

bool Laptop::needsBackup() const { return storage_ == "HDD"; }

const std::string& Laptop::getCpu() const { return cpu_; }
int Laptop::getRamGb() const { return ramGb_; }
const std::string& Laptop::getStorage() const { return storage_; }