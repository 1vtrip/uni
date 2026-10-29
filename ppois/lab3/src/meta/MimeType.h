#pragma once
#include <iostream>
#include <string>

/** @brief Тип содержимого файла в формате MIME: "text/plain", "image/png". */
class MimeType {
public:
    /** @brief По умолчанию неизвестные двоичные данные: application/octet-stream. */
    MimeType(const std::string& type = "application", const std::string& subtype = "octet-stream");

    /**
     * @brief Тип по расширению файла, без учёта регистра и точки в начале: "PNG", ".png" -> image/png.
     * Неизвестное расширение -> application/octet-stream.
     */
    static MimeType fromExtension(const std::string& ext);

    /** @brief Текст: type_ == "text". */
    bool isText() const;
    /** @brief Изображение: type_ == "image". */
    bool isImage() const;
    /** @brief Архив: subtype_ == "zip". */
    bool isArchive() const;
    /** @brief Исполняемый: subtype_ == "x-executable". */
    bool isExecutable() const;
    /** @brief Строка вида "text/plain". */
    std::string toString() const;

    const std::string& getType() const;
    const std::string& getSubtype() const;

    bool operator==(const MimeType& o) const;
    bool operator!=(const MimeType& o) const;
    friend std::ostream& operator<<(std::ostream& os, const MimeType& m);

private:
    std::string type_;
    std::string subtype_;
};
