#pragma once
#include <iostream>
#include <string>
#include "core/Timestamp.h"
#include "meta/Permission.h"

/**
 * @brief Ссылка для общего доступа к файлу: токен, доступ (чтение или запись), срок и лимит переходов.
 * maxUses_ = 0 — переходов без ограничения.
 */
class ShareLink {
public:
    /**
     * @brief Ссылка на days дней от createdAt.
     * @throws InvalidOperationException пустой токен, доступ "выполнение", days не положительный
     * или maxUses отрицательный.
     */
    ShareLink(const std::string& token, int entryId, int createdBy, Permission::Access access,
              const Timestamp& createdAt, int days, int maxUses);

    /** @brief Не действует: срок прошёл, ссылку отозвали или переходы закончились. */
    bool isExpired(const Timestamp& now) const;
    /**
     * @brief Перейти по ссылке: засчитать переход и вернуть ID файла.
     * @throws ShareLinkExpiredException ссылка не действует (в сообщении — почему).
     */
    int use(const Timestamp& now);
    void revoke();
    /** @brief Сколько переходов осталось; -1 — без ограничения. */
    int remainingUses() const;
    bool allowsWrite() const;

    const std::string& getToken() const;
    int getEntryId() const;
    int getCreatedBy() const;
    Permission::Access getAccess() const;
    const Timestamp& getCreatedAt() const;
    const Timestamp& getExpiresAt() const;
    int getMaxUses() const;
    int getUses() const;
    bool isRevoked() const;

    /** @brief Ссылки равны, если совпадают токены. */
    bool operator==(const ShareLink& o) const;
    bool operator!=(const ShareLink& o) const;
    /** @brief "3fa9c1 → #12, чтение, до 05.11.2026 10:15, переходов 1/2". */
    friend std::ostream& operator<<(std::ostream& os, const ShareLink& l);

private:
    /** @brief Почему ссылка не действует: "отозвана", "срок истёк", "переходы закончились"; пусто, если действует. */
    std::string expiryReason(const Timestamp& now) const;

    std::string token_;
    int entryId_;
    int createdBy_;
    Permission::Access access_;
    Timestamp createdAt_;
    Timestamp expiresAt_;
    int maxUses_;
    int uses_;
    bool revoked_;
};
