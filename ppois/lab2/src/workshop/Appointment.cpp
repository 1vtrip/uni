#include "workshop/Appointment.h"

Appointment::Appointment(int id, int clientId, const Date& date, int hour, const std::string& purpose)
    : id_(id), clientId_(clientId), date_(date), hour_(hour), purpose_(purpose),
      confirmed_(false), cancelled_(false) {}

bool Appointment::isWorkingHour(int hour) { return hour >= 9 && hour <= 18; }

bool Appointment::isValid() const { return date_.isValid() && isWorkingHour(hour_); }

bool Appointment::confirm() {
    if (cancelled_) return false;
    confirmed_ = true;
    return true;
}

void Appointment::cancel() {
    cancelled_ = true;
    confirmed_ = false;
}

bool Appointment::reschedule(const Date& date, int hour) {
    if (cancelled_) return false;   // TODO(exc): RepairFirmException("Запись отменена")
    if (!isWorkingHour(hour) || !date.isValid())
        return false;   // TODO(exc): ScheduleConflictException(дата и час)
    date_ = date;
    hour_ = hour;
    confirmed_ = false;
    return true;
}

bool Appointment::conflictsWith(const Appointment& other) const {
    return date_ == other.date_ && hour_ == other.hour_ && !cancelled_ && !other.cancelled_;
}

int Appointment::getId() const { return id_; }
int Appointment::getClientId() const { return clientId_; }
const Date& Appointment::getDate() const { return date_; }
int Appointment::getHour() const { return hour_; }
const std::string& Appointment::getPurpose() const { return purpose_; }
bool Appointment::isConfirmed() const { return confirmed_; }
bool Appointment::isCancelled() const { return cancelled_; }

bool Appointment::operator==(const Appointment& other) const { return id_ == other.id_; }
bool Appointment::operator!=(const Appointment& other) const { return !(*this == other); }

std::ostream& operator<<(std::ostream& os, const Appointment& a) {
    os << "Запись №" << a.id_ << ": клиент #" << a.clientId_ << ", " << a.date_ << " в " << a.hour_ << ":00, "
       << a.purpose_;
    if (a.cancelled_) return os << " [отменена]";
    return os << (a.confirmed_ ? " [подтверждена]" : " [не подтверждена]");
}
