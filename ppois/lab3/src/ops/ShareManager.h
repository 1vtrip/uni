#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "ops/ShareLink.h"

/** @brief Все ссылки общего доступа: создаёт, находит по токену и отзывает. */
class ShareManager {
public:
    ShareManager();

    /**
     * @brief Создать ссылку и вернуть её токен: Checksum::of(entryId + ":" + номер).toHex().
     * Если такой токен уже есть (совпадение хеша), берётся следующий номер.
     * @throws InvalidOperationException неверные параметры ссылки (см. конструктор ShareLink).
     */
    std::string create(int entryId, int userId, Permission::Access a, const Timestamp& now, int days, int maxUses);
    /**
     * @brief Ссылка по токену. Возвращается ссылка на элемент вектора: после следующего create (push_back)
     * она может стать недействительной, поэтому её используют сразу, а не сохраняют.
     * @throws EntryNotFoundException такого токена нет.
     */
    ShareLink& find(const std::string& token);
    const ShareLink& find(const std::string& token) const;
    bool contains(const std::string& token) const;
    /**
     * @brief Отозвать ссылку. Отзывает создатель; force = true — без проверки (FileSystem передаёт его для
     * администратора: менеджер знает только ID и не решает, кто админ).
     * @throws EntryNotFoundException такого токена нет. @throws AccessDeniedException не создатель и не force.
     */
    void revoke(const std::string& token, int userId, bool force = false);
    /** @brief Отозвать все ссылки на файл (файл удалён); вернуть, сколько было не отозвано. Ссылки остаются для истории. */
    int revokeAllFor(int entryId);
    /** @brief Действующие ссылки, созданные пользователем. */
    std::vector<ShareLink> activeOf(int userId, const Timestamp& now) const;
    int count() const;
    const std::vector<ShareLink>& getLinks() const;

    /** @brief "Ссылок: 3" и ссылки построчно. */
    friend std::ostream& operator<<(std::ostream& os, const ShareManager& m);

private:
    /** @brief Индекс ссылки с токеном, -1 если нет. */
    int indexOf(const std::string& token) const;

    std::vector<ShareLink> links_;
    int counter_;
};
