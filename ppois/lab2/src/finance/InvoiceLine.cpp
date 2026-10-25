#include "finance/InvoiceLine.h"

InvoiceLine::InvoiceLine(const std::string& name, int qty, const Money& price)
    : name_(name), qty_(qty), price_(price) {}

Money InvoiceLine::sum() const { return price_ * qty_; }

const std::string& InvoiceLine::getName() const { return name_; }
int InvoiceLine::getQty() const { return qty_; }
const Money& InvoiceLine::getPrice() const { return price_; }

bool InvoiceLine::operator==(const InvoiceLine& other) const {
    return name_ == other.name_ && qty_ == other.qty_ && price_ == other.price_;
}

bool InvoiceLine::operator!=(const InvoiceLine& other) const { return !(*this == other); }

std::ostream& operator<<(std::ostream& os, const InvoiceLine& l) {
    return os << l.name_ << ": " << l.qty_ << " x " << l.price_ << " = " << l.sum();
}
