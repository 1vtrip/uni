#pragma once
#include <iostream>
#include <string>
#include "core/Checksum.h"
#include "core/DataSize.h"
#include "core/Timestamp.h"

/** @brief Снимок содержимого файла в какой-то момент: номер, автор, время и контрольная сумма. */
class FileVersion {
public:
    /** @brief Контрольная сумма считается по content в момент создания. */
    FileVersion(int number, const std::string& content, int authorId,
                const Timestamp& created, const std::string& comment = "");

    /** @brief Размер снимка в байтах. */
    DataSize size() const;
    /** @brief Цела ли версия: сумма содержимого совпадает с сохранённой. */
    bool isIntact() const;
    /**
     * @brief Испортить содержимое, не трогая контрольную сумму (имитация сбоя диска).
     * Нужно, чтобы показать CorruptedFileException: без порчи версия всегда цела.
     */
    void corrupt(const std::string& garbage);

    int getNumber() const;
    const std::string& getContent() const;
    const Timestamp& getCreated() const;
    int getAuthorId() const;
    const Checksum& getChecksum() const;
    const std::string& getComment() const;

    /** @brief Версии равны, если совпадают номера. */
    bool operator==(const FileVersion& o) const;
    bool operator!=(const FileVersion& o) const;
    /** @brief "v3  29.10.2026 10:15  автор #7  12 B  Исправил опечатку". */
    friend std::ostream& operator<<(std::ostream& os, const FileVersion& v);

private:
    int number_;
    std::string content_;
    Timestamp created_;
    int authorId_;
    Checksum checksum_;
    std::string comment_;
};
