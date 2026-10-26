#include "repair/WarrantyClaim.h"

WarrantyClaim::WarrantyClaim(const Warranty& warranty, const Date& date, const std::string& description)
    : warranty_(warranty), date_(date), description_(description), status_("Создана") {}

bool WarrantyClaim::submit(const Date& today, const std::string& article) {
    if (status_ != "Создана") return false;   // TODO(exc): RepairFirmException("Заявка уже подана")
    if (!warranty_.isValid(today) || !warranty_.covers(article))
        return false;   // TODO(exc): WarrantyExpiredException(warranty_.getOrderId())
    article_ = article;
    status_ = "На рассмотрении";
    return true;
}

bool WarrantyClaim::approve() {
    if (status_ != "На рассмотрении") return false;
    status_ = "Одобрена";
    return true;
}

bool WarrantyClaim::reject(const std::string& reason) {
    if (status_ != "На рассмотрении") return false;
    status_ = "Отклонена";
    rejectReason_ = reason;
    return true;
}

bool WarrantyClaim::isClosed() const { return status_ == "Одобрена" || status_ == "Отклонена"; }

const Warranty& WarrantyClaim::getWarranty() const { return warranty_; }
const Date& WarrantyClaim::getDate() const { return date_; }
const std::string& WarrantyClaim::getDescription() const { return description_; }
const std::string& WarrantyClaim::getStatus() const { return status_; }
const std::string& WarrantyClaim::getArticle() const { return article_; }
const std::string& WarrantyClaim::getRejectReason() const { return rejectReason_; }

bool WarrantyClaim::operator==(const WarrantyClaim& other) const {
    return warranty_ == other.warranty_ && date_ == other.date_ && description_ == other.description_;
}

bool WarrantyClaim::operator!=(const WarrantyClaim& other) const { return !(*this == other); }

std::ostream& operator<<(std::ostream& os, const WarrantyClaim& c) {
    os << "Гарантийная заявка по заказу №" << c.warranty_.getOrderId() << " от " << c.date_
       << ": " << c.description_ << " [" << c.status_ << "]";
    if (!c.article_.empty()) os << ", запчасть " << c.article_;
    if (!c.rejectReason_.empty()) os << ", причина: " << c.rejectReason_;
    return os;
}
