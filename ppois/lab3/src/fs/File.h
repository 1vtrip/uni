#pragma once
#include <iostream>
#include <string>
#include "core/Checksum.h"
#include "fs/FsEntry.h"
#include "meta/MimeType.h"

/**
 * @brief Общая часть всех файлов: размер, MIME-тип и контрольная сумма.
 * Класс абстрактный, хотя в нём нет ни одного "= 0": он не переопределяет typeName(),
 * и этот метод остаётся чисто виртуальным из FsEntry. Создаются только наследники.
 */
class File : public FsEntry {
public:
    File(int id, const std::string& name, int parentId, int ownerId,
         const Permission& permission, const Timestamp& now,
         const MimeType& mime, const DataSize& size = DataSize(0));

    /** @brief Размер из поля size_; TextFile и ImageFile считают его по-своему. */
    DataSize size() const override;
    /** @brief Строка FsEntry::print и MIME-тип в квадратных скобках. */
    void print(std::ostream& os) const override;

    /** @brief Расширение имени в нижнем регистре: "Photo.PNG" -> "png", без точки -> "". */
    std::string extension() const;
    /** @brief Совпадает ли текущая контрольная сумма с ожидаемой (файл не повреждён). */
    bool verify(const Checksum& expected) const;

    const MimeType& getMime() const;
    const Checksum& getChecksum() const;

protected:
    /** @brief Пересчитать контрольную сумму по новому содержимому. Вызывают только наследники. */
    void updateChecksum(const std::string& data);

    DataSize size_;
    MimeType mime_;
    Checksum checksum_;
};
