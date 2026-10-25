#include "repair/Diagnosis.h"

Diagnosis::Diagnosis() : technicianId_(0), repairable_(true), date_() {}

Diagnosis::Diagnosis(int technicianId, const Date& date)
    : technicianId_(technicianId), repairable_(true), date_(date) {}

void Diagnosis::addDefect(const Defect& d) { defects_.push_back(d); }

void Diagnosis::addDefects(const std::vector<Defect>& list) {
    for (size_t i = 0; i < list.size(); ++i) defects_.push_back(list[i]);
}

double Diagnosis::totalHours() const {
    double hours = 0;
    for (size_t i = 0; i < defects_.size(); ++i) hours += defects_[i].laborHours();
    return hours;
}

Money Diagnosis::estimate(const PriceList& prices) const {
    return prices.laborCost(totalHours(), false);
}

int Diagnosis::criticalCount() const {
    int n = 0;
    for (size_t i = 0; i < defects_.size(); ++i)
        if (defects_[i].isCritical()) ++n;
    return n;
}

bool Diagnosis::decideRepairable() {
    repairable_ = criticalCount() <= 2;
    return repairable_;
}

bool Diagnosis::isDone() const { return technicianId_ != 0; }

int Diagnosis::getTechnicianId() const { return technicianId_; }
const std::vector<Defect>& Diagnosis::getDefects() const { return defects_; }
bool Diagnosis::isRepairable() const { return repairable_; }
const Date& Diagnosis::getDate() const { return date_; }

bool Diagnosis::operator==(const Diagnosis& other) const {
    return technicianId_ == other.technicianId_ && date_ == other.date_
        && repairable_ == other.repairable_ && defects_ == other.defects_;
}

bool Diagnosis::operator!=(const Diagnosis& other) const { return !(*this == other); }

std::ostream& operator<<(std::ostream& os, const Diagnosis& d) {
    if (!d.isDone()) return os << "Диагностика не проводилась";
    os << "Диагностика от " << d.date_ << ", мастер #" << d.technicianId_ << ", дефектов " << d.defects_.size()
       << ", " << d.totalHours() << " ч, " << (d.repairable_ ? "ремонтопригодно" : "НЕРЕМОНТОПРИГОДНО");
    for (size_t i = 0; i < d.defects_.size(); ++i)
        os << "\n  " << d.defects_[i];
    return os;
}
