#include "ops/ShareLink.h"
#include "exceptions/Exceptions.h"

ShareLink::ShareLink(const std::string& token, int entryId, int createdBy, Permission::Access access,
                     const Timestamp& createdAt, int days, int maxUses)
    : token_(token), entryId_(entryId), createdBy_(createdBy), access_(access), createdAt_(createdAt),
      expiresAt_(createdAt.addDays(days)), maxUses_(maxUses), uses_(0), revoked_(false) {
    if (token.empty()) throw InvalidOperationException("токен ссылки не может быть пустым");
    if (access == Permission::Access::Execute)
        throw InvalidOperationException("по ссылке можно дать только чтение или запись");
    if (days <= 0) throw InvalidOperationException("срок ссылки должен быть положительным");
    if (maxUses < 0) throw InvalidOperationException("лимит переходов не может быть отрицательным");
}

/** @brief Причины проверяются по порядку: отзыв важнее срока, срок важнее переходов. */
std::string ShareLink::expiryReason(const Timestamp& now) const {
    if (revoked_) return "отозвана";
    if (expiresAt_ <= now) return "срок истёк";
    if (maxUses_ > 0 && uses_ >= maxUses_) return "переходы закончились";
    return "";
}

bool ShareLink::isExpired(const Timestamp& now) const { return !expiryReason(now).empty(); }

int ShareLink::use(const Timestamp& now) {
    std::string reason = expiryReason(now);
    if (!reason.empty()) throw ShareLinkExpiredException(token_ + " (" + reason + ")");
    ++uses_;
    return entryId_;
}

void ShareLink::revoke() { revoked_ = true; }

int ShareLink::remainingUses() const {
    if (maxUses_ == 0) return -1;
    return uses_ >= maxUses_ ? 0 : maxUses_ - uses_;
}

bool ShareLink::allowsWrite() const { return access_ == Permission::Access::Write; }

const std::string& ShareLink::getToken() const { return token_; }
int ShareLink::getEntryId() const { return entryId_; }
int ShareLink::getCreatedBy() const { return createdBy_; }
Permission::Access ShareLink::getAccess() const { return access_; }
const Timestamp& ShareLink::getCreatedAt() const { return createdAt_; }
const Timestamp& ShareLink::getExpiresAt() const { return expiresAt_; }
int ShareLink::getMaxUses() const { return maxUses_; }
int ShareLink::getUses() const { return uses_; }
bool ShareLink::isRevoked() const { return revoked_; }

bool ShareLink::operator==(const ShareLink& o) const { return token_ == o.token_; }
bool ShareLink::operator!=(const ShareLink& o) const { return !(*this == o); }

std::ostream& operator<<(std::ostream& os, const ShareLink& l) {
    os << l.token_ << " → #" << l.entryId_ << ", " << (l.allowsWrite() ? "запись" : "чтение")
       << ", до " << l.expiresAt_ << ", переходов " << l.uses_ << '/';
    if (l.maxUses_ == 0) os << "∞";
    else os << l.maxUses_;
    if (l.revoked_) os << " [ОТОЗВАНА]";
    return os;
}
