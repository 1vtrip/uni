#include "storage/Quota.h"
#include "exceptions/Exceptions.h"
#include <string>

Quota::Quota(int userId, const DataSize& limit, double warnPercent)
    : userId_(userId), limit_(limit), used_(0), warnPercent_(warnPercent) {}

bool Quota::canAllocate(const DataSize& size) const { return used_ + size <= limit_; }

void Quota::consume(const DataSize& size) {
    if (!canAllocate(size))
        throw QuotaExceededException("#" + std::to_string(userId_),
                                     size.toString(), remaining().toString());
    used_ += size;
}

void Quota::release(const DataSize& size) { used_ -= size; }   // оператор -= не уходит ниже 0
DataSize Quota::remaining() const { return limit_ - used_; }
double Quota::usagePercent() const { return used_.percentOf(limit_); }
bool Quota::isNearLimit() const { return usagePercent() >= warnPercent_; }

void Quota::setLimit(const DataSize& limit) {
    if (limit < used_)
        throw InvalidOperationException("Новый лимит меньше занятого места");
    limit_ = limit;
}

int Quota::getUserId() const { return userId_; }
const DataSize& Quota::getLimit() const { return limit_; }
const DataSize& Quota::getUsed() const { return used_; }
bool Quota::operator==(const Quota& o) const { return userId_ == o.userId_; }
bool Quota::operator!=(const Quota& o) const { return !(*this == o); }

std::ostream& operator<<(std::ostream& os, const Quota& q) {
    return os << q.used_ << " / " << q.limit_ << " (" << (int)q.usagePercent() << "%)"
              << (q.isNearLimit() ? " [ПОЧТИ ЗАПОЛНЕНА]" : "");
}
