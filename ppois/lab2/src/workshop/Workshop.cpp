#include "workshop/Workshop.h"

Workshop::Workshop(const std::string& name, const Address& address) : name_(name), address_(address) {}

bool Workshop::addBench(const Workbench& b) {
    for (size_t i = 0; i < benches_.size(); ++i)
        if (benches_[i] == b) return false;
    benches_.push_back(b);
    return true;
}

bool Workshop::addTool(const Tool& t) {
    for (size_t i = 0; i < tools_.size(); ++i)
        if (tools_[i] == t) return false;
    tools_.push_back(t);
    return true;
}

int Workshop::benchOf(int techId) const {
    if (techId <= 0) return -1;
    for (size_t i = 0; i < benches_.size(); ++i)
        if (benches_[i].getTechnicianId() == techId) return benches_[i].getNumber();
    return -1;
}

/** @brief Место занимается прямо в benches_[i]: у копии изменение бы потерялось. */
int Workshop::assignBench(int techId) {
    if (techId <= 0) return -1;
    int current = benchOf(techId);
    if (current != -1) return current;
    for (size_t i = 0; i < benches_.size(); ++i)
        if (benches_[i].occupy(techId)) return benches_[i].getNumber();
    return -1;   // TODO(exc): WorkbenchOccupiedException("свободных мест нет")
}

bool Workshop::releaseBench(int number) {
    for (size_t i = 0; i < benches_.size(); ++i) {
        if (benches_[i].getNumber() == number) {
            benches_[i].release();
            return true;
        }
    }
    return false;
}

int Workshop::freeBenches() const {
    int n = 0;
    for (size_t i = 0; i < benches_.size(); ++i)
        if (benches_[i].isFree()) ++n;
    return n;
}

int Workshop::indexOfTool(const std::string& name) const {
    for (size_t i = 0; i < tools_.size(); ++i)
        if (tools_[i].getName() == name) return (int)i;
    return -1;
}

Tool& Workshop::toolAt(int index) { return tools_[index]; }
const Tool& Workshop::toolAt(int index) const { return tools_[index]; }

std::vector<Tool> Workshop::toolsToCalibrate(const Date& today) const {
    std::vector<Tool> result;
    for (size_t i = 0; i < tools_.size(); ++i)
        if (tools_[i].needsCalibration(today)) result.push_back(tools_[i]);
    return result;
}

const std::string& Workshop::getName() const { return name_; }
const Address& Workshop::getAddress() const { return address_; }
const std::vector<Workbench>& Workshop::getBenches() const { return benches_; }
const std::vector<Tool>& Workshop::getTools() const { return tools_; }

std::ostream& operator<<(std::ostream& os, const Workshop& w) {
    os << "Мастерская «" << w.name_ << "», " << w.address_ << ", свободно мест " << w.freeBenches()
       << " из " << w.benches_.size();
    for (size_t i = 0; i < w.benches_.size(); ++i) os << "\n  " << w.benches_[i];
    for (size_t i = 0; i < w.tools_.size(); ++i) os << "\n  " << w.tools_[i];
    return os;
}
