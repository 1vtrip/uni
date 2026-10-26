#include "repair/QualityCheck.h"

QualityCheck::QualityCheck(int orderId, int inspectorId)
    : orderId_(orderId), inspectorId_(inspectorId) {}

void QualityCheck::addItem(const std::string& item) {
    items_.push_back(item);
    results_.push_back(false);
}

bool QualityCheck::mark(int index, bool ok) {
    if (index < 0 || index >= (int)results_.size())
        return false;   // TODO(exc): RepairFirmException("Нет пункта проверки №...")
    results_[index] = ok;
    return true;
}

bool QualityCheck::passed() const {
    if (results_.empty()) return false;
    for (size_t i = 0; i < results_.size(); ++i)
        if (!results_[i]) return false;
    return true;
}

bool QualityCheck::confirm() const {
    return passed();   // TODO(exc): if (!passed()) throw QualityCheckFailedException(orderId_)
}

std::vector<std::string> QualityCheck::failedItems() const {
    std::vector<std::string> result;
    for (size_t i = 0; i < items_.size(); ++i)
        if (!results_[i]) result.push_back(items_[i]);
    return result;
}

int QualityCheck::getOrderId() const { return orderId_; }
int QualityCheck::getInspectorId() const { return inspectorId_; }
const std::vector<std::string>& QualityCheck::getItems() const { return items_; }
const std::vector<bool>& QualityCheck::getResults() const { return results_; }

bool QualityCheck::operator==(const QualityCheck& other) const {
    return orderId_ == other.orderId_ && inspectorId_ == other.inspectorId_
        && items_ == other.items_ && results_ == other.results_;
}

bool QualityCheck::operator!=(const QualityCheck& other) const { return !(*this == other); }

std::ostream& operator<<(std::ostream& os, const QualityCheck& q) {
    os << "Проверка качества заказа №" << q.orderId_ << ", проверяющий #" << q.inspectorId_
       << (q.passed() ? ": ПРОЙДЕНА" : ": не пройдена");
    for (size_t i = 0; i < q.items_.size(); ++i)
        os << "\n  [" << (q.results_[i] ? "+" : " ") << "] " << q.items_[i];
    return os;
}