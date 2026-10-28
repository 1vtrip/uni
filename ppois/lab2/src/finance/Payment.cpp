#include "finance/Payment.h"
#include "exceptions/Exceptions.h"
#include <string>

Payment::Payment(int invoiceId, const Money& amount, const PaymentMethod& method, const Date& date)
    : invoiceId_(invoiceId), amount_(amount), method_(method), date_(date),
      processed_(false), refunded_(false), deposited_() {}

void Payment::process(Invoice& inv, CashRegister& cash) {
    if (processed_) throw RepairFirmException("Платёж уже проведён");
    if (inv.getId() != invoiceId_) throw RepairFirmException("Платёж относится к другому счёту");
    if (inv.isPaid()) throw RepairFirmException("Счёт №" + std::to_string(invoiceId_) + " уже оплачен");
    if (amount_ < inv.total()) throw InsufficientFundsException("внесено меньше суммы счёта");
    deposited_ = method_.netAmount(inv.total());
    cash.deposit(deposited_, "оплата счёта №" + std::to_string(invoiceId_) + " (" + method_.getType() + ")");
    inv.markPaid();
    processed_ = true;
}

/** @brief Если withdraw бросит исключение, refunded_ не изменится: платёж останется корректным. */
void Payment::refund(CashRegister& cash) {
    if (!processed_) throw RepairFirmException("Платёж ещё не проведён");
    if (refunded_) throw RepairFirmException("Деньги по платежу уже возвращены");
    cash.withdraw(deposited_, "возврат по счёту №" + std::to_string(invoiceId_));
    refunded_ = true;
}

Money Payment::change(const Invoice& inv) const {
    if (amount_ < inv.total()) return Money();
    return amount_ - inv.total();
}

int Payment::getInvoiceId() const { return invoiceId_; }
const Money& Payment::getAmount() const { return amount_; }
const PaymentMethod& Payment::getMethod() const { return method_; }
const Date& Payment::getDate() const { return date_; }
bool Payment::isProcessed() const { return processed_; }
bool Payment::isRefunded() const { return refunded_; }
const Money& Payment::getDeposited() const { return deposited_; }

bool Payment::operator==(const Payment& other) const {
    return invoiceId_ == other.invoiceId_ && amount_ == other.amount_
        && method_ == other.method_ && date_ == other.date_;
}

bool Payment::operator!=(const Payment& other) const { return !(*this == other); }

std::ostream& operator<<(std::ostream& os, const Payment& p) {
    os << "Платёж по счёту №" << p.invoiceId_ << " от " << p.date_ << ": " << p.amount_ << ", " << p.method_;
    if (p.refunded_) os << " [ВОЗВРАТ]";
    else if (p.processed_) os << " [проведён, в кассу " << p.deposited_ << "]";
    else os << " [не проведён]";
    return os;
}
