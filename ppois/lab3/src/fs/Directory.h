#pragma once
#include <string>
#include <vector>
#include "fs/FsEntry.h"

/**
 * @brief Папка. Хранит только ID своих элементов: имена и размеры детей она не видит,
 * поэтому проверку имени и полный размер с содержимым делает FileSystem.
 */
class Directory final : public FsEntry {
public:
    /** @brief @throws InvalidOperationException maxChildren не положительный. */
    Directory(int id, const std::string& name, int parentId, int ownerId,
              const Permission& permission, const Timestamp& now, int maxChildren = 1000);

    std::string typeName() const override;
    /** @brief 4 KB — размер самой записи папки, без содержимого (его считает FileSystem::directorySize). */
    DataSize size() const override;
    bool isDirectory() const override;
    char typeChar() const override;
    /** @brief "[Папка, 5 элементов]" с правильным окончанием: 1 элемент, 2 элемента, 11 элементов. */
    std::string preview(int maxChars) const override;

    /**
     * @brief Добавить элемент по ID.
     * @throws EntryAlreadyExistsException такой ID уже есть.
     * @throws InvalidOperationException папка заполнена (maxChildren_).
     */
    void addChild(int id);
    /** @brief Убрать элемент; false, если такого ID нет. */
    bool removeChild(int id);
    bool hasChild(int id) const;
    int childCount() const;
    bool isEmpty() const;
    bool isFull() const;

    const std::vector<int>& getChildIds() const;
    int getMaxChildren() const;

private:
    /** @brief Слово «элемент» в нужной форме для числа n. */
    static std::string itemsWord(int n);

    std::vector<int> childIds_;
    int maxChildren_;
};
