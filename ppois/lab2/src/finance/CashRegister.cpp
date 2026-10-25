#include "finance/CashRegister.h"
#include <sstream>

CashRegister::CashRegister(const Money& initial) : balance_(initial) {}

/** @brief Запись собирается через ostringstream: у Money есть только operator<<. */
void CashRegister::deposit(const Money& sum, const std::string& note) {
    balance_ += sum;
    std::ostringstream out;
    out << "+ " << sum << " " << note;
    log_.push_back(out.str());
}

bool CashRegister::withdraw(const Money& sum, const std::string& note) {
    if (balance_ < sum) return false;   // TODO(exc): InsufficientFundsException("В кассе недостаточно денег: " + note)
    balance_ -= sum;
    std::ostringstream out;
    out << "- " << sum << " " << note;
    log_.push_back(out.str());
    return true;
}

std::string CashRegister::report() const {
    std::ostringstream out;
    out << "Журнал кассы:";
    for (size_t i = 0; i < log_.size(); ++i)
        out << "\n  " << log_[i];
    out << "\nОстаток: " << balance_;
    return out.str();
}

const Money& CashRegister::getBalance() const { return balance_; }
const std::vector<std::string>& CashRegister::getLog() const { return log_; }

std::ostream& operator<<(std::ostream& os, const CashRegister& c) {
    return os << "Касса: " << c.balance_ << ", операций " << c.log_.size();
}
