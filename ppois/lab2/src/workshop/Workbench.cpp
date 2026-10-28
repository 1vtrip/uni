#include "workshop/Workbench.h"
#include "exceptions/Exceptions.h"

Workbench::Workbench(int number, const std::string& equipment)
    : number_(number), equipment_(equipment), technicianId_(0) {}

bool Workbench::isFree() const { return technicianId_ == 0; }

void Workbench::occupy(int techId) {
    if (techId <= 0) throw WorkbenchOccupiedException("место №" + std::to_string(number_) + " нельзя занять без мастера");
    if (!isFree())
        throw WorkbenchOccupiedException("место №" + std::to_string(number_) + " (мастер #" + std::to_string(technicianId_) + ")");
    technicianId_ = techId;
}

void Workbench::release() { technicianId_ = 0; }

int Workbench::getNumber() const { return number_; }
const std::string& Workbench::getEquipment() const { return equipment_; }
int Workbench::getTechnicianId() const { return technicianId_; }

bool Workbench::operator==(const Workbench& other) const { return number_ == other.number_; }
bool Workbench::operator!=(const Workbench& other) const { return !(*this == other); }

std::ostream& operator<<(std::ostream& os, const Workbench& w) {
    os << "Место №" << w.number_ << " (" << w.equipment_ << "): ";
    if (w.isFree()) return os << "свободно";
    return os << "занято мастером #" << w.technicianId_;
}
