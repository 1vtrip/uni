#pragma once
#include <iostream>
#include <vector>
#include "meta/Permission.h"

/**
 * @brief Одно правило ACL: кому (пользователю или группе), какое действие, разрешить или запретить.
 * Флаг isGroup_ нужен, потому что ID пользователя и группы могут совпасть (пользователь #2 и группа #2).
 */
class AccessControlEntry {
public:
    /** @brief @throws InvalidOperationException principalId не положительный. */
    AccessControlEntry(int principalId, bool isGroup, Permission::Access access, bool allow);

    /** @brief Касается ли правило пользователя: для группы — есть ли она в groupIds, иначе совпадение ID. */
    bool appliesTo(int userId, const std::vector<int>& groupIds) const;
    /** @brief Касается ли правило и этого пользователя, и этого действия. */
    bool matches(Permission::Access access, int userId, const std::vector<int>& groupIds) const;
    /** @brief Название действия: "чтение", "запись", "выполнение". */
    static const char* accessName(Permission::Access access);

    int getPrincipalId() const;
    bool isGroup() const;
    Permission::Access getAccess() const;
    bool isAllow() const;

    /**
     * @brief Правила равны, если совпадают «кому» и «какое действие»; разрешение или запрет не важны.
     * Так ACL понимает, что новое правило должно заменить старое, а не лечь рядом.
     */
    bool operator==(const AccessControlEntry& o) const;
    bool operator!=(const AccessControlEntry& o) const;
    /** @brief "запретить группе #2: запись". */
    friend std::ostream& operator<<(std::ostream& os, const AccessControlEntry& e);

private:
    int principalId_;
    bool isGroup_;
    Permission::Access access_;
    bool allow_;
};
