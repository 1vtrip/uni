#pragma once
#include <iostream>
#include <string>
#include "core/DataSize.h"
#include "core/Timestamp.h"
#include "meta/Metadata.h"
#include "meta/Permission.h"

/**
 * @brief Абстрактный элемент файловой системы: файл, папка или ссылка.
 * Связь с деревом только через ID: parentId_ — папка, в которой лежит элемент.
 */
class FsEntry {
public:
    /** @brief Имя не проверяется: его проверяет FileSystem перед созданием (у корня имя пустое). */
    FsEntry(int id, const std::string& name, int parentId, int ownerId,
            const Permission& permission, const Timestamp& now);
    virtual ~FsEntry() = default;

    /** @brief Название типа для пользователя: "Папка", "Текстовый файл" и т. д. */
    virtual std::string typeName() const = 0;
    /** @brief Размер самого элемента; у каждого типа считается по-своему. */
    virtual DataSize size() const = 0;
    /** @brief Папка ли это; true только у Directory. */
    virtual bool isDirectory() const;
    /** @brief Символ типа в начале строки print, как в ls -l: '-' файл, 'd' папка, 'l' ссылка. */
    virtual char typeChar() const;
    /** @brief Краткое представление содержимого, у каждого типа своё. По умолчанию "[" + typeName() + "]". */
    virtual std::string preview(int maxChars) const;
    /** @brief Строка в стиле ls -l: "-rw-r--r--  #12     1.5 KB  29.10.2026 10:15  notes.txt". */
    virtual void print(std::ostream& os) const;

    /** @brief Переименовать. @throws InvalidPathException имя не проходит Path::isValidName. */
    void rename(const std::string& newName, const Timestamp& now);
    /** @brief Запомнить новую родительскую папку. Сами списки детей меняет FileSystem. */
    void moveTo(int newParentId, const Timestamp& now);
    /** @brief Отметить изменение: modified_ = now. */
    void touch(const Timestamp& now);
    void chmod(const Permission& p);
    void chown(int newOwnerId);
    /** @brief Разрешено ли действие пользователю по правам rwx (владелец, иначе группа, иначе остальные). */
    bool canAccess(Permission::Access a, int userId, bool inGroup) const;
    /** @brief Скрытый элемент: имя начинается с точки, как ".bashrc". */
    bool isHidden() const;

    int getId() const;
    const std::string& getName() const;
    int getParentId() const;
    int getOwnerId() const;
    const Permission& getPermission() const;
    const Timestamp& getCreated() const;
    const Timestamp& getModified() const;
    Metadata& getMetadata();
    const Metadata& getMetadata() const;

    /** @brief Элементы равны, если совпадают ID. */
    bool operator==(const FsEntry& o) const;
    bool operator!=(const FsEntry& o) const;

protected:
    int id_;
    std::string name_;
    int parentId_;
    int ownerId_;
    Permission permission_;
    Timestamp created_;
    Timestamp modified_;
    Metadata metadata_;
};

/** @brief Вывод через виртуальный print: для любого наследника печатается его версия. */
std::ostream& operator<<(std::ostream& os, const FsEntry& e);
