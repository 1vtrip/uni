#pragma once
#include <string>
#include <iostream>
#include "core/Date.h"

/** @brief Одно уведомление человеку: текст, канал ("SMS" / "Email") и дата отправки. */
class Notification {
public:
    Notification(int personId, const std::string& text, const std::string& channel, const Date& date);

    /** @brief Строка вида "[27.10.2026] SMS -> #7: текст". */
    std::string format() const;
    /** @brief Отправлено ли по e-mail. */
    bool isEmail() const;

    int getPersonId() const;
    const std::string& getText() const;
    const std::string& getChannel() const;
    const Date& getDate() const;

    bool operator==(const Notification& other) const;
    bool operator!=(const Notification& other) const;
    friend std::ostream& operator<<(std::ostream& os, const Notification& n);

private:
    int personId_;
    std::string text_;
    std::string channel_;
    Date date_;
};
