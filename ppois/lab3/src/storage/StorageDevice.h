#pragma once
#include <iostream>
#include <string>
#include "core/DataSize.h"

/** @brief Физический накопитель: SSD или HDD с ёмкостью и занятым местом. */
class StorageDevice {
public:
    /** @brief @throws InvalidOperationException тип не "SSD" и не "HDD". */
    StorageDevice(int id = 1, const std::string& name = "disk0", const std::string& type = "SSD",
                  const DataSize& capacity = DataSize::fromGB(10), bool online = true);

    /**
     * @brief Занять место.
     * @throws InsufficientStorageException устройство отключено или места не хватает.
     */
    void allocate(const DataSize& s);
    /** @brief Освободить место; занятое не уходит ниже 0. */
    void free(const DataSize& s);
    DataSize freeSpace() const;
    double usagePercent() const;
    /** @brief Сколько секунд займёт запись: SSD — 500 МБ/с, HDD — 120 МБ/с. */
    double estimateWriteSeconds(const DataSize& s) const;
    void setOnline(bool v);
    bool isOnline() const;

    int getId() const;
    const std::string& getName() const;
    const std::string& getType() const;
    const DataSize& getCapacity() const;
    const DataSize& getUsed() const;

    /** @brief Устройства равны, если совпадают ID. */
    bool operator==(const StorageDevice& o) const;
    bool operator!=(const StorageDevice& o) const;
    /** @brief "disk0 (SSD): 1.0 GB / 10.0 GB, 10%". */
    friend std::ostream& operator<<(std::ostream& os, const StorageDevice& d);

private:
    int id_;
    std::string name_;
    std::string type_;
    DataSize capacity_;
    DataSize used_;
    bool online_;
};
