#include "accounts/Session.h"
#include "exceptions/Exceptions.h"

Session::Session(int id, int userId, const Timestamp& started, int maxIdleMinutes)
    : id_(id), userId_(userId), started_(started), lastActivity_(started),
      maxIdleMinutes_(maxIdleMinutes), active_(true) {
    if (maxIdleMinutes <= 0) throw InvalidOperationException("время простоя сессии должно быть положительным");
}

/** @brief Без этой проверки истёкшая час назад сессия снова стала бы живой. */
void Session::touch(const Timestamp& now) {
    if (isExpired(now)) throw SessionExpiredException(id_);
    lastActivity_ = now;
}

void Session::close() { active_ = false; }

bool Session::isExpired(const Timestamp& now) const {
    return !active_ || lastActivity_.minutesTo(now) > maxIdleMinutes_;
}

long long Session::minutesLeft(const Timestamp& now) const {
    if (isExpired(now)) return 0;
    return maxIdleMinutes_ - lastActivity_.minutesTo(now);
}

/** @brief close() не получает время, поэтому конец закрытой сессии — её последняя активность. */
long long Session::durationMinutes(const Timestamp& now) const {
    return started_.minutesTo(active_ ? now : lastActivity_);
}

int Session::getId() const { return id_; }
int Session::getUserId() const { return userId_; }
const Timestamp& Session::getStarted() const { return started_; }
const Timestamp& Session::getLastActivity() const { return lastActivity_; }
int Session::getMaxIdleMinutes() const { return maxIdleMinutes_; }
bool Session::isActive() const { return active_; }

bool Session::operator==(const Session& o) const { return id_ == o.id_; }
bool Session::operator!=(const Session& o) const { return !(*this == o); }

std::ostream& operator<<(std::ostream& os, const Session& s) {
    return os << "Сессия #" << s.id_ << " пользователя #" << s.userId_ << " с " << s.started_
              << (s.active_ ? ", активна" : ", закрыта");
}
