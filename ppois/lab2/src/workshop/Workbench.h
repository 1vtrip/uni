#pragma once
#include <string>
#include <iostream>

/** @brief Рабочее место в мастерской: номер, оборудование и мастер, который его занял (0 — свободно). */
class Workbench {
public:
    Workbench(int number, const std::string& equipment);

    /** @brief Свободно ли место. */
    bool isFree() const;
    /** @brief Занять место мастером techId; место занято или techId <= 0 -> WorkbenchOccupiedException. */
    void occupy(int techId);
    /** @brief Освободить место. */
    void release();

    int getNumber() const;
    const std::string& getEquipment() const;
    int getTechnicianId() const;

    /** @brief Места равны, если совпадает номер. */
    bool operator==(const Workbench& other) const;
    bool operator!=(const Workbench& other) const;
    friend std::ostream& operator<<(std::ostream& os, const Workbench& w);

private:
    int number_;
    std::string equipment_;
    int technicianId_;
};
