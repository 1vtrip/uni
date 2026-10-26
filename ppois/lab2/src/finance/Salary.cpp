#include "finance/Salary.h"

Salary::Salary(int employeeId, int month, const Money& base)
    : employeeId_(employeeId), month_(month), base_(base), bonus_(), deductions_(), paid_(false) {}

bool Salary::addBonus(const Money& m) {
    if (m < Money()) return false;
    bonus_ += m;
    return true;
}

bool Salary::addDeduction(const Money& m) {
    if (m < Money()) return false;
    deductions_ += m;
    return true;
}

Money Salary::gross() const {
    Money sum = base_ + bonus_ - deductions_;
    if (sum < Money()) return Money();
    return sum;
}

Money Salary::tax() const { return gross().percent(13); }

Money Salary::net() const { return gross() - tax(); }

Money Salary::amountDue() const { return paid_ ? Money() : net(); }
bool Salary::isPaid() const { return paid_; }
void Salary::markPaid() { paid_ = true; }

int Salary::getEmployeeId() const { return employeeId_; }
int Salary::getMonth() const { return month_; }
const Money& Salary::getBase() const { return base_; }
const Money& Salary::getBonus() const { return bonus_; }
const Money& Salary::getDeductions() const { return deductions_; }

bool Salary::operator==(const Salary& other) const {
    return employeeId_ == other.employeeId_ && month_ == other.month_;
}

bool Salary::operator!=(const Salary& other) const { return !(*this == other); }

std::ostream& operator<<(std::ostream& os, const Salary& s) {
    return os << "Зарплата сотрудника #" << s.employeeId_ << " за месяц " << s.month_
              << ": оклад " << s.base_ << ", премии " << s.bonus_ << ", удержания " << s.deductions_
              << ", налог " << s.tax() << ", к выплате " << s.net() << (s.paid_ ? " [выдана]" : "");
}
