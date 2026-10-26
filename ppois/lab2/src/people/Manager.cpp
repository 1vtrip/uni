#include "people/Manager.h"
#include "people/Technician.h"
#include "repair/RepairOrder.h"
#include "finance/Discount.h"

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

bool Manager::assignTechnician(RepairOrder& o, int taskId, Technician& t) const {
    int i = o.indexOfTask(taskId);
    if (i == -1) return false;   // TODO(exc): RepairFirmException("Нет задачи №...")
    const RepairTask& task = o.taskAt(i);
    if (task.isDone() || task.isStarted()) return false;   // TODO(exc): RepairFirmException("Задача уже назначена")
    if (!t.canWorkOn(o.getDevice()) || !t.isAvailable())
        return false;   // TODO(exc): TechnicianNotAvailableException
    t.takeTask(taskId);
    o.taskAt(i).start(t.getId());
    return true;
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
