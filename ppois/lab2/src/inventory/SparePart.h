#pragma once
#include <string>
#include <vector>
#include <iostream>
#include "core/Money.h"

/** @brief Forward declaration: в заголовке Device нужен только как const Device&. */
class Device;

/** @brief Запчасть: артикул, название, модели, к которым подходит, закупочная и продажная цена. */
class SparePart {
public:
    SparePart(const std::string& article, const std::string& name,
              const std::vector<std::string>& models, const Money& buyPrice, const Money& sellPrice);

    /** @brief Подходит ли запчасть к устройству: его модель есть в списке моделей. */
    bool fits(const Device& d) const;
    /** @brief Добавить модель в список совместимых, если её там ещё нет. */
    void addModel(const std::string& model);
    /** @brief Наценка: цена продажи минус закупочная. */
    Money margin() const;
    /** @brief Наценка в процентах от закупочной цены (0, если закупочная цена 0). */
    double marginPercent() const;

    const std::string& getArticle() const;
    const std::string& getName() const;
    const std::vector<std::string>& getModels() const;
    const Money& getBuyPrice() const;
    const Money& getSellPrice() const;

    /** @brief Запчасти равны, если совпадает артикул. */
    bool operator==(const SparePart& other) const;
    bool operator!=(const SparePart& other) const;
    friend std::ostream& operator<<(std::ostream& os, const SparePart& p);

private:
    std::string article_;
    std::string name_;
    std::vector<std::string> models_;
    Money buyPrice_;
    Money sellPrice_;
};
