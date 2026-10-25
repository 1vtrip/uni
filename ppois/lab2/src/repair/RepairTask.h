#pragma once
#include <string>
#include <vector>
#include <iostream>
#include "core/Money.h"
#include "inventory/SparePart.h"
#include "finance/PriceList.h"
#include "repair/UsedPart.h"

/** @brief Forward declaration: в заголовке Device нужен только как const Device&. */
class Device;

/** @brief Одна работа в заказе: исполнитель, фактические часы и установленные запчасти. */
class RepairTask {
public:
    RepairTask(int id, const std::string& description);

    /** @brief Мастер берёт задачу; false, если она уже закрыта или её взял другой мастер. */
    bool start(int technicianId);
    /** @brief Закрыть задачу с фактическими часами; false, если не начата, уже закрыта или hours <= 0. */
    bool finish(double hours);   // TODO(exc): RepairFirmException
    /**
     * @brief Записать установленную запчасть по цене продажи на момент установки.
     * Со склада не списывает (это делает Warehouse::take). false, если задача закрыта или запчасть не подходит.
     */
    bool addPart(const SparePart& part, int qty, const Device& d);   // TODO(exc): IncompatiblePartException
    /** @brief Стоимость установленных запчастей. */
    Money partsCost() const;
    /** @brief Полная стоимость: работа по фактическим часам (с наценкой при срочности) + запчасти. */
    Money cost(const PriceList& prices, bool urgent) const;
    /** @brief Взята ли задача мастером. */
    bool isStarted() const;

    int getId() const;
    const std::string& getDescription() const;
    int getTechnicianId() const;
    double getHours() const;
    const std::vector<UsedPart>& getParts() const;
    bool isDone() const;

    /** @brief Задачи равны, если совпадает id. */
    bool operator==(const RepairTask& other) const;
    bool operator!=(const RepairTask& other) const;
    friend std::ostream& operator<<(std::ostream& os, const RepairTask& t);

private:
    int id_;
    std::string description_;
    int technicianId_;
    double hours_;
    std::vector<UsedPart> parts_;
    bool done_;
};
