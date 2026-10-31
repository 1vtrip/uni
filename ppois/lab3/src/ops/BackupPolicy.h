#pragma once
#include <iostream>
#include "core/DataSize.h"
#include "core/Timestamp.h"

class FsEntry;   // нужен только как const& в shouldInclude, include — в .cpp

/** @brief Правила резервного копирования: как часто, сколько копий хранить, брать ли скрытые, предел размера. */
class BackupPolicy {
public:
    /** @brief @throws InvalidOperationException интервал, число копий или предел размера не положительные. */
    BackupPolicy(int intervalDays = 1, int keepCount = 5, bool includeHidden = false,
                 const DataSize& maxSize = DataSize::fromGB(5));

    /** @brief Пора ли делать копию: с прошлой прошло не меньше intervalDays_ полных суток. */
    bool isDue(const Timestamp& lastBackup, const Timestamp& now) const;
    /** @brief Когда нужна следующая копия: lastBackup + intervalDays_ дней. */
    Timestamp nextDue(const Timestamp& lastBackup) const;
    /**
     * @brief Включать ли элемент: только файлы (typeChar() == '-', папки и ссылки нет), скрытые — по настройке.
     * Ссылка без своей цели в копии была бы битой.
     */
    bool shouldInclude(const FsEntry& e) const;

    int getIntervalDays() const;
    int getKeepCount() const;
    bool includesHidden() const;
    const DataSize& getMaxSize() const;

    /** @brief "каждые 1 дн., хранить 5, до 5.0 GB, без скрытых". */
    friend std::ostream& operator<<(std::ostream& os, const BackupPolicy& p);

private:
    int intervalDays_;
    int keepCount_;
    bool includeHidden_;
    DataSize maxSize_;
};
