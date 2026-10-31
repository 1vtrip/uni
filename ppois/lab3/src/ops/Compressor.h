#pragma once
#include <iostream>
#include <string>
#include "core/DataSize.h"
#include "meta/ArchiveEntry.h"
#include "meta/MimeType.h"

class File;   // нужен только как const& в compress, include — в .cpp

/** @brief Модель сжатия: оценивает степень сжатия по типу файла и время работы по уровню 1–9. */
class Compressor {
public:
    static const int MIN_LEVEL = 1;
    static const int MAX_LEVEL = 9;

    /** @brief @throws InvalidOperationException пустое имя алгоритма или уровень вне 1–9. */
    Compressor(const std::string& algorithm = "zip", int level = 5);

    /**
     * @brief Степень сжатия: текст 0,3, изображение 0,95, прочее 0,6, каждый уровень выше 5 умножает на 0,98.
     * Архив всегда 1,0: уже сжатое повторно не сжимается.
     */
    double estimateRatio(const MimeType& m) const;
    /**
     * @brief Запись для архива: имя, исходный и сжатый размер. Работает через const File&, поэтому
     * подходит любой наследник: размер берётся из виртуального size(), тип — из getMime().
     */
    ArchiveEntry compress(const File& f) const;
    /** @brief Сколько секунд займёт сжатие: скорость 100 / level МБ/с (уровень 9 — около 11 МБ/с). */
    double estimateSeconds(const DataSize& s) const;
    /** @brief @throws InvalidOperationException уровень вне 1–9. */
    void setLevel(int level);

    const std::string& getAlgorithm() const;
    int getLevel() const;

    /** @brief "zip, уровень 7". */
    friend std::ostream& operator<<(std::ostream& os, const Compressor& c);

private:
    /** @brief @throws InvalidOperationException уровень вне 1–9. */
    static void checkLevel(int level);

    std::string algorithm_;
    int level_;
};
