#pragma once
#include <iostream>
#include <string>
#include <vector>

/**
 * @brief Путь в файловой системе, разобранный на части.
 * "/home/ivan/a.txt" хранится как {"home", "ivan", "a.txt"} и absolute_ = true.
 * Корень "/" — абсолютный путь без частей.
 */
class Path {
public:
    /** @brief Корневой путь "/". */
    Path();

    /**
     * @brief Разобрать и нормализовать путь: пустые части и "." пропускаются, ".." убирает предыдущую часть.
     * @throws InvalidPathException строка пустая, ".." выходит выше начала пути или имя недопустимо.
     */
    static Path parse(const std::string& text);
    /** @brief Допустимое имя: не пустое, не длиннее 255, без '/', не "." и не "..". */
    static bool isValidName(const std::string& name);

    /** @brief Строка пути: "/home/ivan/a.txt", у корня "/", у пустого относительного ".". */
    std::string toString() const;
    /** @brief Путь без последней части; у корня родитель — сам корень. */
    Path parent() const;
    /** @brief Последняя часть пути; у корня пустая строка. */
    std::string fileName() const;
    /** @brief Расширение после последней точки в нижнем регистре: "a.TXT" -> "txt", ".bashrc" -> "". */
    std::string extension() const;
    /** @brief Путь с добавленным именем в конце. Имя проверяет вызывающий через isValidName. */
    Path join(const std::string& name) const;
    /** @brief Начинается ли путь с prefix по частям: "/home/ivan" начинается с "/home", но не с "/home/iv". */
    bool startsWith(const Path& prefix) const;

    bool isRoot() const;
    bool isAbsolute() const;
    int depth() const;
    const std::vector<std::string>& parts() const;

    bool operator==(const Path& o) const;
    bool operator!=(const Path& o) const;
    friend std::ostream& operator<<(std::ostream& os, const Path& p);

private:
    std::vector<std::string> parts_;
    bool absolute_;
};
