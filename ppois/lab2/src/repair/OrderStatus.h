#pragma once
#include <string>
#include <iostream>

/**
 * @brief Статус заказа и разрешённые переходы.
 * Принят -> Диагностика -> (Ожидание запчастей) -> В ремонте -> Проверка качества -> Готов -> Выдан.
 * Из проверки качества можно вернуться в ремонт, отменить можно до начала ремонта.
 */
class OrderStatus {
public:
    /** @brief Возможные статусы. enum class: значения не превращаются в int сами и не конфликтуют по именам. */
    enum class Status { Accepted, Diagnostics, WaitingParts, InRepair, QualityCheck, Ready, Issued, Cancelled };

    explicit OrderStatus(Status s = Status::Accepted);

    /** @brief Разрешён ли переход из текущего статуса в next. */
    bool canMoveTo(Status next) const;
    /** @brief Перейти в next; запрещённый переход -> InvalidStatusTransitionException, статус не меняется. */
    void moveTo(Status next);
    /** @brief Финальный статус: Выдан или Отменён, дальше переходов нет. */
    bool isFinal() const;
    /** @brief Название текущего статуса. */
    std::string toString() const;
    /** @brief Название статуса s. */
    static std::string toString(Status s);

    Status get() const;

    bool operator==(const OrderStatus& other) const;
    bool operator!=(const OrderStatus& other) const;
    friend std::ostream& operator<<(std::ostream& os, const OrderStatus& s);

private:
    Status value_;
};
