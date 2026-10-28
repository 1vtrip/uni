#include "workshop/Delivery.h"
#include "people/Courier.h"
#include "exceptions/Exceptions.h"

Delivery::Delivery(int id, int orderId, const Address& from, const Address& to)
    : id_(id), orderId_(orderId), from_(from), to_(to), courierId_(0), done_(false) {}

bool Delivery::assignCourier(const Courier& c) {
    if (done_) return false;
    courierId_ = c.getId();
    return true;
}

bool Delivery::isAssigned() const { return courierId_ != 0; }

Money Delivery::cost() const {
    return Money::fromRubles(from_.sameCity(to_) ? 10 : 40);
}

void Delivery::complete() {
    if (!isAssigned()) throw RepairFirmException("Доставке №" + std::to_string(id_) + " не назначен курьер");
    if (done_) throw RepairFirmException("Доставка №" + std::to_string(id_) + " уже выполнена");
    done_ = true;
}

int Delivery::getId() const { return id_; }
int Delivery::getOrderId() const { return orderId_; }
const Address& Delivery::getFrom() const { return from_; }
const Address& Delivery::getTo() const { return to_; }
int Delivery::getCourierId() const { return courierId_; }
bool Delivery::isDone() const { return done_; }

bool Delivery::operator==(const Delivery& other) const { return id_ == other.id_; }
bool Delivery::operator!=(const Delivery& other) const { return !(*this == other); }

std::ostream& operator<<(std::ostream& os, const Delivery& d) {
    os << "Доставка №" << d.id_ << " по заказу №" << d.orderId_ << ": " << d.from_ << " -> " << d.to_
       << ", " << d.cost();
    if (d.done_) return os << " [доставлено курьером #" << d.courierId_ << "]";
    if (d.isAssigned()) return os << " [в пути, курьер #" << d.courierId_ << "]";
    return os << " [курьер не назначен]";
}