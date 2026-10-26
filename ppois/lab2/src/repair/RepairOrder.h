#pragma once
#include <string>
#include <vector>
#include <iostream>
#include "core/Date.h"
#include "core/Money.h"
#include "devices/Device.h"
#include "finance/PriceList.h"
#include "repair/OrderStatus.h"
#include "repair/Diagnosis.h"
#include "repair/RepairTask.h"
#include "interfaces/IPrintable.h"

/**
 * @brief Заказ на ремонт: устройство, статус, срок, диагноз и работы.
 * Устройство хранится по значению, поэтому от наследника остаётся только часть Device (срезка).
 * Срок, цена диагностики и описание устройства вычисляются в конструкторе, пока параметр
 * device ещё ссылается на настоящий объект наследника.
 */
class RepairOrder : public IPrintable {
public:
    RepairOrder(int id, int clientId, const Device& device, const std::string& problem,
                const Date& created, bool urgent);

    /** @brief Перевести заказ в статус s; false, если переход запрещён. */
    bool setStatus(int s);   // TODO(exc): InvalidStatusTransitionException
    /** @brief Сохранить диагноз; если устройство неремонтопригодно, заказ отменяется и возвращается false. */
    bool setDiagnosis(const Diagnosis& d);   // TODO(exc): DeviceNotRepairableException
    /** @brief Добавить работу; false, если заказ уже выдан или отменён. */
    bool addTask(const RepairTask& t);   // TODO(exc): RepairFirmException
    /** @brief Индекс задачи по id или -1. */
    int indexOfTask(int taskId) const;   // TODO(exc): вместе с taskAt -> findTask + RepairFirmException
    /** @brief Задача по индексу из indexOfTask. Ссылку не хранить: после addTask она может устареть. */
    RepairTask& taskAt(int index);
    const RepairTask& taskAt(int index) const;
    /** @brief Все работы выполнены; false, если работ ещё нет. */
    bool allTasksDone() const;
    /** @brief Полная стоимость: диагностика + все работы (с наценкой, если заказ срочный). */
    Money totalCost(const PriceList& prices) const;
    /** @brief Просрочен: заказ не завершён, а срок уже прошёл. */
    bool isOverdue(const Date& today) const;
    /** @brief Сколько дней осталось до срока (отрицательно, если просрочен). */
    int daysLeft(const Date& today) const;
    /** @brief Отменить заказ; false, если ремонт уже начат или заказ завершён. */
    bool cancel();

    int getId() const;
    int getClientId() const;
    const Device& getDevice() const;
    const std::string& getProblem() const;
    const OrderStatus& getStatus() const;
    const Date& getCreated() const;
    const Date& getDeadline() const;
    const Diagnosis& getDiagnosis() const;
    const std::vector<RepairTask>& getTasks() const;
    const Money& getDiagnosticPrice() const;
    const std::string& getDeviceInfo() const;
    bool isUrgent() const;

    /** @brief Заказы равны, если совпадает id. */
    bool operator==(const RepairOrder& other) const;
    bool operator!=(const RepairOrder& other) const;
    /** @brief Номер, устройство, проблема, статус, сроки и работы (из IPrintable). */
    void print(std::ostream& os) const override;

private:
    int id_;
    int clientId_;
    Device device_;
    std::string problem_;
    OrderStatus status_;
    Date created_;
    Date deadline_;
    Diagnosis diagnosis_;
    std::vector<RepairTask> tasks_;
    Money diagnosticPrice_;
    std::string deviceInfo_;
    bool urgent_;
};
