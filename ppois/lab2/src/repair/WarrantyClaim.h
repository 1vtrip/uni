#pragma once
#include <string>
#include <iostream>
#include "core/Date.h"
#include "repair/Warranty.h"

/** @brief Гарантийная заявка: "Создана" -> "На рассмотрении" -> "Одобрена" или "Отклонена". */
class WarrantyClaim {
public:
    WarrantyClaim(const Warranty& warranty, const Date& date, const std::string& description);

    /**
     * @brief Подать заявку по запчасти article: из "Создана" в "На рассмотрении".
     * false, если заявка уже подана, гарантия не действует или запчасть не покрыта.
     */
    bool submit(const Date& today, const std::string& article);   // TODO(exc): WarrantyExpiredException
    /** @brief Одобрить; false, если заявка не на рассмотрении. */
    bool approve();   // TODO(exc): RepairFirmException
    /** @brief Отклонить с причиной; false, если заявка не на рассмотрении. */
    bool reject(const std::string& reason);   // TODO(exc): RepairFirmException
    /** @brief Рассмотрение закончено: заявка одобрена или отклонена. */
    bool isClosed() const;

    const Warranty& getWarranty() const;
    const Date& getDate() const;
    const std::string& getDescription() const;
    const std::string& getStatus() const;
    const std::string& getArticle() const;
    const std::string& getRejectReason() const;

    bool operator==(const WarrantyClaim& other) const;
    bool operator!=(const WarrantyClaim& other) const;
    friend std::ostream& operator<<(std::ostream& os, const WarrantyClaim& c);

private:
    Warranty warranty_;
    Date date_;
    std::string description_;
    std::string status_;
    std::string article_;
    std::string rejectReason_;
};
