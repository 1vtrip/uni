#pragma once
#include <iostream>
#include <string>

/** @brief Метка файла или папки: имя и цвет, например #учёба(blue). */
class Tag {
public:
    /**
     * @brief Создать метку.
     * @throws InvalidOperationException имя не проходит isValidName или цвет не проходит isValidColor.
     */
    explicit Tag(const std::string& name = "tag", const std::string& color = "gray");

    /** @brief Допустимое имя: от 1 до 30 букв (не байтов), без пробельных символов. */
    static bool isValidName(const std::string& n);
    /** @brief Допустимый цвет: red, orange, yellow, green, blue, purple, gray. */
    static bool isValidColor(const std::string& c);

    /** @brief Совпадает ли имя с query без учёта регистра (полное совпадение, не подстрока). */
    bool matches(const std::string& query) const;
    /** @brief Сменить цвет. @throws InvalidOperationException цвет недопустим (цвет тогда не меняется). */
    void setColor(const std::string& color);

    const std::string& getName() const;
    const std::string& getColor() const;

    /** @brief Метки равны, если имена совпадают без учёта регистра; цвет не важен. */
    bool operator==(const Tag& o) const;
    bool operator!=(const Tag& o) const;
    friend std::ostream& operator<<(std::ostream& os, const Tag& t);

private:
    std::string name_;
    std::string color_;
};
