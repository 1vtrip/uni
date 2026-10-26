#include "devices/Device.h"

Device::Device(const std::string& serial, const std::string& model, const Brand& brand,
               const DeviceCategory& category, const Date& purchased)
    : serial_(serial), model_(model), brand_(brand), category_(category), purchased_(purchased) {}

std::string Device::typeName() const { return "Устройство"; }

int Device::estimateRepairDays() const { return category_.estimateDays(1); }

Money Device::diagnosticPrice() const { return category_.getDiagnosticPrice(); }

bool Device::needsHomeVisit() const { return false; }

/** @brief Базовая проверка: только износ. Наследники добавляют свои дефекты к этому списку. */
std::vector<Defect> Device::quickTest(const Date& today) const {
    std::vector<Defect> result;
    if (ageMonths(today) > 60)
        result.push_back(Defect("AGE", "Сильный износ (старше 5 лет)", 2));
    return result;
}

void Device::print(std::ostream& os) const {
    os << typeName() << " " << brand_.getName() << " " << model_ << " (S/N " << serial_ << ")";
}

int Device::ageMonths(const Date& today) const { return purchased_.monthsTo(today); }

bool Device::underFactoryWarranty(const Date& today) const {
    return ageMonths(today) < 12 && brand_.allowsWarrantyRepair();
}

const std::string& Device::getSerial() const { return serial_; }
const std::string& Device::getModel() const { return model_; }
const Brand& Device::getBrand() const { return brand_; }
const DeviceCategory& Device::getCategory() const { return category_; }
const Date& Device::getPurchased() const { return purchased_; }

bool Device::operator==(const Device& other) const { return serial_ == other.serial_; }
bool Device::operator!=(const Device& other) const { return !(*this == other); }
