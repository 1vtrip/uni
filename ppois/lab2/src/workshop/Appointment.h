#pragma once
#include <string>
#include <iostream>
#include "core/Date.h"

/** @brief Запись клиента на приём: дата, час и цель визита. Приём с 9:00 до 19:00, последняя запись на 18:00. */
class Appointment {
public:
    Appointment(int id, int clientId, const Date& date, int hour, const std::string& purpose);

    /** @brief Рабочий ли час для записи: от 9 до 18 включительно. */
    static bool isWorkingHour(int hour);
    /** @brief Корректна ли запись: дата существует, час рабочий. */
    bool isValid() const;
    /** @brief Подтвердить запись; false, если она отменена. */
    bool confirm();
    /** @brief Отменить запись. */
    void cancel();
    /**
     * @brief Перенести на другую дату и час; подтверждение сбрасывается.
     * false, если запись отменена или час нерабочий.
     */
    bool reschedule(const Date& date, int hour);   // TODO(exc): RepairFirmException / ScheduleConflictException
    /** @brief Конфликт: те же дата и час, и обе записи не отменены. */
    bool conflictsWith(const Appointment& other) const;

    int getId() const;
    int getClientId() const;
    const Date& getDate() const;
    int getHour() const;
    const std::string& getPurpose() const;
    bool isConfirmed() const;
    bool isCancelled() const;

    /** @brief Записи равны, если совпадает id. */
    bool operator==(const Appointment& other) const;
    bool operator!=(const Appointment& other) const;
    friend std::ostream& operator<<(std::ostream& os, const Appointment& a);

private:
    int id_;
    int clientId_;
    Date date_;
    int hour_;
    std::string purpose_;
    bool confirmed_;
    bool cancelled_;
};
