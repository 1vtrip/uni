#pragma once
#include <iostream>
#include <string>

/**
 * @brief Права доступа в стиле Unix: 9 битов rwx для владельца, группы и остальных.
 * Хранятся в одном int, как в chmod: 0750 = rwxr-x---.
 */
class Permission {
public:
    /** @brief Виды доступа. TODO(exc): заменить на enum class Access. */
    static const int READ = 0, WRITE = 1, EXECUTE = 2;
    /** @brief Кому даётся доступ. TODO(exc): заменить на enum class Scope. */
    static const int OWNER = 0, GROUP = 1, OTHERS = 2;

    explicit Permission(int mode = 0644);
    /**
     * @brief Разобрать строку вида "rwxr-x---".
     * @return false, если строка не из 9 символов или символ не на своём месте; result тогда не меняется.
     * TODO(exc): FileSystemException
     */
    static bool fromString(const std::string& rwx, Permission& result);
    /** @brief Обычный файл: rw-r--r-- (0644). */
    static Permission forFile();
    /** @brief Папка: rwxr-xr-x (0755). */
    static Permission forDirectory();
    /** @brief Личное: rw------- (0600). */
    static Permission forPrivate();

    /** @brief Есть ли у scope право access. */
    bool has(int scope, int access) const;
    /** @brief Можно ли действие пользователю: проверяется только одна тройка (владелец, иначе группа, иначе остальные). */
    bool allows(int access, bool isOwner, bool inGroup) const;
    /** @brief Выдать право. */
    void grant(int scope, int access);
    /** @brief Отозвать право. */
    void revoke(int scope, int access);
    int toOctal() const;
    /** @brief Строка вида "rw-r--r--". */
    std::string toString() const;

    bool operator==(const Permission& o) const;
    bool operator!=(const Permission& o) const;
    friend std::ostream& operator<<(std::ostream& os, const Permission& p);

private:
    /** @brief Маска одного бита: r=4, w=2, x=1, сдвинутые на 6 (владелец), 3 (группа) или 0 (остальные). */
    static int bit(int scope, int access);

    int mode_;
};
