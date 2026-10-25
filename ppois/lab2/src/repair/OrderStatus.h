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
    // TODO(exc): на этапе «Переход» константы заменятся на enum class Status
    static const int ACCEPTED = 0, DIAGNOSTICS = 1, WAITING_PARTS = 2, IN_REPAIR = 3,
                     QUALITY_CHECK = 4, READY = 5, ISSUED = 6, CANCELLED = 7;

    explicit OrderStatus(int s = ACCEPTED);

    /** @brief Разрешён ли переход из текущего статуса в next. */
    bool canMoveTo(int next) const;
    /** @brief Перейти в next; false и статус не меняется, если переход запрещён. */
    bool moveTo(int next);   // TODO(exc): InvalidStatusTransitionException
    /** @brief Финальный статус: Выдан или Отменён, дальше переходов нет. */
    bool isFinal() const;
    /** @brief Название текущего статуса. */
    std::string toString() const;
    /** @brief Название статуса s. */
    static std::string toString(int s);

    int get() const;

    bool operator==(const OrderStatus& other) const;
    bool operator!=(const OrderStatus& other) const;
    friend std::ostream& operator<<(std::ostream& os, const OrderStatus& s);

private:
    int value_;
};
