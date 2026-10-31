#pragma once
#include <iostream>
#include <string>
#include "core/Timestamp.h"

/** @brief Одна запись журнала аудита: кто, когда, что сделал, с чем и получилось ли. */
class AuditRecord {
public:
    /**
     * @brief userId = 0 — пользователь неизвестен (например, вход под несуществующим логином).
     * @throws InvalidOperationException пустое действие.
     */
    AuditRecord(int id, const Timestamp& time, int userId, const std::string& action,
                const std::string& target, bool success, const std::string& details = "");

    bool isFailure() const;
    /**
     * @brief "[29.10.2026 10:15] #3 delete /home/ivan/a.txt — ОШИБКА: нет прав",
     * у удачного действия "— успешно" (и подробности после двоеточия, если есть).
     */
    std::string toLine() const;

    int getId() const;
    const Timestamp& getTime() const;
    int getUserId() const;
    const std::string& getAction() const;
    const std::string& getTarget() const;
    bool isSuccess() const;
    const std::string& getDetails() const;

    /** @brief Записи равны, если совпадают ID. */
    bool operator==(const AuditRecord& o) const;
    bool operator!=(const AuditRecord& o) const;
    /** @brief Выводит toLine(). */
    friend std::ostream& operator<<(std::ostream& os, const AuditRecord& r);

private:
    int id_;
    Timestamp time_;
    int userId_;
    std::string action_;
    std::string target_;
    bool success_;
    std::string details_;
};
