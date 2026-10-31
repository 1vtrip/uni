#pragma once
#include <iostream>
#include <string>
#include "core/Timestamp.h"

/** @brief Уведомление пользователю: текст, канал ("system" или "email"), время и отметка о прочтении. */
class Notification {
public:
    /** @brief @throws InvalidOperationException пустой текст или канал не "system" и не "email". */
    Notification(int userId, const std::string& text, const Timestamp& time, const std::string& channel = "system");

    /** @brief "* [29.10.2026 10:15] (система) Квота почти заполнена"; звёздочка — не прочитано. */
    std::string format() const;
    void markRead();
    bool isRead() const;

    int getUserId() const;
    const std::string& getText() const;
    const std::string& getChannel() const;
    const Timestamp& getTime() const;

    /** @brief Своего ID нет: уведомления равны, если совпадают получатель, время и текст. */
    bool operator==(const Notification& o) const;
    bool operator!=(const Notification& o) const;
    /** @brief Выводит format(). */
    friend std::ostream& operator<<(std::ostream& os, const Notification& n);

private:
    int userId_;
    std::string text_;
    std::string channel_;
    Timestamp time_;
    bool read_;
};
