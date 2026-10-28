#include "people/Accountant.h"
#include "repair/RepairOrder.h"
#include "finance/PriceList.h"
#include "finance/Payment.h"
#include "finance/CashRegister.h"

Accountant::Accountant(int id, const std::string& name, const ContactInfo& contacts, const Date& birth,
                       const Date& hired, const Money& baseSalary, const std::string& certificate)
    : Person(id, name, contacts, birth),
      Employee(id, name, contacts, birth, "Бухгалтер", hired, baseSalary), certificate_(certificate) {}

std::string Accountant::getRole() const { return "Бухгалтер"; }

void Accountant::print(std::ostream& os) const {
    Employee::print(os);
    os << ", аттестат " << certificate_;
}

Money Accountant::calculateSalary(int month) const { return Employee::calculateSalary(month); }

Invoice Accountant::makeInvoice(int invoiceId, const RepairOrder& o, const PriceList& p, const Date& today) const {
    Invoice inv(invoiceId, o.getId(), o.getClientId(), today);
    inv.addLine(InvoiceLine("Диагностика", 1, o.getDiagnosticPrice()));
    const std::vector<RepairTask>& tasks = o.getTasks();
    for (size_t i = 0; i < tasks.size(); ++i) {
        if (tasks[i].isDone())
            inv.addLine(InvoiceLine("Работа: " + tasks[i].getDescription(), 1,
                                    p.laborCost(tasks[i].getHours(), o.isUrgent())));
        const std::vector<UsedPart>& parts = tasks[i].getParts();
        for (size_t k = 0; k < parts.size(); ++k)
            inv.addLine(InvoiceLine("Запчасть " + parts[k].getArticle(), parts[k].getQty(), parts[k].getPrice()));
    }
    return inv;
}

Salary Accountant::makeSalary(const Employee& e, int month) const {
    return Salary(e.getId(), month, e.calculateSalary(month));
}

void Accountant::refund(Payment& p, CashRegister& cash) const { p.refund(cash); }

const std::string& Accountant::getCertificate() const { return certificate_; }
