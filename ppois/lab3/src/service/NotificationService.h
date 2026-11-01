#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "service/Notification.h"

class UserAccount;   // нужны только как const& в параметрах, include — в .cpp
class ShareLink;

/** @brief Отправка уведомлений и их история: квота, открытые ссылки, безопасность. */
class NotificationService {
public:
    NotificationService();

    /** @brief Обычное уведомление в канал "system". @throws InvalidOperationException пустой текст. */
    void notify(int userId, const std::string& text, const Timestamp& now);
    /**
     * @brief Предупредить, что квота почти заполнена (только если isNearLimit). Повтор не отправляется,
     * пока у пользователя есть непрочитанное предупреждение о квоте: иначе каждая запись файла добавляла бы новое.
     * @return true, если уведомление отправлено.
     */
    bool quotaWarning(const UserAccount& u, const Timestamp& now);
    /** @brief Сообщить создателю ссылки, что по ней перешли. */
    void shareOpened(const ShareLink& link, const Timestamp& now);
    /** @brief Предупреждение о безопасности в канал "email": важно, даже если человек сейчас не в системе. */
    void securityAlert(int userId, const std::string& what, const Timestamp& now);
    /** @brief Непрочитанные уведомления пользователя (копии). */
    std::vector<Notification> unreadFor(int userId) const;
    /** @brief Все уведомления пользователя (копии). */
    std::vector<Notification> allFor(int userId) const;
    /** @brief Отметить все уведомления пользователя прочитанными; вернуть, сколько было непрочитанных. */
    int markAllRead(int userId);
    int count() const;
    const std::vector<Notification>& getHistory() const;

    /** @brief "Уведомлений: 3" и уведомления построчно. */
    friend std::ostream& operator<<(std::ostream& os, const NotificationService& s);

private:
    /** @brief Начало текста, по которому предупреждение о квоте отличается от остальных. */
    static const char* QUOTA_PREFIX;
    /** @brief Есть ли у пользователя непрочитанное предупреждение о квоте. */
    bool hasUnreadQuotaWarning(int userId) const;

    std::vector<Notification> history_;
};
