#include "people/Receptionist.h"
#include "people/Client.h"
#include "finance/Invoice.h"
#include "exceptions/Exceptions.h"

Receptionist::Receptionist(int id, const std::string& name, const ContactInfo& contacts, const Date& birth,
                           const Date& hired, const Money& baseSalary)
    : Person(id, name, contacts, birth),
      Employee(id, name, contacts, birth, "Приёмщик", hired, baseSalary), acceptedCount_(0) {}

std::string Receptionist::getRole() const { return "Приёмщик"; }

void Receptionist::print(std::ostream& os) const {
    Employee::print(os);
    os << ", принято устройств " << acceptedCount_;
}

Money Receptionist::calculateSalary(int month) const {
    return Employee::calculateSalary(month) + Money::fromRubles(2.0) * acceptedCount_;
}

RepairOrder Receptionist::acceptDevice(int orderId, const Client& c, const Device& d, const std::string& problem,
                                       const Date& today, bool urgent) {
    ++acceptedCount_;
    return RepairOrder(orderId, c.getId(), d, problem, today, urgent);
}

void Receptionist::issueDevice(RepairOrder& o, const Invoice& inv) const {
    if (inv.getOrderId() != o.getId()) throw RepairFirmException("Счёт относится к другому заказу");
    if (!inv.isPaid()) throw RepairFirmException("Счёт по заказу №" + std::to_string(o.getId()) + " не оплачен");
    o.setStatus(OrderStatus::Status::Issued);
}

int Receptionist::getAcceptedCount() const { return acceptedCount_; }
