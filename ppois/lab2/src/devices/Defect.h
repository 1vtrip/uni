#pragma once
#include <string>
#include <iostream>

/** @brief Неисправность устройства: код, описание и тяжесть от 1 (мелочь) до 5 (критично). */
class Defect {
public:
    Defect(const std::string& code, const std::string& description, int severity);

    /** @brief Критический ли дефект (тяжесть 4 и выше). */
    bool isCritical() const;
    /** @brief Сколько часов работы нужно на устранение: тяжесть * 0,5. */
    double laborHours() const;

    const std::string& getCode() const;
    const std::string& getDescription() const;
    int getSeverity() const;

    bool operator==(const Defect& other) const;
    bool operator!=(const Defect& other) const;
    friend std::ostream& operator<<(std::ostream& os, const Defect& d);

private:
    std::string code_;
    std::string description_;
    int severity_;
};
