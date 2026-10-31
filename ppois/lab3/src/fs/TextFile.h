#pragma once
#include <string>
#include "fs/File.h"

/** @brief Текстовый файл: содержимое хранится строкой, размер — её длина в байтах. */
class TextFile : public File {
public:
    TextFile(int id, const std::string& name, int parentId, int ownerId,
             const Permission& permission, const Timestamp& now,
             const std::string& content = "");

    std::string typeName() const override;
    /** @brief Длина текста в байтах. */
    DataSize size() const override;
    /** @brief Первые maxChars символов, дальше "...". */
    std::string preview(int maxChars) const override;

    /** @brief Заменить содержимое целиком, пересчитать контрольную сумму и время изменения. */
    void write(const std::string& text, const Timestamp& now);
    /** @brief Дописать текст в конец. */
    void append(const std::string& text, const Timestamp& now);
    /** @brief Число строк: пустой файл — 0, иначе число '\n' + 1. */
    int lineCount() const;
    /** @brief Число слов, разделённых пробелами, переводами строк и табуляцией. */
    int wordCount() const;
    /** @brief Сколько раз встречается what; вхождения не перекрываются ("aa" в "aaaa" — 2). */
    int countOccurrences(const std::string& what) const;
    /** @brief Заменить все вхождения from на to; возвращает число замен. */
    int replaceAll(const std::string& from, const std::string& to, const Timestamp& now);
    const std::string& getContent() const;

private:
    std::string content_;
};
