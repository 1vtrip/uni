#pragma once
#include <string>
#include <vector>
#include <iostream>
#include "core/Date.h"

/** @brief Гарантия на ремонт: с какого дня, на сколько месяцев и какие запчасти покрывает. */
class Warranty {
public:
    Warranty(int orderId, const Date& start, int months);

    /** @brief Добавить запчасть в список покрытых гарантией (без повторов). */
    void addArticle(const std::string& article);
    /** @brief Последний день гарантии. */
    Date endDate() const;
    /** @brief Действует ли гарантия в день today (начало и конец включительно). */
    bool isValid(const Date& today) const;
    /** @brief Покрывает ли гарантия запчасть с этим артикулом. */
    bool covers(const std::string& article) const;
    /** @brief Продлить на months месяцев; false, если months <= 0. */
    bool extend(int months);

    int getOrderId() const;
    const Date& getStart() const;
    int getMonths() const;
    const std::vector<std::string>& getCoveredArticles() const;

    /** @brief Гарантии равны, если это гарантия на один и тот же заказ. */
    bool operator==(const Warranty& other) const;
    bool operator!=(const Warranty& other) const;
    friend std::ostream& operator<<(std::ostream& os, const Warranty& w);

private:
    int orderId_;
    Date start_;
    int months_;
    std::vector<std::string> coveredArticles_;
};
