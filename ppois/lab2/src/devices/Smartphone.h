#pragma once
#include "devices/Device.h"

/** @brief Смартфон: IMEI, диагональ экрана и признак попадания влаги. */
class Smartphone : public Device {
public:
    Smartphone(const std::string& serial, const std::string& model, const Brand& brand,
               const DeviceCategory& category, const Date& purchased,
               const std::string& imei, double screen, bool wet);

    std::string typeName() const override;
    /** @brief Базовый срок + 5 дней, если телефон был в воде. */
    int estimateRepairDays() const override;
    /** @brief Базовая проверка + следы влаги и некорректный IMEI. */
    std::vector<Defect> quickTest(const Date& today) const override;
    /** @brief Базовый вывод + IMEI и отметка о влаге. */
    void print(std::ostream& os) const override;

    /** @brief 15 цифр и контрольная сумма по алгоритму Луна. */
    bool isImeiValid() const;
    /** @brief Цена замены экрана: 30 руб. за дюйм диагонали. */
    Money screenReplacementCost() const;

    const std::string& getImei() const;
    double getScreen() const;
    bool isWet() const;

private:
    std::string imei_;
    double screen_;
    bool wet_;
};
