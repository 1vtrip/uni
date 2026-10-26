#pragma once
#include <string>
#include <iostream>
#include "core/Date.h"
#include "interfaces/ICertifiable.h"

/** @brief Инструмент: инвентарный номер, дата калибровки и кто его сейчас держит (0 — на месте). */
class Tool : public ICertifiable {
public:
    Tool(const std::string& invNumber, const std::string& name, const Date& calibrated);

    /** @brief Свободен ли инструмент. */
    bool isAvailable() const;
    /** @brief Взять инструмент мастером techId; false, если его уже кто-то взял. */
    bool take(int techId);   // TODO(exc): WorkbenchOccupiedException
    /** @brief Вернуть инструмент на место. */
    void giveBack();
    /** @brief Нужна ли калибровка: с прошлой прошло больше 180 дней. */
    bool needsCalibration(const Date& today) const;
    /** @brief Откалибровать в день today. */
    void calibrate(const Date& today);
    /** @brief Сколько дней осталось до следующей калибровки (отрицательно, если просрочена). */
    int daysToCalibration(const Date& today) const;
    /** @brief То же, что needsCalibration (из ICertifiable). */
    bool needsCheck(const Date& today) const override;
    /** @brief То же, что calibrate (из ICertifiable). */
    void certify(const Date& today) override;

    const std::string& getInvNumber() const;
    const std::string& getName() const;
    const Date& getCalibrated() const;
    int getHolderId() const;

    /** @brief Инструменты равны, если совпадает инвентарный номер. */
    bool operator==(const Tool& other) const;
    bool operator!=(const Tool& other) const;
    friend std::ostream& operator<<(std::ostream& os, const Tool& t);

private:
    std::string invNumber_;
    std::string name_;
    Date calibrated_;
    int holderId_;
};
