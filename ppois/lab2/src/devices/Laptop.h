#pragma once
#include "devices/Device.h"

/** @brief Ноутбук: процессор, объём памяти и тип накопителя ("HDD" или "SSD"). */
class Laptop : public Device {
public:
    Laptop(const std::string& serial, const std::string& model, const Brand& brand,
           const DeviceCategory& category, const Date& purchased,
           const std::string& cpu, int ramGb, const std::string& storage);

    std::string typeName() const override;
    /** @brief Базовый срок + 2 дня, если диск HDD (нужно время на копирование данных). */
    int estimateRepairDays() const override;
    /** @brief Базовая цена диагностики * 1,5: ноутбук разбирать дольше. */
    Money diagnosticPrice() const override;
    /** @brief Базовая проверка + мало памяти (< 4 ГБ) и устаревший HDD. */
    std::vector<Defect> quickTest(const Date& today) const override;
    /** @brief Базовый вывод + процессор, память и диск. */
    void print(std::ostream& os) const override;

    /** @brief Нужна ли резервная копия данных перед ремонтом: да для HDD. */
    bool needsBackup() const;

    const std::string& getCpu() const;
    int getRamGb() const;
    const std::string& getStorage() const;

private:
    std::string cpu_;
    int ramGb_;
    std::string storage_;
};
