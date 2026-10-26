#include "repair/Warranty.h"

Warranty::Warranty(int orderId, const Date& start, int months)
    : orderId_(orderId), start_(start), months_(months) {}

void Warranty::addArticle(const std::string& article) {
    if (!covers(article)) coveredArticles_.push_back(article);
}

Date Warranty::endDate() const { return start_.addMonths(months_); }

bool Warranty::isValid(const Date& today) const {
    return !(today < start_) && !(endDate() < today);
}

bool Warranty::covers(const std::string& article) const {
    for (size_t i = 0; i < coveredArticles_.size(); ++i)
        if (coveredArticles_[i] == article) return true;
    return false;
}

bool Warranty::extend(int months) {
    if (months <= 0) return false;
    months_ += months;
    return true;
}

int Warranty::getOrderId() const { return orderId_; }
const Date& Warranty::getStart() const { return start_; }
int Warranty::getMonths() const { return months_; }
const std::vector<std::string>& Warranty::getCoveredArticles() const { return coveredArticles_; }

bool Warranty::operator==(const Warranty& other) const { return orderId_ == other.orderId_; }
bool Warranty::operator!=(const Warranty& other) const { return !(*this == other); }

std::ostream& operator<<(std::ostream& os, const Warranty& w) {
    os << "Гарантия по заказу №" << w.orderId_ << ": " << w.start_ << " - " << w.endDate()
       << " (" << w.months_ << " мес.), запчасти:";
    if (w.coveredArticles_.empty()) os << " нет";
    for (size_t i = 0; i < w.coveredArticles_.size(); ++i)
        os << (i == 0 ? " " : ", ") << w.coveredArticles_[i];
    return os;
}
