#include "service/AuthService.h"
#include "exceptions/Exceptions.h"

AuthService::AuthService(const PasswordPolicy& policy) : sessions_(), policy_(policy), nextSessionId_(1) {}

int AuthService::indexOf(int sessionId) const {
    for (size_t i = 0; i < sessions_.size(); ++i)
        if (sessions_[i].getId() == sessionId) return (int)i;
    return -1;
}

/**
 * @brief AuthService не знает, гость перед ним или админ: canLogin и maxSessionMinutes виртуальные,
 * поэтому у гостя проверяется срок и сессия 30 минут, у админа — 240.
 */
int AuthService::login(Account& acc, const std::string& password, const Timestamp& now) {
    if (acc.isBlocked()) throw AuthenticationException(acc.getLogin(), "учётная запись заблокирована");
    if (!acc.canLogin(now)) throw AuthenticationException(acc.getLogin(), "вход запрещён: срок доступа истёк");
    if (!acc.checkPassword(password)) {
        if (acc.isBlocked())
            throw AuthenticationException(acc.getLogin(), "неверный пароль, учётная запись заблокирована");
        throw AuthenticationException(acc.getLogin());
    }
    int id = nextSessionId_++;
    sessions_.push_back(Session(id, acc.getId(), now, acc.maxSessionMinutes()));
    return id;
}

void AuthService::logout(int sessionId) {
    int i = indexOf(sessionId);
    if (i < 0) throw SessionExpiredException(sessionId);
    sessions_[i].close();
}

/** @brief Session::touch сам бросает SessionExpiredException у истёкшей сессии. */
int AuthService::requireSession(int sessionId, const Timestamp& now) {
    int i = indexOf(sessionId);
    if (i < 0) throw SessionExpiredException(sessionId);
    sessions_[i].touch(now);
    return sessions_[i].getUserId();
}

int AuthService::activeSessionsOf(int userId, const Timestamp& now) const {
    int count = 0;
    for (size_t i = 0; i < sessions_.size(); ++i)
        if (sessions_[i].getUserId() == userId && !sessions_[i].isExpired(now)) ++count;
    return count;
}

/** @brief После erase индекс не увеличивается, иначе соседняя сессия была бы пропущена. */
int AuthService::cleanup(const Timestamp& now) {
    int removed = 0;
    size_t i = 0;
    while (i < sessions_.size()) {
        if (sessions_[i].isExpired(now)) {
            sessions_.erase(sessions_.begin() + i);
            ++removed;
        } else {
            ++i;
        }
    }
    return removed;
}

const Session& AuthService::session(int sessionId) const {
    int i = indexOf(sessionId);
    if (i < 0) throw SessionExpiredException(sessionId);
    return sessions_[i];
}

int AuthService::count() const { return (int)sessions_.size(); }
const PasswordPolicy& AuthService::getPolicy() const { return policy_; }
const std::vector<Session>& AuthService::getSessions() const { return sessions_; }

std::ostream& operator<<(std::ostream& os, const AuthService& a) {
    os << "Сессий: " << a.sessions_.size();
    for (size_t i = 0; i < a.sessions_.size(); ++i) os << "\n  " << a.sessions_[i];
    return os;
}
