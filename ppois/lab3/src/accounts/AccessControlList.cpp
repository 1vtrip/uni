#include "accounts/AccessControlList.h"

AccessControlList::AccessControlList(int entryId) : entryId_(entryId), rules_() {}

/** @brief Без замены в списке могли бы лежать «разрешить #7 чтение» и «запретить #7 чтение» сразу. */
void AccessControlList::add(const AccessControlEntry& rule) {
    for (size_t i = 0; i < rules_.size(); ++i) {
        if (rules_[i] == rule) {
            rules_[i] = rule;
            return;
        }
    }
    rules_.push_back(rule);
}

/** @brief После erase следующее правило сдвигается на место i, поэтому индекс растёт только без удаления. */
int AccessControlList::removeFor(int principalId, bool isGroup) {
    int removed = 0;
    size_t i = 0;
    while (i < rules_.size()) {
        if (rules_[i].getPrincipalId() == principalId && rules_[i].isGroup() == isGroup) {
            rules_.erase(rules_.begin() + i);
            ++removed;
        } else {
            ++i;
        }
    }
    return removed;
}

/**
 * @brief Нельзя вернуть ответ на первом подходящем правиле: если первым стоит разрешение,
 * а дальше запрет, запрет бы потерялся. Поэтому запрет возвращается сразу,
 * а разрешение только запоминается, и поиск запрета продолжается.
 */
int AccessControlList::check(Permission::Access access, int userId, const std::vector<int>& groupIds) const {
    bool allowed = false;
    for (size_t i = 0; i < rules_.size(); ++i) {
        if (!rules_[i].matches(access, userId, groupIds)) continue;
        if (!rules_[i].isAllow()) return -1;
        allowed = true;
    }
    return allowed ? 1 : 0;
}

int AccessControlList::count() const { return (int)rules_.size(); }
bool AccessControlList::isEmpty() const { return rules_.empty(); }
int AccessControlList::getEntryId() const { return entryId_; }
const std::vector<AccessControlEntry>& AccessControlList::getRules() const { return rules_; }

bool AccessControlList::operator==(const AccessControlList& o) const { return entryId_ == o.entryId_; }
bool AccessControlList::operator!=(const AccessControlList& o) const { return !(*this == o); }

std::ostream& operator<<(std::ostream& os, const AccessControlList& acl) {
    os << "ACL #" << acl.entryId_ << " (правил: " << acl.rules_.size() << "):";
    for (size_t i = 0; i < acl.rules_.size(); ++i) os << "\n  " << acl.rules_[i];
    return os;
}
