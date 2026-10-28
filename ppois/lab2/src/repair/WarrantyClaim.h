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
     * Заявка уже подана -> RepairFirmException; гарантия не действует или запчасть не покрыта -> WarrantyExpiredException.
     */
    void submit(const Date& today, const std::string& article);
    /** @brief Одобрить; заявка не на рассмотрении -> RepairFirmException. */
    void approve();
    /** @brief Отклонить с причиной; заявка не на рассмотрении -> RepairFirmException. */
    void reject(const std::string& reason);
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
