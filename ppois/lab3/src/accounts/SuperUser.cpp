#include "accounts/SuperUser.h"

/** @brief Вызовы Account(...) внутри конструкторов UserAccount и AdminAccount здесь пропускаются. */
SuperUser::SuperUser(int id, const std::string& login, const std::string& displayName,
                     const Credentials& credentials, const Timestamp& created, int homeDirId)
    : Account(id, login, displayName, credentials, created),
      UserAccount(id, login, displayName, credentials, created, homeDirId),
      AdminAccount(id, login, displayName, credentials, created, MAX_LEVEL) {}

std::string SuperUser::role() const { return "Суперпользователь"; }

void SuperUser::print(std::ostream& os) const {
    Account::print(os);   // логин и ID один раз
    printUserPart(os);
    printAdminPart(os);
}

int SuperUser::maxSessionMinutes() const { return 480; }
bool SuperUser::canReadAnyFile() const { return true; }
DataSize SuperUser::defaultQuota() const { return DataSize::fromGB(100); }
