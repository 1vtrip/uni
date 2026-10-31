#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "storage/FileVersion.h"

/**
 * @brief История версий одного файла. Версии лежат по возрастанию номера, самая старая — первая.
 * Хранится не больше maxVersions_ версий: лишние старые удаляет prune().
 */
class VersionHistory {
public:
    /** @brief @throws InvalidOperationException maxVersions не положительный. */
    VersionHistory(int fileId = 0, int maxVersions = 10);

    /**
     * @brief Добавить версию и вернуть её номер (последний + 1).
     * Если содержимое не изменилось (та же контрольная сумма), новая версия не создаётся
     * и возвращается номер последней.
     */
    int addVersion(const std::string& content, int authorId, const Timestamp& now, const std::string& comment);
    /** @brief Версия по номеру. @throws VersionNotFoundException такой нет (не было или удалена prune). */
    const FileVersion& get(int number) const;
    FileVersion& get(int number);
    /** @brief Последняя версия. @throws VersionNotFoundException история пуста. */
    const FileVersion& latest() const;
    /**
     * @brief Содержимое версии number для отката.
     * @throws VersionNotFoundException такой версии нет. @throws CorruptedFileException версия повреждена.
     */
    std::string rollbackTo(int number) const;
    /** @brief Удалить самые старые версии сверх maxVersions_; вернуть, сколько удалено. */
    int prune();
    bool hasVersion(int number) const;
    /** @brief Суммарный размер всех хранимых версий. */
    DataSize totalSize() const;
    int count() const;

    int getFileId() const;
    int getMaxVersions() const;
    const std::vector<FileVersion>& getVersions() const;

    /** @brief Истории равны, если относятся к одному файлу. */
    bool operator==(const VersionHistory& o) const;
    bool operator!=(const VersionHistory& o) const;
    /** @brief Все версии построчно. */
    friend std::ostream& operator<<(std::ostream& os, const VersionHistory& h);

private:
    /** @brief Индекс версии с номером number в versions_, -1 если нет. */
    int indexOf(int number) const;

    int fileId_;
    std::vector<FileVersion> versions_;
    int maxVersions_;
};
