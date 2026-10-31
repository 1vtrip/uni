#pragma once
#include <iostream>
#include "core/Timestamp.h"

/** @brief Сеанс работы пользователя после входа. Истекает, если закрыт или простой больше maxIdleMinutes_. */
class Session {
public:
    /** @brief @throws InvalidOperationException maxIdleMinutes не положительный. */
    Session(int id, int userId, const Timestamp& started, int maxIdleMinutes);

    /**
     * @brief Отметить активность: lastActivity_ = now.
     * @throws SessionExpiredException сессия уже истекла: продлить её нельзя, нужно войти заново.
     */
    void touch(const Timestamp& now);
    /** @brief Закрыть сессию (выход). */
    void close();
    /** @brief Истекла ли: закрыта или простой строго больше maxIdleMinutes_. */
    bool isExpired(const Timestamp& now) const;
    /** @brief Сколько минут осталось до истечения по простою, не меньше 0. */
    long long minutesLeft(const Timestamp& now) const;
    /** @brief Длительность: активная — от начала до now, закрытая — от начала до последней активности. */
    long long durationMinutes(const Timestamp& now) const;

    int getId() const;
    int getUserId() const;
    const Timestamp& getStarted() const;
    const Timestamp& getLastActivity() const;
    int getMaxIdleMinutes() const;
    bool isActive() const;

    /** @brief Сессии равны, если совпадают ID. */
    bool operator==(const Session& o) const;
    bool operator!=(const Session& o) const;
    /** @brief "Сессия #3 пользователя #7 с 29.10.2026 10:15, активна". */
    friend std::ostream& operator<<(std::ostream& os, const Session& s);

private:
    int id_;
    int userId_;
    Timestamp started_;
    Timestamp lastActivity_;
    int maxIdleMinutes_;
    bool active_;
};
