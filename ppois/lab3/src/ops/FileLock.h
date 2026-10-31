#pragma once
#include <iostream>
#include "core/Timestamp.h"

/**
 * @brief Блокировка файла на время работы. Shared — «читаю, другим тоже можно читать»,
 * Exclusive — «пишу, больше никому нельзя». Действует до expiresAt_.
 */
class FileLock {
public:
    enum class LockMode { Shared, Exclusive };

    /** @brief Блокировка на minutes минут от now. @throws InvalidOperationException minutes не положительный. */
    FileLock(int entryId, int userId, LockMode mode, const Timestamp& now, int minutes);

    /**
     * @brief Помешает ли эта блокировка пользователю userId взять блокировку wanted.
     * Свои не мешают; Shared + Shared совместимы; всё, где есть Exclusive, — конфликт.
     * Срок не проверяется: истёкшие блокировки LockManager удаляет до проверки.
     */
    bool conflictsWith(LockMode wanted, int userId) const;
    /** @brief Истёк ли срок: now не раньше expiresAt_. */
    bool isExpired(const Timestamp& now) const;
    bool isHeldBy(int userId) const;
    /**
     * @brief Продлить: expiresAt_ = now + minutes.
     * @throws InvalidOperationException блокировка уже истекла (пока она была просрочена,
     * файл мог взять другой) или minutes не положительный.
     */
    void refresh(const Timestamp& now, int minutes);
    /** @brief "общая" или "монопольная". */
    static const char* modeName(LockMode mode);

    int getEntryId() const;
    int getUserId() const;
    LockMode getMode() const;
    const Timestamp& getAcquiredAt() const;
    const Timestamp& getExpiresAt() const;

    /** @brief Блокировки равны, если это один и тот же файл и пользователь. */
    bool operator==(const FileLock& o) const;
    bool operator!=(const FileLock& o) const;
    /** @brief "#12: монопольная, пользователь #7, до 29.10.2026 10:45". */
    friend std::ostream& operator<<(std::ostream& os, const FileLock& l);

private:
    int entryId_;
    int userId_;
    LockMode mode_;
    Timestamp acquiredAt_;
    Timestamp expiresAt_;
};
