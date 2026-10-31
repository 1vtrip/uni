#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "accounts/Credentials.h"
#include "core/Timestamp.h"

class PasswordPolicy;   // предварительное объявление: здесь он нужен только как const&, include — в .cpp

/** @brief Абстрактная учётная запись: логин, пароль (в виде хеша), группы и блокировка. */
class Account {
public:
    static const int MAX_FAILED_LOGINS = 5;

    /**
     * @brief Логин хранится и в login_, и в credentials, поэтому они обязаны совпадать.
     * @throws InvalidOperationException логины разные (иначе пароль проверялся бы с чужой солью).
     */
    Account(int id, const std::string& login, const std::string& displayName,
            const Credentials& credentials, const Timestamp& created);
    virtual ~Account() = default;

    /** @brief Название роли: "Пользователь", "Гость", "Администратор"... */
    virtual std::string role() const = 0;
    /** @brief Сколько минут живёт сессия без активности; по умолчанию 60. */
    virtual int maxSessionMinutes() const;
    /** @brief Может ли управлять пользователями; по умолчанию false. */
    virtual bool canManageUsers() const;
    /** @brief Может ли читать любой файл в обход прав; по умолчанию false. */
    virtual bool canReadAnyFile() const;
    /**
     * @brief Разрешён ли вход в момент now (кроме пароля и блокировки); по умолчанию да.
     * Гость переопределяет: нельзя после срока действия. Так AuthService проверяет любую запись одинаково.
     */
    virtual bool canLogin(const Timestamp& now) const;
    /** @brief "#7 ivan (Иван Петров) — Пользователь", у заблокированной с пометкой. */
    virtual void print(std::ostream& os) const;

    /**
     * @brief Проверка пароля. У заблокированной записи всегда false, даже при верном пароле.
     * Неверный пароль увеличивает счётчик, на MAX_FAILED_LOGINS ошибке подряд запись блокируется;
     * верный пароль сбрасывает счётчик.
     */
    bool checkPassword(const std::string& password);
    /**
     * @brief Сменить пароль. Неверный старый пароль считается неудачной попыткой входа.
     * @throws AuthenticationException запись заблокирована или старый пароль неверный.
     * @throws WeakPasswordException новый пароль не проходит policy.
     */
    void changePassword(const std::string& oldPwd, const std::string& newPwd, const PasswordPolicy& policy);
    /** @brief Вступить в группу; повторное вступление ничего не меняет. */
    void joinGroup(int groupId);
    void leaveGroup(int groupId);
    bool isInGroup(int groupId) const;
    void block();
    /** @brief Разблокировать и сбросить счётчик ошибок, иначе хватило бы одной ошибки для новой блокировки. */
    void unblock();
    bool isBlocked() const;

    int getId() const;
    const std::string& getLogin() const;
    const std::string& getDisplayName() const;
    const Credentials& getCredentials() const;
    const std::vector<int>& getGroupIds() const;
    const Timestamp& getCreated() const;
    int getFailedLogins() const;

    /** @brief Учётные записи равны, если совпадают ID. */
    bool operator==(const Account& o) const;
    bool operator!=(const Account& o) const;

protected:
    int id_;
    std::string login_;
    std::string displayName_;
    Credentials credentials_;
    std::vector<int> groupIds_;
    Timestamp created_;
    bool blocked_;
    int failedLogins_;
};

/** @brief Вывод через виртуальный print: для любого наследника печатается его версия. */
std::ostream& operator<<(std::ostream& os, const Account& a);
