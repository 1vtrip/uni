#include "accounts/GuestAccount.h"
#include "exceptions/Exceptions.h"

GuestAccount::GuestAccount(int id, const std::string& login, const std::string& displayName,
                           const Credentials& credentials, const Timestamp& created, int homeDirId,
                           const Timestamp& expiresAt)
    : UserAccount(id, login, displayName, credentials, created, homeDirId), expiresAt_(expiresAt) {
    if (expiresAt <= created) throw InvalidOperationException("срок гостевого доступа должен быть позже даты создания");
}

std::string GuestAccount::role() const { return "Гость"; }

void GuestAccount::print(std::ostream& os) const {
    UserAccount::print(os);
    os << ", доступ до " << expiresAt_;
}

int GuestAccount::maxSessionMinutes() const { return 30; }
DataSize GuestAccount::defaultQuota() const { return DataSize::fromMB(50); }
bool GuestAccount::canShare() const { return false; }
bool GuestAccount::canLogin(const Timestamp& now) const { return !isExpired(now); }

bool GuestAccount::isExpired(const Timestamp& now) const { return expiresAt_ <= now; }

void GuestAccount::extend(int days, const Timestamp& now) {
    if (days <= 0) throw InvalidOperationException("продлить можно только на положительное число дней");
    Timestamp from = isExpired(now) ? now : expiresAt_;
    expiresAt_ = from.addDays(days);
}

const Timestamp& GuestAccount::getExpiresAt() const { return expiresAt_; }
