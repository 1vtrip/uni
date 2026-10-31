#pragma once
#include <iostream>
#include <vector>
#include "ops/FileLock.h"

/** @brief Все блокировки всех файлов: выдаёт, снимает и проверяет, можно ли писать. */
class LockManager {
public:
    LockManager();

    /**
     * @brief Взять блокировку. Сначала удаляются истёкшие, потом проверяются все блокировки файла.
     * Если у пользователя уже есть блокировка этого файла, она заменяется (так Shared повышается до Exclusive).
     * @throws FileLockedException есть чужая конфликтующая блокировка (в сообщении "#entryId" и ID владельца).
     * @throws InvalidOperationException minutes не положительный.
     */
    void acquire(int entryId, int userId, FileLock::LockMode mode, const Timestamp& now, int minutes);
    /** @brief Снять свою блокировку файла; false, если её не было. */
    bool release(int entryId, int userId);
    /** @brief Снять все блокировки файла (файл удалён); вернуть, сколько снято. */
    int releaseAllFor(int entryId);
    /** @brief Есть ли у файла живая блокировка. Истёкшие пропускаются: const-метод не может их удалить. */
    bool isLocked(int entryId, const Timestamp& now) const;
    /** @brief Можно ли писать: нет живых чужих блокировок, даже Shared (кто-то читает). Своя не мешает. */
    bool canWrite(int entryId, int userId, const Timestamp& now) const;
    /** @brief Удалить истёкшие блокировки; вернуть, сколько удалено. */
    int cleanupExpired(const Timestamp& now);
    /** @brief Блокировки пользователя (копии). */
    std::vector<FileLock> locksOf(int userId) const;
    int count() const;
    const std::vector<FileLock>& getLocks() const;

    /** @brief "Блокировок: 2" и блокировки построчно. */
    friend std::ostream& operator<<(std::ostream& os, const LockManager& m);

private:
    std::vector<FileLock> locks_;
};
