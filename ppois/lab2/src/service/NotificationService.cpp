#include "service/NotificationService.h"
#include <sstream>
#include "people/Client.h"
#include "repair/RepairOrder.h"
#include "workshop/Appointment.h"

NotificationService::NotificationService() {}

Notification NotificationService::send(const Person& p, const std::string& text, const Date& date) {
    std::string channel = p.getContacts().isEmailValid() ? "Email" : "SMS";
    Notification n(p.getId(), text, channel, date);
    history_.push_back(n);
    return n;
}

Notification NotificationService::statusChanged(const RepairOrder& o, const Client& c, const Date& today) {
    std::ostringstream text;
    text << c.getName() << ", заказ №" << o.getId() << ": статус «" << o.getStatus() << "», срок " << o.getDeadline();
    return send(c, text.str(), today);
}

Notification NotificationService::readyForPickup(const RepairOrder& o, const Client& c, const Date& today) {
    std::ostringstream text;
    text << c.getName() << ", заказ №" << o.getId() << " готов к выдаче. Ждём вас!";
    return send(c, text.str(), today);
}

Notification NotificationService::remind(const Appointment& a, const Client& c, const Date& today) {
    std::ostringstream text;
    text << c.getName() << ", напоминаем о записи " << a.getDate() << " в " << a.getHour() << ":00 (" << a.getPurpose() << ")";
    return send(c, text.str(), today);
}

const std::vector<Notification>& NotificationService::history() const { return history_; }

std::vector<Notification> NotificationService::historyFor(int personId) const {
    std::vector<Notification> result;
    for (size_t i = 0; i < history_.size(); ++i)
        if (history_[i].getPersonId() == personId) result.push_back(history_[i]);
    return result;
}
