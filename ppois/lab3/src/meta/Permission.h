#pragma once
#include <iostream>
#include <string>

/**
 * @brief Права доступа в стиле Unix: 9 битов rwx для владельца, группы и остальных.
 * Хранятся в одном int, как в chmod: 0750 = rwxr-x---.
 */
class Permission {
public:
    /** @brief Вид доступа: чтение, запись, выполнение. */
    enum class Access { Read, Write, Execute };
    /** @brief Кому даётся доступ: владельцу, группе, остальным. */
    enum class Scope { Owner, Group, Others };

    explicit Permission(int mode = 0644);
    /**
     * @brief Разобрать строку вида "rwxr-x---".
     * @throws FileSystemException строка не из 9 символов или символ не на своём месте.
     */
    static Permission fromString(const std::string& rwx);
    /** @brief Обычный файл: rw-r--r-- (0644). */
    static Permission forFile();
    /** @brief Папка: rwxr-xr-x (0755). */
    static Permission forDirectory();
    /** @brief Личное: rw------- (0600). */
    static Permission forPrivate();

    /** @brief Есть ли у scope право access. */
    bool has(Scope scope, Access access) const;
    /** @brief Можно ли действие пользователю: проверяется только одна тройка (владелец, иначе группа, иначе остальные). */
    bool allows(Access access, bool isOwner, bool inGroup) const;
    /** @brief Выдать право. */
    void grant(Scope scope, Access access);
    /** @brief Отозвать право. */
    void revoke(Scope scope, Access access);
    int toOctal() const;
    /** @brief Строка вида "rw-r--r--". */
    std::string toString() const;

    bool operator==(const Permission& o) const;
    bool operator!=(const Permission& o) const;
    friend std::ostream& operator<<(std::ostream& os, const Permission& p);

private:
    /** @brief Маска одного бита: r=4, w=2, x=1, сдвинутые на 6 (владелец), 3 (группа) или 0 (остальные). */
    static int bit(Scope scope, Access access);

    int mode_;
};
