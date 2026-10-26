#pragma once
#include <string>
#include <vector>
#include <iostream>
#include "core/Address.h"
#include "core/Date.h"
#include "workshop/Workbench.h"
#include "workshop/Tool.h"

/** @brief Мастерская: адрес, рабочие места и инструменты. */
class Workshop {
public:
    Workshop(const std::string& name, const Address& address);

    /** @brief Добавить место; false, если место с таким номером уже есть. */
    bool addBench(const Workbench& b);
    /** @brief Добавить инструмент; false, если такой инвентарный номер уже есть. */
    bool addTool(const Tool& t);
    /** @brief Номер места, которое занимает мастер techId, или -1. */
    int benchOf(int techId) const;
    /**
     * @brief Дать мастеру первое свободное место и вернуть его номер.
     * Если мастер уже на месте, возвращается номер этого места. -1, если свободных нет.
     */
    int assignBench(int techId);   // TODO(exc): WorkbenchOccupiedException
    /** @brief Освободить место по номеру; false, если такого места нет. */
    bool releaseBench(int number);
    /** @brief Сколько мест сейчас свободно. */
    int freeBenches() const;
    /** @brief Индекс инструмента по названию или -1. */
    int indexOfTool(const std::string& name) const;   // TODO(exc): вместе с toolAt -> findTool + RepairFirmException
    /** @brief Инструмент по индексу из indexOfTool. Ссылку не хранить: после addTool она может устареть. */
    Tool& toolAt(int index);
    const Tool& toolAt(int index) const;
    /** @brief Инструменты, которым нужна калибровка на дату today. */
    std::vector<Tool> toolsToCalibrate(const Date& today) const;

    const std::string& getName() const;
    const Address& getAddress() const;
    const std::vector<Workbench>& getBenches() const;
    const std::vector<Tool>& getTools() const;

    friend std::ostream& operator<<(std::ostream& os, const Workshop& w);

private:
    std::string name_;
    Address address_;
    std::vector<Workbench> benches_;
    std::vector<Tool> tools_;
};
