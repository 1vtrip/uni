#pragma once
#include <string>
#include <vector>
#include <iostream>

/** @brief Проверка качества перед выдачей. Пункт i: название items_[i], результат results_[i]. */
class QualityCheck {
public:
    QualityCheck(int orderId, int inspectorId);

    /** @brief Добавить пункт проверки; сначала он считается не пройденным. */
    void addItem(const std::string& item);
    /** @brief Отметить пункт index; нет такого пункта -> RepairFirmException. */
    void mark(int index, bool ok);
    /** @brief Пройдена: пункты есть и все отмечены как пройденные. */
    bool passed() const;
    /** @brief Потребовать, чтобы проверка была пройдена: иначе QualityCheckFailedException. passed() спрашивает, confirm() требует. */
    void confirm() const;
    /** @brief Названия не пройденных пунктов: что нужно переделать. */
    std::vector<std::string> failedItems() const;

    int getOrderId() const;
    int getInspectorId() const;
    const std::vector<std::string>& getItems() const;
    const std::vector<bool>& getResults() const;

    bool operator==(const QualityCheck& other) const;
    bool operator!=(const QualityCheck& other) const;
    friend std::ostream& operator<<(std::ostream& os, const QualityCheck& q);

private:
    int orderId_;
    int inspectorId_;
    std::vector<std::string> items_;
    std::vector<bool> results_;
};
