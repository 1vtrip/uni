#include "storage/TrashItem.h"
#include "exceptions/Exceptions.h"

TrashItem::TrashItem(int entryId, int originalParentId, const std::string& originalPath, int deletedBy,
                     const DataSize& size, const Timestamp& deletedAt, int retentionDays)
    : entryId_(entryId), originalParentId_(originalParentId), originalPath_(originalPath),
      deletedAt_(deletedAt), expiresAt_(deletedAt.addDays(retentionDays)), deletedBy_(deletedBy), size_(size) {
    if (retentionDays <= 0) throw InvalidOperationException("срок хранения в корзине должен быть положительным");
}

bool TrashItem::isExpired(const Timestamp& now) const { return expiresAt_ <= now; }

int TrashItem::daysLeft(const Timestamp& now) const {
    int days = now.daysTo(expiresAt_);
    return days < 0 ? 0 : days;
}

int TrashItem::getEntryId() const { return entryId_; }
int TrashItem::getOriginalParentId() const { return originalParentId_; }
const std::string& TrashItem::getOriginalPath() const { return originalPath_; }
const Timestamp& TrashItem::getDeletedAt() const { return deletedAt_; }
const Timestamp& TrashItem::getExpiresAt() const { return expiresAt_; }
int TrashItem::getDeletedBy() const { return deletedBy_; }
const DataSize& TrashItem::getSize() const { return size_; }

bool TrashItem::operator==(const TrashItem& o) const { return entryId_ == o.entryId_; }
bool TrashItem::operator!=(const TrashItem& o) const { return !(*this == o); }

std::ostream& operator<<(std::ostream& os, const TrashItem& t) {
    return os << '#' << t.entryId_ << ' ' << t.originalPath_ << "  " << t.size_
              << "  удалён " << t.deletedAt_ << " (#" << t.deletedBy_ << "), хранится до " << t.expiresAt_;
}
