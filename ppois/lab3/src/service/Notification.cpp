#include "service/Notification.h"
#include "exceptions/Exceptions.h"
#include <sstream>

Notification::Notification(int userId, const std::string& text, const Timestamp& time, const std::string& channel)
    : userId_(userId), text_(text), channel_(channel), time_(time), read_(false) {
    if (text.empty()) throw InvalidOperationException("текст уведомления не может быть пустым");
    if (channel != "system" && channel != "email")
        throw InvalidOperationException("канал уведомления должен быть system или email, а не " + channel);
}

/** @brief Строка собирается через ostringstream, потому что у Timestamp есть только operator<<. */
std::string Notification::format() const {
    std::ostringstream out;
    out << (read_ ? "  " : "* ") << '[' << time_ << "] (" << (channel_ == "email" ? "почта" : "система") << ") "
        << text_;
    return out.str();
}

void Notification::markRead() { read_ = true; }
bool Notification::isRead() const { return read_; }

int Notification::getUserId() const { return userId_; }
const std::string& Notification::getText() const { return text_; }
const std::string& Notification::getChannel() const { return channel_; }
const Timestamp& Notification::getTime() const { return time_; }

bool Notification::operator==(const Notification& o) const {
    return userId_ == o.userId_ && time_ == o.time_ && text_ == o.text_;
}
bool Notification::operator!=(const Notification& o) const { return !(*this == o); }

std::ostream& operator<<(std::ostream& os, const Notification& n) { return os << n.format(); }
