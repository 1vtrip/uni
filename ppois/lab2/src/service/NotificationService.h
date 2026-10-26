#pragma once
#include <string>
#include <vector>
#include "core/Date.h"
#include "service/Notification.h"

/** @brief Forward declaration: в заголовке нужны только как ссылки. */
class Person;
class Client;
class RepairOrder;
class Appointment;

/** @brief Рассылка уведомлений клиентам. Канал выбирается сам: Email, если адрес верный, иначе SMS. */
class NotificationService {
public:
    NotificationService();

    /** @brief Уведомить клиента о новом статусе заказа. */
    Notification statusChanged(const RepairOrder& o, const Client& c, const Date& today);
    /** @brief Уведомить, что устройство готово к выдаче. */
    Notification readyForPickup(const RepairOrder& o, const Client& c, const Date& today);
    /** @brief Напомнить о записи на приём. */
    Notification remind(const Appointment& a, const Client& c, const Date& today);

    /** @brief Все отправленные уведомления. */
    const std::vector<Notification>& history() const;
    /** @brief Уведомления одного человека. */
    std::vector<Notification> historyFor(int personId) const;

private:
    /** @brief Общая часть: выбрать канал по контактам, создать уведомление и сохранить в историю. */
    Notification send(const Person& p, const std::string& text, const Date& date);

    std::vector<Notification> history_;
};
