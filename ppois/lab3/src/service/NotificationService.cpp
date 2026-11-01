#include "service/NotificationService.h"
#include "accounts/UserAccount.h"
#include "ops/ShareLink.h"
#include <sstream>

const char* NotificationService::QUOTA_PREFIX = "Квота заполнена";

NotificationService::NotificationService() : history_() {}

void NotificationService::notify(int userId, const std::string& text, const Timestamp& now) {
    history_.push_back(Notification(userId, text, now, "system"));
}

bool NotificationService::hasUnreadQuotaWarning(int userId) const {
    std::string prefix = QUOTA_PREFIX;
    for (size_t i = 0; i < history_.size(); ++i) {
        const Notification& n = history_[i];
        if (n.getUserId() == userId && !n.isRead() && n.getText().compare(0, prefix.size(), prefix) == 0)
            return true;
    }
    return false;
}

bool NotificationService::quotaWarning(const UserAccount& u, const Timestamp& now) {
    const Quota& q = u.getQuota();
    if (!q.isNearLimit() || hasUnreadQuotaWarning(u.getId())) return false;
    std::ostringstream text;
    text << QUOTA_PREFIX << " на " << (int)q.usagePercent() << "%: " << q.getUsed() << " из " << q.getLimit();
    notify(u.getId(), text.str(), now);
    return true;
}

/** @brief Уведомление получает создатель ссылки, а не тот, кто по ней перешёл. */
void NotificationService::shareOpened(const ShareLink& link, const Timestamp& now) {
    std::ostringstream text;
    text << "По ссылке " << link.getToken() << " открыли файл #" << link.getEntryId() << " (переход " << link.getUses();
    if (link.getMaxUses() > 0) text << '/' << link.getMaxUses();
    text << ')';
    notify(link.getCreatedBy(), text.str(), now);
}

void NotificationService::securityAlert(int userId, const std::string& what, const Timestamp& now) {
    history_.push_back(Notification(userId, "Безопасность: " + what, now, "email"));
}

std::vector<Notification> NotificationService::unreadFor(int userId) const {
    std::vector<Notification> result;
    for (size_t i = 0; i < history_.size(); ++i)
        if (history_[i].getUserId() == userId && !history_[i].isRead()) result.push_back(history_[i]);
    return result;
}

std::vector<Notification> NotificationService::allFor(int userId) const {
    std::vector<Notification> result;
    for (size_t i = 0; i < history_.size(); ++i)
        if (history_[i].getUserId() == userId) result.push_back(history_[i]);
    return result;
}

int NotificationService::markAllRead(int userId) {
    int marked = 0;
    for (size_t i = 0; i < history_.size(); ++i) {
        if (history_[i].getUserId() == userId && !history_[i].isRead()) {
            history_[i].markRead();
            ++marked;
        }
    }
    return marked;
}

int NotificationService::count() const { return (int)history_.size(); }
const std::vector<Notification>& NotificationService::getHistory() const { return history_; }

std::ostream& operator<<(std::ostream& os, const NotificationService& s) {
    os << "Уведомлений: " << s.history_.size();
    for (size_t i = 0; i < s.history_.size(); ++i) os << "\n  " << s.history_[i];
    return os;
}
