#pragma once
#include <iostream>
#include <string>
#include <vector>

/** @brief Группа пользователей (например, students). Участники хранятся по ID, без повторов. */
class UserGroup {
public:
    /** @brief @throws InvalidOperationException имя не проходит те же правила, что у метки (1–30 букв, без пробелов). */
    UserGroup(int id, const std::string& name);

    /** @brief Добавить участника; false, если он уже в группе (повтор — не ошибка). */
    bool addMember(int userId);
    /** @brief Убрать участника; false, если его нет. */
    bool removeMember(int userId);
    bool contains(int userId) const;
    int size() const;
    bool isEmpty() const;
    /** @brief Переименовать. @throws InvalidOperationException недопустимое имя. */
    void rename(const std::string& newName);

    int getId() const;
    const std::string& getName() const;
    const std::vector<int>& getMemberIds() const;

    /** @brief Группы равны, если совпадают ID. */
    bool operator==(const UserGroup& o) const;
    bool operator!=(const UserGroup& o) const;
    /** @brief "students (#2): 3 участника". */
    friend std::ostream& operator<<(std::ostream& os, const UserGroup& g);

private:
    /** @brief @throws InvalidOperationException имя недопустимо. */
    static void checkName(const std::string& name);
    /** @brief Слово «участник» в нужной форме для числа n. */
    static std::string membersWord(int n);

    int id_;
    std::string name_;
    std::vector<int> memberIds_;
};
