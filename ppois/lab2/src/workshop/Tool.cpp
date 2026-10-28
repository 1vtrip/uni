#include "workshop/Tool.h"
#include "exceptions/Exceptions.h"

Tool::Tool(const std::string& invNumber, const std::string& name, const Date& calibrated)
    : invNumber_(invNumber), name_(name), calibrated_(calibrated), holderId_(0) {}

bool Tool::isAvailable() const { return holderId_ == 0; }

void Tool::take(int techId) {
    if (techId <= 0) throw WorkbenchOccupiedException("инструмент " + name_ + " нельзя выдать без мастера");
    if (!isAvailable())
        throw WorkbenchOccupiedException("инструмент " + name_ + " (у мастера #" + std::to_string(holderId_) + ")");
    holderId_ = techId;
}

void Tool::giveBack() { holderId_ = 0; }

bool Tool::needsCalibration(const Date& today) const { return calibrated_.daysTo(today) > 180; }

void Tool::calibrate(const Date& today) { calibrated_ = today; }

bool Tool::needsCheck(const Date& today) const { return needsCalibration(today); }
void Tool::certify(const Date& today) { calibrate(today); }

int Tool::daysToCalibration(const Date& today) const { return 180 - calibrated_.daysTo(today); }

const std::string& Tool::getInvNumber() const { return invNumber_; }
const std::string& Tool::getName() const { return name_; }
const Date& Tool::getCalibrated() const { return calibrated_; }
int Tool::getHolderId() const { return holderId_; }

bool Tool::operator==(const Tool& other) const { return invNumber_ == other.invNumber_; }
bool Tool::operator!=(const Tool& other) const { return !(*this == other); }

std::ostream& operator<<(std::ostream& os, const Tool& t) {
    os << t.invNumber_ << " " << t.name_ << ", калибровка " << t.calibrated_;
    if (t.isAvailable()) return os << ", на месте";
    return os << ", у мастера #" << t.holderId_;
}
