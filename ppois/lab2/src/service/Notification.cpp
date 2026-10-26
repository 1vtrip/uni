#include "service/Notification.h"
#include <sstream>

Notification::Notification(int personId, const std::string& text, const std::string& channel, const Date& date)
    : personId_(personId), text_(text), channel_(channel), date_(date) {}

std::string Notification::format() const {
    std::ostringstream out;
    out << "[" << date_ << "] " << channel_ << " -> #" << personId_ << ": " << text_;
    return out.str();
}

bool Notification::isEmail() const { return channel_ == "Email"; }

int Notification::getPersonId() const { return personId_; }
const std::string& Notification::getText() const { return text_; }
const std::string& Notification::getChannel() const { return channel_; }
const Date& Notification::getDate() const { return date_; }

bool Notification::operator==(const Notification& other) const {
    return personId_ == other.personId_ && text_ == other.text_
        && channel_ == other.channel_ && date_ == other.date_;
}

bool Notification::operator!=(const Notification& other) const { return !(*this == other); }

std::ostream& operator<<(std::ostream& os, const Notification& n) { return os << n.format(); }
