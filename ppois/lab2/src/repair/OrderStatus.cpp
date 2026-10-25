#include "repair/OrderStatus.h"

OrderStatus::OrderStatus(int s) : value_(s) {}

int OrderStatus::get() const { return value_; }

bool OrderStatus::canMoveTo(int next) const {
    switch (value_) {
        case ACCEPTED:      return next == DIAGNOSTICS || next == CANCELLED;
        case DIAGNOSTICS:   return next == WAITING_PARTS || next == IN_REPAIR || next == CANCELLED;
        case WAITING_PARTS: return next == IN_REPAIR || next == CANCELLED;
        case IN_REPAIR:     return next == QUALITY_CHECK;
        case QUALITY_CHECK: return next == READY || next == IN_REPAIR;
        case READY:         return next == ISSUED;
        default:            return false;   // ISSUED, CANCELLED
    }
}

bool OrderStatus::moveTo(int next) {
    if (!canMoveTo(next))
        return false;   // TODO(exc): InvalidStatusTransitionException(toString(value_), toString(next))
    value_ = next;
    return true;
}

bool OrderStatus::isFinal() const { return value_ == ISSUED || value_ == CANCELLED; }

std::string OrderStatus::toString() const { return toString(value_); }

std::string OrderStatus::toString(int s) {
    switch (s) {
        case ACCEPTED:      return "Принят";
        case DIAGNOSTICS:   return "Диагностика";
        case WAITING_PARTS: return "Ожидание запчастей";
        case IN_REPAIR:     return "В ремонте";
        case QUALITY_CHECK: return "Проверка качества";
        case READY:         return "Готов";
        case ISSUED:        return "Выдан";
        case CANCELLED:     return "Отменён";
    }
    return "?";
}

bool OrderStatus::operator==(const OrderStatus& other) const { return value_ == other.value_; }
bool OrderStatus::operator!=(const OrderStatus& other) const { return !(*this == other); }

std::ostream& operator<<(std::ostream& os, const OrderStatus& s) { return os << s.toString(); }
