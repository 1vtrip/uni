#pragma once
#include <string>
#include <vector>
#include "accounts/Account.h"

/** @brief Администратор: уровень 1–3 и список привилегий. Читать любые файлы может с уровня 2. */
class AdminAccount : public Account {
public:
    static const int MIN_LEVEL = 1;
    static const int MAX_LEVEL = 3;

    /** @brief @throws InvalidOperationException уровень вне 1–3. */
    AdminAccount(int id, const std::string& login, const std::string& displayName,
                 const Credentials& credentials, const Timestamp& created, int adminLevel = 1);

    std::string role() const override;
    /** @brief Строка Account::print и printAdminPart. */
    void print(std::ostream& os) const override;
    /** @brief 240 минут. */
    int maxSessionMinutes() const override;
    /** @brief Администратор управляет пользователями. */
    bool canManageUsers() const override;
    /** @brief Читать любой файл можно только с уровня 2: права зависят от поля, а не только от типа. */
    bool canReadAnyFile() const override;

    bool hasPrivilege(const std::string& p) const;
    /** @brief Выдать привилегию; повтор ничего не меняет. @throws InvalidOperationException пустое имя. */
    void grantPrivilege(const std::string& p);
    /** @brief Отозвать привилегию; false, если её не было. */
    bool revokePrivilege(const std::string& p);
    /** @brief Повысить уровень на 1; false, если уже максимальный. */
    bool promote();
    /** @brief Понизить уровень на 1; false, если уже минимальный. */
    bool demote();

    int getAdminLevel() const;
    const std::vector<std::string>& getPrivileges() const;

protected:
    /** @brief Только поля администратора: уровень и привилегии. Отдельно, чтобы SuperUser не печатал Account дважды. */
    void printAdminPart(std::ostream& os) const;

    int adminLevel_;
    std::vector<std::string> privileges_;
};
