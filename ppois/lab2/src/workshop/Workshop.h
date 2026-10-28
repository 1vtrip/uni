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
     * Если мастер уже на месте, возвращается номер этого места. Свободных нет -> WorkbenchOccupiedException.
     */
    int assignBench(int techId);
    /** @brief Освободить место по номеру; false, если такого места нет. */
    bool releaseBench(int number);
    /** @brief Сколько мест сейчас свободно. */
    int freeBenches() const;
    /** @brief Инструмент по названию; нет такого -> RepairFirmException. Ссылку не хранить: после addTool она может устареть. */
    Tool& findTool(const std::string& name);
    const Tool& findTool(const std::string& name) const;
    /** @brief Инструменты, которым нужна калибровка на дату today. */
    std::vector<Tool> toolsToCalibrate(const Date& today) const;

    const std::string& getName() const;
    const Address& getAddress() const;
    const std::vector<Workbench>& getBenches() const;
    const std::vector<Tool>& getTools() const;

    friend std::ostream& operator<<(std::ostream& os, const Workshop& w);

private:
    /** @brief Индекс инструмента по названию или -1 (внутренний помощник для findTool). */
    int indexOfTool(const std::string& name) const;

    std::string name_;
    Address address_;
    std::vector<Workbench> benches_;
    std::vector<Tool> tools_;
};
