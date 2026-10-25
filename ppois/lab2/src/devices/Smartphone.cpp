#include "devices/Smartphone.h"

Smartphone::Smartphone(const std::string& serial, const std::string& model, const Brand& brand,
               const DeviceCategory& category, const Date& purchased,
               const std::string& imei, double screen, bool wet) :
Device(serial, model, brand, category, purchased), imei_(imei), screen_(screen), wet_(wet) {}

const std::string& Smartphone::getImei() const { return imei_; }

double Smartphone::getScreen() const { return screen_; }

bool Smartphone::isWet() const { return wet_; }

std::string Smartphone::typeName() const { return "Смартфон"; }

int Smartphone::estimateRepairDays() const {
    return Device::estimateRepairDays() + (wet_ ? 5 : 0);
}

std::vector<Defect> Smartphone::quickTest(const Date& today) const {
    std::vector<Defect> result = Device::quickTest(today);
    if (wet_) result.push_back(Defect("WATER", "Следы влаги", 5));
    if (!isImeiValid()) result.push_back(Defect("IMEI", "Некорректный IMEI", 3));
    return result;
}

/** @brief Каждая вторая цифра (нечётный индекс) удваивается, при > 9 вычитается 9; сумма кратна 10. */
bool Smartphone::isImeiValid() const {
    if (imei_.size() != 15) return false;
    int sum = 0;
    for (int i = 0; i < 15; ++i) {
        if (imei_[i] < '0' || imei_[i] > '9') return false;
        int d = imei_[i] - '0';
        if (i % 2 == 1) {
            d *= 2;
            if (d > 9) d -= 9;
        }
        sum += d;
    }
    return sum % 10 == 0;
}

Money Smartphone::screenReplacementCost() const {
    return Money::fromRubles(screen_ * 30.0);
}

void Smartphone::print(std::ostream& os) const {
    Device::print(os);
    os << ", IMEI " << imei_ << (wet_ ? ", после влаги" : "");
}