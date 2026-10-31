#include "ops/ShareManager.h"
#include "core/Checksum.h"
#include "exceptions/Exceptions.h"

ShareManager::ShareManager() : links_(), counter_(0) {}

int ShareManager::indexOf(const std::string& token) const {
    for (size_t i = 0; i < links_.size(); ++i)
        if (links_[i].getToken() == token) return (int)i;
    return -1;
}

/**
 * @brief Номер стоит в начале строки, а за ним ID файла, автора и время: в djb2 последний символ меняет хеш
 * только на свою разницу, и короткая строка с номером в конце давала бы токены подряд (...ca73, ...ca74).
 * djb2 — простой хеш, у разных строк результат может совпасть; тогда find находил бы чужую ссылку,
 * поэтому номер увеличивается, пока токен не станет новым.
 */
std::string ShareManager::create(int entryId, int userId, Permission::Access a, const Timestamp& now,
                                 int days, int maxUses) {
    std::string token;
    do {
        token = Checksum::of(std::to_string(++counter_) + ":" + std::to_string(entryId) + ":"
                             + std::to_string(userId) + ":" + std::to_string(now.toMinutes())).toHex();
    } while (contains(token));
    links_.push_back(ShareLink(token, entryId, userId, a, now, days, maxUses));
    return token;
}

ShareLink& ShareManager::find(const std::string& token) {
    int i = indexOf(token);
    if (i < 0) throw EntryNotFoundException("ссылка " + token);
    return links_[i];
}

const ShareLink& ShareManager::find(const std::string& token) const {
    int i = indexOf(token);
    if (i < 0) throw EntryNotFoundException("ссылка " + token);
    return links_[i];
}

bool ShareManager::contains(const std::string& token) const { return indexOf(token) >= 0; }

void ShareManager::revoke(const std::string& token, int userId, bool force) {
    ShareLink& link = find(token);
    if (!force && link.getCreatedBy() != userId)
        throw AccessDeniedException("#" + std::to_string(userId), "отозвать чужую ссылку " + token);
    link.revoke();
}

int ShareManager::revokeAllFor(int entryId) {
    int revoked = 0;
    for (size_t i = 0; i < links_.size(); ++i) {
        if (links_[i].getEntryId() == entryId && !links_[i].isRevoked()) {
            links_[i].revoke();
            ++revoked;
        }
    }
    return revoked;
}

std::vector<ShareLink> ShareManager::activeOf(int userId, const Timestamp& now) const {
    std::vector<ShareLink> result;
    for (size_t i = 0; i < links_.size(); ++i)
        if (links_[i].getCreatedBy() == userId && !links_[i].isExpired(now)) result.push_back(links_[i]);
    return result;
}

int ShareManager::count() const { return (int)links_.size(); }
const std::vector<ShareLink>& ShareManager::getLinks() const { return links_; }

std::ostream& operator<<(std::ostream& os, const ShareManager& m) {
    os << "Ссылок: " << m.links_.size();
    for (size_t i = 0; i < m.links_.size(); ++i) os << "\n  " << m.links_[i];
    return os;
}
