#include "repair/RepairTask.h"
#include "devices/Device.h"
#include "exceptions/Exceptions.h"

RepairTask::RepairTask(int id, const std::string& description)
    : id_(id), description_(description), technicianId_(0), hours_(0), done_(false) {}

bool RepairTask::start(int technicianId) {
    if (done_ || technicianId <= 0) return false;
    if (technicianId_ != 0 && technicianId_ != technicianId) return false;
    technicianId_ = technicianId;
    return true;
}

void RepairTask::finish(double hours) {
    if (!isStarted()) throw RepairFirmException("Задача №" + std::to_string(id_) + " ещё не начата");
    if (done_) throw RepairFirmException("Задача №" + std::to_string(id_) + " уже закрыта");
    if (hours <= 0) throw RepairFirmException("Часы работы должны быть больше нуля");
    hours_ = hours;
    done_ = true;
}

void RepairTask::addPart(const SparePart& part, int qty, const Device& d) {
    if (done_) throw RepairFirmException("Задача №" + std::to_string(id_) + " уже закрыта");
    if (qty <= 0) throw RepairFirmException("Количество запчастей должно быть больше нуля");
    if (!part.fits(d)) throw IncompatiblePartException(part.getArticle(), d.getModel());
    parts_.push_back(UsedPart(part.getArticle(), qty, part.getSellPrice()));
}

Money RepairTask::partsCost() const {
    Money sum;
    for (size_t i = 0; i < parts_.size(); ++i) sum += parts_[i].sum();
    return sum;
}

Money RepairTask::cost(const PriceList& prices, bool urgent) const {
    return prices.laborCost(hours_, urgent) + partsCost();
}

bool RepairTask::isStarted() const { return technicianId_ != 0; }

int RepairTask::getId() const { return id_; }
const std::string& RepairTask::getDescription() const { return description_; }
int RepairTask::getTechnicianId() const { return technicianId_; }
double RepairTask::getHours() const { return hours_; }
const std::vector<UsedPart>& RepairTask::getParts() const { return parts_; }
bool RepairTask::isDone() const { return done_; }

bool RepairTask::operator==(const RepairTask& other) const { return id_ == other.id_; }
bool RepairTask::operator!=(const RepairTask& other) const { return !(*this == other); }

std::ostream& operator<<(std::ostream& os, const RepairTask& t) {
    os << "Задача №" << t.id_ << " «" << t.description_ << "»: ";
    if (t.done_) os << "выполнена мастером #" << t.technicianId_ << " за " << t.hours_ << " ч";
    else if (t.isStarted()) os << "в работе у мастера #" << t.technicianId_;
    else os << "не назначена";
    for (size_t i = 0; i < t.parts_.size(); ++i)
        os << "\n    " << t.parts_[i];
    return os;
}
