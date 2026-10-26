#include "service/ReportGenerator.h"
#include <sstream>
#include "finance/Invoice.h"
#include "repair/RepairOrder.h"
#include "people/Technician.h"
#include "inventory/Warehouse.h"

ReportGenerator::ReportGenerator(const Date& from, const Date& to) : from_(from), to_(to) {}

bool ReportGenerator::inPeriod(const Date& d) const { return !(d < from_) && !(to_ < d); }

Money ReportGenerator::revenue(const std::vector<Invoice>& all) const {
    Money sum;
    for (size_t i = 0; i < all.size(); ++i)
        if (all[i].isPaid() && inPeriod(all[i].getDate())) sum += all[i].total();
    return sum;
}

std::string ReportGenerator::technicianStats(const Technician& t, const std::vector<RepairOrder>& orders) const {
    int tasks = 0, done = 0;
    double hours = 0;
    for (size_t i = 0; i < orders.size(); ++i) {
        if (!inPeriod(orders[i].getCreated())) continue;
        const std::vector<RepairTask>& list = orders[i].getTasks();
        for (size_t k = 0; k < list.size(); ++k) {
            if (list[k].getTechnicianId() != t.getId()) continue;
            ++tasks;
            if (list[k].isDone()) {
                ++done;
                hours += list[k].getHours();
            }
        }
    }
    std::ostringstream out;
    out << t.getName() << " (уровень " << t.getLevel() << "): задач за период " << tasks << ", закрыто " << done
        << ", часов " << hours << ", загрузка " << (int)(t.workload() * 100) << "%";
    return out.str();
}

std::string ReportGenerator::stockReport(const Warehouse& w) const {
    std::ostringstream out;
    out << "Склад: позиций " << w.getItems().size() << ", стоимость " << w.totalValue();
    std::vector<StockItem> low = w.lowStock();
    if (low.empty()) {
        out << ", дозаказ не нужен";
    } else {
        out << "\nДозаказать:";
        for (size_t i = 0; i < low.size(); ++i) out << "\n  " << low[i];
    }
    return out.str();
}

Money ReportGenerator::salaryOf(const Employee& e, int month) const { return e.calculateSalary(month); }

int ReportGenerator::overdueCount(const std::vector<RepairOrder>& orders, const Date& today) const {
    int n = 0;
    for (size_t i = 0; i < orders.size(); ++i)
        if (orders[i].isOverdue(today)) ++n;
    return n;
}

std::string ReportGenerator::checkStatus(const ICertifiable& item, const Date& today) const {
    return item.needsCheck(today) ? "требуется проверка" : "в порядке";
}

Money ReportGenerator::unpaid(const IPayable& item) const { return item.amountDue(); }

const Date& ReportGenerator::getFrom() const { return from_; }
const Date& ReportGenerator::getTo() const { return to_; }

std::ostream& operator<<(std::ostream& os, const ReportGenerator& r) {
    return os << "Отчёт за период " << r.from_ << " - " << r.to_;
}
