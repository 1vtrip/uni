#pragma once
#include <string>
#include <vector>
#include "fs/File.h"
#include "meta/ArchiveEntry.h"

/** @brief Архив: список записей о сжатых файлах. Размер архива — сумма сжатых размеров. */
class ArchiveFile final : public File {
public:
    ArchiveFile(int id, const std::string& name, int parentId, int ownerId,
                const Permission& permission, const Timestamp& now,
                const std::string& format = "zip");

    std::string typeName() const override;
    /** @brief Сумма compressedSize всех записей. */
    DataSize size() const override;
    /** @brief Имена записей через запятую, не длиннее maxChars (дальше "..."); пустой — "[Архив пуст]". */
    std::string preview(int maxChars) const override;

    /** @brief Добавить запись. @throws EntryAlreadyExistsException запись с таким именем уже есть. */
    void addEntry(const ArchiveEntry& e, const Timestamp& now);
    /** @brief Есть ли запись с таким именем. */
    bool contains(const std::string& name) const;
    /** @brief Сумма исходных размеров всех записей. */
    DataSize originalTotal() const;
    /** @brief Степень сжатия всего архива: size / originalTotal (0, если архив пуст). */
    double compressionRatio() const;
    int entryCount() const;

    const std::vector<ArchiveEntry>& getEntries() const;
    const std::string& getFormat() const;

private:
    std::vector<ArchiveEntry> entries_;
    std::string format_;
};
