#include "repair/RepairTask.h"
#include "devices/Device.h"

RepairTask::RepairTask(int id, const std::string& description)
    : id_(id), description_(description), technicianId_(0), hours_(0), done_(false) {}

bool RepairTask::start(int technicianId) {
    if (done_ || technicianId <= 0) return false;
    if (technicianId_ != 0 && technicianId_ != technicianId) return false;
    technicianId_ = technicianId;
    return true;
}

bool RepairTask::finish(double hours) {
    if (!isStarted() || done_ || hours <= 0)
        return false;   // TODO(exc): RepairFirmException("Задача №... не начата или уже закрыта")
    hours_ = hours;
    done_ = true;
    return true;
}

bool RepairTask::addPart(const SparePart& part, int qty, const Device& d) {
    if (done_ || qty <= 0) return false;
    if (!part.fits(d))
        return false;   // TODO(exc): IncompatiblePartException(part.getArticle(), d.getModel())
    parts_.push_back(UsedPart(part.getArticle(), qty, part.getSellPrice()));
    return true;
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
