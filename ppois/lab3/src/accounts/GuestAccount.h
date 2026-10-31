#pragma once
#include "accounts/UserAccount.h"

/** @brief Временный гость: маленькая квота, короткая сессия, нельзя делиться файлами, есть срок действия. */
class GuestAccount final : public UserAccount {
public:
    /** @brief @throws InvalidOperationException срок действия не позже даты создания. */
    GuestAccount(int id, const std::string& login, const std::string& displayName,
                 const Credentials& credentials, const Timestamp& created, int homeDirId,
                 const Timestamp& expiresAt);

    std::string role() const override;
    /** @brief Строка UserAccount::print и срок действия. */
    void print(std::ostream& os) const override;
    /** @brief 30 минут. */
    int maxSessionMinutes() const override;
    /** @brief 50 MB. */
    DataSize defaultQuota() const override;
    /** @brief Гостю нельзя делиться файлами. */
    bool canShare() const override;
    /** @brief Входить можно только до истечения срока. */
    bool canLogin(const Timestamp& now) const override;

    /** @brief Истёк ли срок: now не раньше expiresAt_. */
    bool isExpired(const Timestamp& now) const;
    /**
     * @brief Продлить на days дней: от старого срока, если он ещё не истёк, иначе от now
     * (иначе давно просроченный гость остался бы просроченным).
     * @throws InvalidOperationException days не положительный.
     */
    void extend(int days, const Timestamp& now);

    const Timestamp& getExpiresAt() const;

private:
    Timestamp expiresAt_;
};
