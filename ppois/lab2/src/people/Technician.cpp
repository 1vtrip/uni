#include "people/Technician.h"
#include "devices/Device.h"
#include "exceptions/Exceptions.h"

Technician::Technician(int id, const std::string& name, const ContactInfo& contacts, const Date& birth,
                       const Date& hired, const Money& baseSalary, int level, int maxTasks)
    : Person(id, name, contacts, birth),
      Employee(id, name, contacts, birth, "Мастер", hired, baseSalary),
      level_(level), completedTasks_(0), maxTasks_(maxTasks), certified_(hired) {}

std::string Technician::getRole() const { return "Мастер"; }

void Technician::print(std::ostream& os) const {
    Employee::print(os);
    os << ", уровень " << level_ << ", задач " << activeTaskIds_.size() << "/" << maxTasks_
       << ", закрыто " << completedTasks_;
}

Money Technician::calculateSalary(int month) const {
    Money base = Employee::calculateSalary(month);
    return base + base.percent(5.0 * level_) + Money::fromRubles(15.0) * completedTasks_;
}

bool Technician::canWorkOn(const Device& d) const {
    for (size_t i = 0; i < specs_.size(); ++i)
        if (specs_[i].matches(d.getCategory(), level_)) return true;
    return false;
}

void Technician::addSpecialization(const Specialization& s) {
    for (size_t i = 0; i < specs_.size(); ++i)
        if (specs_[i] == s) return;
    specs_.push_back(s);
}

void Technician::takeTask(int taskId) {
    if (!isAvailable()) throw TechnicianNotAvailableException(name_ + " загружен полностью");
    for (size_t i = 0; i < activeTaskIds_.size(); ++i)
        if (activeTaskIds_[i] == taskId)
            throw RepairFirmException("Задача №" + std::to_string(taskId) + " уже у мастера " + name_);
    activeTaskIds_.push_back(taskId);
}

void Technician::finishTask(int taskId) {
    for (size_t i = 0; i < activeTaskIds_.size(); ++i) {
        if (activeTaskIds_[i] == taskId) {
            activeTaskIds_.erase(activeTaskIds_.begin() + i);
            ++completedTasks_;
            if (completedTasks_ % 20 == 0) upgradeLevel();
            return;
        }
    }
    throw RepairFirmException("У мастера " + name_ + " нет задачи №" + std::to_string(taskId));
}

bool Technician::isAvailable() const { return (int)activeTaskIds_.size() < maxTasks_; }

double Technician::workload() const {
    if (maxTasks_ <= 0) return 1.0;
    return (double)activeTaskIds_.size() / maxTasks_;
}

void Technician::upgradeLevel() { if (level_ < 5) ++level_; }

int Technician::getLevel() const { return level_; }
int Technician::getCompletedTasks() const { return completedTasks_; }
int Technician::getMaxTasks() const { return maxTasks_; }
const std::vector<int>& Technician::getActiveTaskIds() const { return activeTaskIds_; }
const std::vector<Specialization>& Technician::getSpecializations() const { return specs_; }

/** @brief Первая аттестация считается проведённой в день найма. */
bool Technician::needsCheck(const Date& today) const { return certified_.daysTo(today) > 365; }
void Technician::certify(const Date& today) { certified_ = today; }
const Date& Technician::getCertified() const { return certified_; }
