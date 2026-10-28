#include "repair/WarrantyClaim.h"
#include "exceptions/Exceptions.h"

WarrantyClaim::WarrantyClaim(const Warranty& warranty, const Date& date, const std::string& description)
    : warranty_(warranty), date_(date), description_(description), status_("Создана") {}

void WarrantyClaim::submit(const Date& today, const std::string& article) {
    if (status_ != "Создана") throw RepairFirmException("Заявка уже подана");
    if (!warranty_.isValid(today) || !warranty_.covers(article))
        throw WarrantyExpiredException(warranty_.getOrderId());
    article_ = article;
    status_ = "На рассмотрении";
}

void WarrantyClaim::approve() {
    if (status_ != "На рассмотрении") throw RepairFirmException("Заявка не на рассмотрении, а «" + status_ + "»");
    status_ = "Одобрена";
}

void WarrantyClaim::reject(const std::string& reason) {
    if (status_ != "На рассмотрении") throw RepairFirmException("Заявка не на рассмотрении, а «" + status_ + "»");
    status_ = "Отклонена";
    rejectReason_ = reason;
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
