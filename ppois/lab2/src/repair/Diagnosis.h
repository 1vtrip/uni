#pragma once
#include <vector>
#include <iostream>
#include "core/Date.h"
#include "core/Money.h"
#include "devices/Defect.h"
#include "finance/PriceList.h"

/** @brief Результат диагностики: мастер, найденные дефекты, ремонтопригодность и дата. */
class Diagnosis {
public:
    /** @brief Диагноза ещё нет: мастер 0, устройство считается ремонтопригодным. */
    Diagnosis();
    Diagnosis(int technicianId, const Date& date);

    /** @brief Добавить один дефект. */
    void addDefect(const Defect& d);
    /** @brief Добавить список дефектов (например, результат Device::quickTest). */
    void addDefects(const std::vector<Defect>& list);
    /** @brief Сколько часов работы нужно на все дефекты. */
    double totalHours() const;
    /** @brief Предварительная стоимость работ по ставке прайс-листа (без срочности). */
    Money estimate(const PriceList& prices) const;
    /** @brief Решить, ремонтопригодно ли: нет, если критических дефектов больше двух. Результат сохраняется. */
    bool decideRepairable();
    /** @brief Число критических дефектов. */
    int criticalCount() const;
    /** @brief Проведена ли диагностика (назначен мастер). */
    bool isDone() const;

    int getTechnicianId() const;
    const std::vector<Defect>& getDefects() const;
    bool isRepairable() const;
    const Date& getDate() const;

    bool operator==(const Diagnosis& other) const;
    bool operator!=(const Diagnosis& other) const;
    friend std::ostream& operator<<(std::ostream& os, const Diagnosis& d);

private:
    int technicianId_;
    std::vector<Defect> defects_;
    bool repairable_;
    Date date_;
};
