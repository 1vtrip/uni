#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "ops/AuditRecord.h"

/** @brief Журнал аудита: последние maxRecords_ действий всех пользователей, старые вытесняются. */
class AuditLog {
public:
    /** @brief @throws InvalidOperationException maxRecords не положительный. */
    explicit AuditLog(int maxRecords = 1000);

    /**
     * @brief Добавить запись; ID берётся из счётчика, а не из размера вектора, поэтому после вытеснения
     * старых записей ID не повторяются. При переполнении удаляется самая старая (первая).
     */
    void record(const Timestamp& t, int userId, const std::string& action, const std::string& target,
                bool success, const std::string& details = "");
    std::vector<AuditRecord> byUser(int userId) const;
    std::vector<AuditRecord> failures() const;
    /** @brief Сколько неудачных входов ("login") было у пользователя, начиная с since. */
    int failedLoginsSince(int userId, const Timestamp& since) const;
    /**
     * @brief ID пользователей, у которых неудачных действий с момента since строго больше threshold.
     * Записи с userId = 0 (неизвестный логин) не учитываются: это не пользователь.
     */
    std::vector<int> suspiciousUsers(int threshold, const Timestamp& since) const;
    int count() const;
    const std::vector<AuditRecord>& getRecords() const;

    /** @brief Все записи построчно. */
    friend std::ostream& operator<<(std::ostream& os, const AuditLog& log);

private:
    std::vector<AuditRecord> records_;
    int maxRecords_;
    int nextId_;
};
