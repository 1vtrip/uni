#pragma once
#include <iostream>
#include <vector>
#include "ops/Backup.h"
#include "ops/BackupPolicy.h"

/** @brief Резервное копирование: запускает копии по политике, хранит историю и удаляет лишние. */
class BackupManager {
public:
    explicit BackupManager(const BackupPolicy& policy = BackupPolicy());

    /**
     * @brief Пора ли делать копию: готовых копий нет или с последней готовой прошло больше интервала.
     * Считается от готовой копии: неудачная попытка не должна сдвигать срок.
     */
    bool isDue(const Timestamp& now) const;
    /**
     * @brief Сделать копию файлов ids с размерами sizes (параллельные векторы) и вернуть её ID.
     * Если сумма больше предела политики, копия сохраняется в истории как Failed, и только потом бросается исключение.
     * @throws InvalidOperationException длины ids и sizes различаются.
     * @throws BackupFailedException сумма размеров больше policy.getMaxSize().
     */
    int runBackup(const std::vector<int>& ids, const std::vector<DataSize>& sizes, const Timestamp& now);
    /**
     * @brief Удалить лишнее: готовые копии сверх keepCount (самые старые) и неудачные, после которых
     * уже была готовая. Вернуть, сколько удалено.
     */
    int cleanupOld();
    /** @brief Последняя готовая копия. @throws BackupFailedException готовых копий нет. */
    const Backup& latest() const;
    /** @brief Копия по ID. @throws EntryNotFoundException такой нет. */
    const Backup& get(int id) const;
    /**
     * @brief Копия для восстановления.
     * @throws EntryNotFoundException такой нет. @throws BackupFailedException копия не готова (провалена).
     */
    const Backup& forRestore(int id) const;
    /** @brief Сколько места занимают готовые копии (неудачные данных не хранят). */
    DataSize storageUsed() const;
    int count() const;

    const BackupPolicy& getPolicy() const;
    void setPolicy(const BackupPolicy& policy);
    const std::vector<Backup>& getBackups() const;

    /** @brief Политика и копии построчно. */
    friend std::ostream& operator<<(std::ostream& os, const BackupManager& m);

private:
    /** @brief Индекс последней готовой копии, -1 если таких нет. */
    int latestUsableIndex() const;

    BackupPolicy policy_;
    std::vector<Backup> backups_;
    int nextId_;
};
