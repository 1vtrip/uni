#include "people/Manager.h"
#include "people/Technician.h"
#include "repair/RepairOrder.h"
#include "finance/Discount.h"
#include "exceptions/Exceptions.h"

Manager::Manager(int id, const std::string& name, const ContactInfo& contacts, const Date& birth,
                 const Date& hired, const Money& baseSalary, const std::string& department, double bonusPercent)
    : Person(id, name, contacts, birth),
      Employee(id, name, contacts, birth, "Менеджер", hired, baseSalary),
      department_(department), bonusPercent_(bonusPercent) {}

std::string Manager::getRole() const { return "Менеджер"; }

void Manager::print(std::ostream& os) const {
    Employee::print(os);
    os << ", отдел «" << department_ << "», премия " << bonusPercent_ << "%";
}

Money Manager::calculateSalary(int month) const {
    Money base = Employee::calculateSalary(month);
    return base + base.percent(bonusPercent_);
}

void Manager::assignTechnician(RepairOrder& o, int taskId, Technician& t) const {
    RepairTask& task = o.findTask(taskId);
    if (task.isDone() || task.isStarted())
        throw RepairFirmException("Задача №" + std::to_string(taskId) + " уже назначена или закрыта");
    if (!t.canWorkOn(o.getDevice()))
        throw TechnicianNotAvailableException(t.getName() + " не работает с такой техникой");
    if (!t.isAvailable())
        throw TechnicianNotAvailableException(t.getName() + " загружен полностью");
    t.takeTask(taskId);
    task.start(t.getId());
}

void Manager::approveDiscount(Discount& d) const { d.approve(); }

int Manager::evaluate(const Technician& t, const std::vector<RepairOrder>& orders, const Date& today) const {
    int score = 3;
    if (t.getCompletedTasks() >= 10) ++score;
    if (t.getCompletedTasks() >= 30) ++score;
    for (size_t i = 0; i < orders.size(); ++i) {
        if (!orders[i].isOverdue(today)) continue;
        const std::vector<RepairTask>& tasks = orders[i].getTasks();
        for (size_t k = 0; k < tasks.size(); ++k) {
            if (tasks[k].getTechnicianId() == t.getId()) {
                --score;
                break;
            }
        }
    }
    if (score < 1) score = 1;
    if (score > 5) score = 5;
    return score;
}

const std::string& Manager::getDepartment() const { return department_; }
double Manager::getBonusPercent() const { return bonusPercent_; }
