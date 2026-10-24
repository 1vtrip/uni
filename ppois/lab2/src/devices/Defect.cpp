#include "devices/Defect.h"

Defect::Defect(const std::string& code, const std::string& description, int severity)
    : code_(code), description_(description), severity_(severity) {}

bool Defect::isCritical() const {
    return severity_ >= 4;
}

double Defect::laborHours() const {
    return severity_ * 0.5;
}

const std::string& Defect::getCode() const { return code_; }
const std::string& Defect::getDescription() const { return description_; }
int Defect::getSeverity() const { return severity_; }

bool Defect::operator==(const Defect& other) const {
    return code_ == other.code_ && description_ == other.description_
        && severity_ == other.severity_;
}

bool Defect::operator!=(const Defect& other) const {
    return !(*this == other);
}

std::ostream& operator<<(std::ostream& os, const Defect& d) {
    return os << "[" << d.code_ << "] " << d.description_ << " (тяжесть " << d.severity_ << ")";
}
