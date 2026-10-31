#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "accounts/Account.h"
#include "accounts/PasswordPolicy.h"
#include "accounts/Session.h"

/**
 * @brief Вход, выход и проверка сессий. Учётные записи хранит FileSystem и передаёт сюда параметром;
 * журнал аудита тоже ведёт FileSystem, здесь только проверки и исключения.
 */
class AuthService {
public:
    explicit AuthService(const PasswordPolicy& policy = PasswordPolicy());

    /**
     * @brief Войти и вернуть ID новой сессии. Порядок: блокировка (до пароля, чтобы не крутить счётчик),
     * canLogin (у гостя — срок доступа), пароль. Длина сессии — виртуальный acc.maxSessionMinutes().
     * acc не const: checkPassword ведёт счётчик ошибок и блокирует на 5-й.
     * @throws AuthenticationException заблокирована, вход запрещён или неверный пароль.
     */
    int login(Account& acc, const std::string& password, const Timestamp& now);
    /** @brief Выйти: закрыть сессию; повторный выход ничего не ломает. @throws SessionExpiredException такой сессии нет. */
    void logout(int sessionId);
    /**
     * @brief Проверить сессию, отметить активность и вернуть ID пользователя.
     * @throws SessionExpiredException сессии нет или она истекла (для пользователя это одно и то же).
     */
    int requireSession(int sessionId, const Timestamp& now);
    /** @brief Сколько у пользователя живых сессий. */
    int activeSessionsOf(int userId, const Timestamp& now) const;
    /** @brief Удалить истёкшие сессии; вернуть, сколько удалено. */
    int cleanup(const Timestamp& now);
    /** @brief Сессия по ID. @throws SessionExpiredException такой нет. */
    const Session& session(int sessionId) const;
    int count() const;

    const PasswordPolicy& getPolicy() const;
    const std::vector<Session>& getSessions() const;

    /** @brief "Сессий: 2" и сессии построчно. */
    friend std::ostream& operator<<(std::ostream& os, const AuthService& a);

private:
    /** @brief Индекс сессии, -1 если нет. */
    int indexOf(int sessionId) const;

    std::vector<Session> sessions_;
    PasswordPolicy policy_;
    int nextSessionId_;
};
