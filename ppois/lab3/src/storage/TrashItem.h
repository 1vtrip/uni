#pragma once
#include <iostream>
#include <string>
#include "core/DataSize.h"
#include "core/Timestamp.h"

/** @brief Запись в корзине: что удалено, откуда, кем и до какого срока хранится. */
class TrashItem {
public:
    /**
     * @brief Срок хранения: expiresAt_ = deletedAt.addDays(retentionDays).
     * @throws InvalidOperationException retentionDays не положительный.
     */
    TrashItem(int entryId, int originalParentId, const std::string& originalPath, int deletedBy,
              const DataSize& size, const Timestamp& deletedAt, int retentionDays);

    /** @brief Истёк ли срок хранения: now не раньше expiresAt_. */
    bool isExpired(const Timestamp& now) const;
    /** @brief Сколько полных дней осталось, не меньше 0. */
    int daysLeft(const Timestamp& now) const;

    int getEntryId() const;
    int getOriginalParentId() const;
    const std::string& getOriginalPath() const;
    const Timestamp& getDeletedAt() const;
    const Timestamp& getExpiresAt() const;
    int getDeletedBy() const;
    const DataSize& getSize() const;

    /** @brief Записи равны, если относятся к одному элементу. */
    bool operator==(const TrashItem& o) const;
    bool operator!=(const TrashItem& o) const;
    /** @brief "#12 /home/ivan/a.txt  1.5 KB  удалён 29.10.2026 10:15 (#7), хранится до 28.11.2026 10:15". */
    friend std::ostream& operator<<(std::ostream& os, const TrashItem& t);

private:
    int entryId_;
    int originalParentId_;
    std::string originalPath_;
    Timestamp deletedAt_;
    Timestamp expiresAt_;
    int deletedBy_;
    DataSize size_;
};
