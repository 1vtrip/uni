#include "people/EmployeeClient.h"

/** @brief Вызовы Person(...) внутри Employee и Client при этом игнорируются: виртуальная база одна. */
EmployeeClient::EmployeeClient(int id, const std::string& name, const ContactInfo& contacts, const Date& birth,
                               const std::string& position, const Date& hired, const Money& baseSalary,
                               const Date& registered)
    : Person(id, name, contacts, birth),
      Employee(id, name, contacts, birth, position, hired, baseSalary),
      Client(id, name, contacts, birth, registered) {}

std::string EmployeeClient::getRole() const { return "Сотрудник-клиент"; }

void EmployeeClient::print(std::ostream& os) const {
    Person::print(os);
    printEmployeePart(os);
    printClientPart(os);
}

Money EmployeeClient::calculateSalary(int month) const { return Employee::calculateSalary(month); }

Money EmployeeClient::applyDiscount(const Money& sum) const { return sum - sum.percent(30); }

int EmployeeClient::maxActiveOrders() const { return 5; }
