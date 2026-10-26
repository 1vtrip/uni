#pragma once
#include <string>
#include <iostream>

/** @brief Отзыв клиента о выполненном заказе: оценка 1-5 и текст. */
class Review {
public:
    Review(int clientId, int orderId, int rating, const std::string& text);

    /** @brief Корректен ли отзыв: оценка 1-5, а при оценке ниже 3 текст обязателен. */
    bool validate() const;   // TODO(exc): InvalidReviewException, метод станет void
    /** @brief Положительный отзыв: оценка 4 или 5. */
    bool isPositive() const;

    int getClientId() const;
    int getOrderId() const;
    int getRating() const;
    const std::string& getText() const;

    /** @brief Отзывы равны, если это отзыв одного клиента об одном заказе. */
    bool operator==(const Review& other) const;
    bool operator!=(const Review& other) const;
    friend std::ostream& operator<<(std::ostream& os, const Review& r);

private:
    int clientId_;
    int orderId_;
    int rating_;
    std::string text_;
};
