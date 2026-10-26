#include "people/Courier.h"

Courier::Courier(int id, const std::string& name, const ContactInfo& contacts, const Date& birth,
                 const Date& hired, const Money& baseSalary, const std::string& carNumber)
    : Employee(id, name, contacts, birth, "Курьер", hired, baseSalary),
      carNumber_(carNumber), deliveredCount_(0) {}

std::string Courier::getRole() const { return "Курьер"; }

void Courier::print(std::ostream& os) const {
    Employee::print(os);
    os << ", машина " << carNumber_ << ", в пути " << deliveryIds_.size() << ", доставлено " << deliveredCount_;
}

Money Courier::calculateSalary(int month) const {
    return Employee::calculateSalary(month) + Money::fromRubles(5.0) * deliveredCount_;
}

bool Courier::pickUp(int deliveryId) {
    for (size_t i = 0; i < deliveryIds_.size(); ++i)
        if (deliveryIds_[i] == deliveryId) return false;
    deliveryIds_.push_back(deliveryId);
    return true;
}

bool Courier::deliver(int deliveryId) {
    for (size_t i = 0; i < deliveryIds_.size(); ++i) {
        if (deliveryIds_[i] == deliveryId) {
            deliveryIds_.erase(deliveryIds_.begin() + i);
            ++deliveredCount_;
            return true;
        }
    }
    return false;   // TODO(exc): RepairFirmException("У курьера нет доставки №...")
}

int Courier::inTransit() const { return (int)deliveryIds_.size(); }

const std::string& Courier::getCarNumber() const { return carNumber_; }
const std::vector<int>& Courier::getDeliveryIds() const { return deliveryIds_; }
int Courier::getDeliveredCount() const { return deliveredCount_; }
