#include "people/Employee.h"

Employee::Employee(int id, const std::string& name, const ContactInfo& contacts, const Date& birth,
                   const std::string& position, const Date& hired, const Money& baseSalary)
    : Person(id, name, contacts, birth), position_(position), hired_(hired), baseSalary_(baseSalary) {}

/** @brief Реализация чисто виртуальной функции: общая часть зарплаты для всех наследников. */
Money Employee::calculateSalary(int) const { return baseSalary_; }

bool Employee::canWorkOn(const Device&) const { return false; }

void Employee::print(std::ostream& os) const {
    Person::print(os);
    os << ", " << position_ << ", оклад " << baseSalary_;
}

int Employee::experienceYears(const Date& today) const { return hired_.monthsTo(today) / 12; }

bool Employee::addShift(const Shift& s) { return schedule_.add(s); }

int Employee::hoursWorked() const { return schedule_.totalHours(); }

const std::string& Employee::getPosition() const { return position_; }
const Date& Employee::getHired() const { return hired_; }
const Money& Employee::getBaseSalary() const { return baseSalary_; }
const Schedule& Employee::getSchedule() const { return schedule_; }
