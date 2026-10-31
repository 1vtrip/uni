#pragma once
#include <iostream>
#include <vector>
#include "accounts/AccessControlEntry.h"

/**
 * @brief Список правил доступа к одному файлу или папке. Проверяется раньше обычных прав rwx.
 * Запрет всегда важнее разрешения, поэтому результат не зависит от порядка правил.
 */
class AccessControlList {
public:
    explicit AccessControlList(int entryId = 0);

    /** @brief Добавить правило; правило для того же адресата и действия заменяется (см. AccessControlEntry::==). */
    void add(const AccessControlEntry& rule);
    /**
     * @brief Удалить все правила пользователя (isGroup = false) или группы (isGroup = true).
     * Флаг нужен, потому что пользователь #2 и группа #2 — разные адресаты. Возвращает, сколько удалено.
     */
    int removeFor(int principalId, bool isGroup);
    /**
     * @brief Решение по правилам: -1 — есть подходящий запрет, 1 — есть разрешение и нет запрета,
     * 0 — подходящих правил нет (тогда FileSystem смотрит на обычные права rwx).
     */
    int check(Permission::Access access, int userId, const std::vector<int>& groupIds) const;
    int count() const;
    bool isEmpty() const;

    int getEntryId() const;
    const std::vector<AccessControlEntry>& getRules() const;

    /** @brief Списки равны, если относятся к одному элементу. */
    bool operator==(const AccessControlList& o) const;
    bool operator!=(const AccessControlList& o) const;
    /** @brief "ACL #12 (правил: 2):" и правила построчно. */
    friend std::ostream& operator<<(std::ostream& os, const AccessControlList& acl);

private:
    int entryId_;
    std::vector<AccessControlEntry> rules_;
};
