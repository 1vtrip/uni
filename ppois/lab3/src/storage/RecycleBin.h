#pragma once
#include <iostream>
#include <vector>
#include "core/DataSize.h"
#include "storage/TrashItem.h"

/**
 * @brief Корзина: удалённые элементы хранятся retentionDays_ дней, общий размер не больше maxSize_.
 * Корзина хранит только записи; сами объекты удаляет FileSystem по ID, которые возвращают add и purgeExpired.
 */
class RecycleBin {
public:
    /** @brief @throws InvalidOperationException retentionDays не положительный. */
    RecycleBin(int retentionDays = 30, const DataSize& maxSize = DataSize::fromGB(1));

    /**
     * @brief Положить запись. Если корзина переполнилась, вытесняются самые старые записи.
     * @return ID вытесненных элементов: FileSystem должен удалить их окончательно и освободить квоту.
     * @throws EntryAlreadyExistsException элемент уже в корзине.
     * @throws InvalidOperationException элемент больше всей корзины (его удаляют сразу, мимо корзины).
     */
    std::vector<int> add(const TrashItem& item);
    /** @brief Убрать запись из корзины и вернуть её. @throws EntryNotFoundException такого элемента нет. */
    TrashItem restore(int entryId);
    /** @brief Удалить записи с истёкшим сроком и вернуть их ID (сами объекты удалит FileSystem). */
    std::vector<int> purgeExpired(const Timestamp& now);
    /** @brief Суммарный размер всех записей. */
    DataSize size() const;
    bool contains(int entryId) const;
    /** @brief Записи, удалённые пользователем userId. */
    std::vector<TrashItem> itemsOf(int userId) const;
    int count() const;
    bool isEmpty() const;

    int getRetentionDays() const;
    const DataSize& getMaxSize() const;
    const std::vector<TrashItem>& getItems() const;

    /** @brief "Корзина: 3 эл., 4.5 MB / 1.0 GB, срок 30 дн." и записи построчно. */
    friend std::ostream& operator<<(std::ostream& os, const RecycleBin& b);

private:
    /** @brief Индекс записи с entryId, -1 если нет. */
    int indexOf(int entryId) const;
    /** @brief Индекс самой старой записи (наименьший deletedAt), -1 если корзина пуста. */
    int oldestIndex() const;

    std::vector<TrashItem> items_;
    int retentionDays_;
    DataSize maxSize_;
};
