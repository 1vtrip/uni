#pragma once
#include <iostream>
#include <string>
#include "core/DataSize.h"

/** @brief Запись о файле внутри архива: имя, размер до и после сжатия. */
class ArchiveEntry {
public:
    ArchiveEntry(const std::string& name = "", const DataSize& originalSize = DataSize(),
                 const DataSize& compressedSize = DataSize());

    /** @brief Степень сжатия: compressed / original (0, если original нулевой; больше 1, если файл вырос). */
    double ratio() const;
    /** @brief Сколько места сэкономлено: original - compressed, не меньше 0. */
    DataSize saved() const;
    /** @brief Экономия в процентах: (1 - ratio) * 100, не меньше 0. */
    double savedPercent() const;
    /** @brief Стал ли файл меньше после сжатия. */
    bool isCompressed() const;

    const std::string& getName() const;
    const DataSize& getOriginalSize() const;
    const DataSize& getCompressedSize() const;

    /** @brief Записи равны, если совпадают имена: в одном архиве имена не повторяются. */
    bool operator==(const ArchiveEntry& o) const;
    bool operator!=(const ArchiveEntry& o) const;
    /** @brief Вывод вида "notes.txt 1.2 KB -> 400 B (сжато на 67%)". */
    friend std::ostream& operator<<(std::ostream& os, const ArchiveEntry& e);

private:
    std::string name_;
    DataSize originalSize_;
    DataSize compressedSize_;
};
