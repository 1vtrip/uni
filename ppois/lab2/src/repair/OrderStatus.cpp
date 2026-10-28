#include "repair/OrderStatus.h"
#include "exceptions/Exceptions.h"

using S = OrderStatus::Status;

OrderStatus::OrderStatus(Status s) : value_(s) {}

OrderStatus::Status OrderStatus::get() const { return value_; }

/** @brief Таблица переходов; из Issued и Cancelled (default) переходов нет. */
bool OrderStatus::canMoveTo(Status next) const {
    switch (value_) {
        case S::Accepted:     return next == S::Diagnostics || next == S::Cancelled;
        case S::Diagnostics:  return next == S::WaitingParts || next == S::InRepair || next == S::Cancelled;
        case S::WaitingParts: return next == S::InRepair || next == S::Cancelled;
        case S::InRepair:     return next == S::QualityCheck;
        case S::QualityCheck: return next == S::Ready || next == S::InRepair;
        case S::Ready:        return next == S::Issued;
        default:              return false;
    }
}

void OrderStatus::moveTo(Status next) {
    if (!canMoveTo(next))
        throw InvalidStatusTransitionException(toString(value_), toString(next));
    value_ = next;
}

bool OrderStatus::isFinal() const { return value_ == S::Issued || value_ == S::Cancelled; }

std::string OrderStatus::toString() const { return toString(value_); }

std::string OrderStatus::toString(Status s) {
    switch (s) {
        case S::Accepted:     return "Принят";
        case S::Diagnostics:  return "Диагностика";
        case S::WaitingParts: return "Ожидание запчастей";
        case S::InRepair:     return "В ремонте";
        case S::QualityCheck: return "Проверка качества";
        case S::Ready:        return "Готов";
        case S::Issued:       return "Выдан";
        case S::Cancelled:    return "Отменён";
    }
    return "?";
}

bool OrderStatus::operator==(const OrderStatus& other) const { return value_ == other.value_; }
bool OrderStatus::operator!=(const OrderStatus& other) const { return !(*this == other); }

std::ostream& operator<<(std::ostream& os, const OrderStatus& s) { return os << s.toString(); }
