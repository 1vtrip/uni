#pragma once
#include <stdexcept>
#include <string>

/** @brief База всех исключений фирмы: их можно поймать одним catch (const RepairFirmException&). */
class RepairFirmException : public std::runtime_error {
public:
    explicit RepairFirmException(const std::string& msg) : std::runtime_error(msg) {}
};

/** @brief Неверный телефон, e-mail или УНП клиента. */
class InvalidClientDataException : public RepairFirmException {
public:
    explicit InvalidClientDataException(const std::string& reason)
        : RepairFirmException("Неверные данные клиента: " + reason) {}
};

/** @brief Клиента с таким ID нет. */
class ClientNotFoundException : public RepairFirmException {
public:
    explicit ClientNotFoundException(int clientId)
        : RepairFirmException("Клиент #" + std::to_string(clientId) + " не найден") {}
};

/** @brief Заказа с таким ID нет. */
class OrderNotFoundException : public RepairFirmException {
public:
    explicit OrderNotFoundException(int orderId)
        : RepairFirmException("Заказ №" + std::to_string(orderId) + " не найден") {}
};

/** @brief Запрещённый переход статуса заказа. */
class InvalidStatusTransitionException : public RepairFirmException {
public:
    InvalidStatusTransitionException(const std::string& from, const std::string& to)
        : RepairFirmException("Нельзя перевести заказ из «" + from + "» в «" + to + "»") {}
};

/** @brief Устройство нельзя отремонтировать (слишком много критических дефектов). */
class DeviceNotRepairableException : public RepairFirmException {
public:
    explicit DeviceNotRepairableException(const std::string& serial)
        : RepairFirmException("Устройство " + serial + " неремонтопригодно, заказ отменён") {}
};

/** @brief Запчасть не подходит к модели устройства. */
class IncompatiblePartException : public RepairFirmException {
public:
    IncompatiblePartException(const std::string& article, const std::string& model)
        : RepairFirmException("Запчасть " + article + " не подходит к модели " + model) {}
};

/** @brief На складе не хватает запчастей. */
class OutOfStockException : public RepairFirmException {
public:
    OutOfStockException(const std::string& article, int requested, int available)
        : RepairFirmException("Недостаточно запчастей " + article + ": нужно "
              + std::to_string(requested) + ", есть " + std::to_string(available)) {}
};

/** @brief Нет свободного мастера, который умеет работать с устройством. */
class TechnicianNotAvailableException : public RepairFirmException {
public:
    explicit TechnicianNotAvailableException(const std::string& reason)
        : RepairFirmException("Мастер недоступен: " + reason) {}
};

/** @brief Смена или запись пересекается с другой либо выпадает на нерабочее время. */
class ScheduleConflictException : public RepairFirmException {
public:
    explicit ScheduleConflictException(const std::string& date)
        : RepairFirmException("Конфликт расписания: " + date) {}
};

/** @brief Рабочее место или инструмент уже заняты. */
class WorkbenchOccupiedException : public RepairFirmException {
public:
    explicit WorkbenchOccupiedException(const std::string& what)
        : RepairFirmException("Занято: " + what) {}
};

/** @brief Гарантия истекла или не покрывает запчасть. */
class WarrantyExpiredException : public RepairFirmException {
public:
    explicit WarrantyExpiredException(int orderId)
        : RepairFirmException("Гарантия по заказу №" + std::to_string(orderId) + " не действует") {}
};

/** @brief Скидку нельзя применить: не одобрена, просрочена или сумма мала. */
class InvalidDiscountException : public RepairFirmException {
public:
    explicit InvalidDiscountException(const std::string& code)
        : RepairFirmException("Скидка " + code + " неприменима") {}
};

/** @brief Не хватает денег в кассе, баллов на карте или внесённой суммы. */
class InsufficientFundsException : public RepairFirmException {
public:
    explicit InsufficientFundsException(const std::string& reason)
        : RepairFirmException("Недостаточно средств: " + reason) {}
};

/** @brief Проверка качества не пройдена. */
class QualityCheckFailedException : public RepairFirmException {
public:
    explicit QualityCheckFailedException(int orderId)
        : RepairFirmException("Заказ №" + std::to_string(orderId) + " не прошёл проверку качества") {}
};

/** @brief Отзыв некорректен: оценка вне 1-5 или низкая оценка без текста. */
class InvalidReviewException : public RepairFirmException {
public:
    explicit InvalidReviewException(int rating)
        : RepairFirmException("Некорректный отзыв с оценкой " + std::to_string(rating)) {}
};

/** @brief Несуществующая дата. */
class InvalidDateException : public RepairFirmException {
public:
    explicit InvalidDateException(const std::string& date)
        : RepairFirmException("Несуществующая дата: " + date) {}
};
