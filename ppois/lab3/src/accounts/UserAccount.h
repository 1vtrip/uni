#pragma once
#include "accounts/Account.h"
#include "storage/Quota.h"

/** @brief Обычный пользователь с домашней папкой и квотой. */
class UserAccount : public Account {
public:
    /**
     * @brief Квота создаётся с лимитом 1 GB, а не через defaultQuota(): виртуальный вызов в конструкторе
     * попал бы в версию UserAccount даже у GuestAccount. Правильный лимит ставит resetQuota() после создания.
     */
    UserAccount(int id, const std::string& login, const std::string& displayName,
                const Credentials& credentials, const Timestamp& created, int homeDirId);

    std::string role() const override;
    /** @brief Строка Account::print и printUserPart. */
    void print(std::ostream& os) const override;
    /** @brief Лимит квоты для этого типа учётной записи: у пользователя 1 GB. */
    virtual DataSize defaultQuota() const;
    /** @brief Может ли делиться файлами по ссылке; у пользователя да. */
    virtual bool canShare() const;

    int getHomeDirId() const;
    Quota& getQuota();
    const Quota& getQuota() const;
    /**
     * @brief Лимит квоты = defaultQuota(). Вызывается после создания объекта, когда виртуальный вызов
     * уже попадает в версию настоящего типа. @throws InvalidOperationException занято больше нового лимита.
     */
    void resetQuota();

protected:
    /** @brief Только поля пользователя: домашняя папка и квота. Отдельно, чтобы SuperUser не печатал Account дважды. */
    void printUserPart(std::ostream& os) const;

    int homeDirId_;
    Quota quota_;
};
