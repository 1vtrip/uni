#pragma once
#include "accounts/AdminAccount.h"
#include "accounts/UserAccount.h"

/**
 * @brief Администратор с личной домашней папкой и квотой. Ромб: UserAccount и AdminAccount наследуют
 * Account виртуально, поэтому Account в SuperUser один — один ID и один логин.
 */
class SuperUser final : public UserAccount, public AdminAccount {
public:
    /** @brief Account(...) вызывается первым: виртуальную базу создаёт самый производный класс. Уровень всегда 3. */
    SuperUser(int id, const std::string& login, const std::string& displayName,
              const Credentials& credentials, const Timestamp& created, int homeDirId);

    /** @brief Обязателен: role() переопределён и в UserAccount, и в AdminAccount — без него «no unique final overrider». */
    std::string role() const override;
    /** @brief Обязателен по той же причине. Account печатается один раз, потом части пользователя и администратора. */
    void print(std::ostream& os) const override;
    /** @brief 480 минут. Переопределён только в AdminAccount, конфликта нет — здесь ради другого значения. */
    int maxSessionMinutes() const override;
    /** @brief Читает любые файлы независимо от уровня. */
    bool canReadAnyFile() const override;
    /** @brief 100 GB. */
    DataSize defaultQuota() const override;
};
