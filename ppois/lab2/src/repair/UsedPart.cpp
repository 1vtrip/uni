#include "repair/UsedPart.h"

UsedPart::UsedPart(const std::string& article, int qty, const Money& price)
    : article_(article), qty_(qty), price_(price) {}

Money UsedPart::sum() const { return price_ * qty_; }

const std::string& UsedPart::getArticle() const { return article_; }
int UsedPart::getQty() const { return qty_; }
const Money& UsedPart::getPrice() const { return price_; }

bool UsedPart::operator==(const UsedPart& other) const {
    return article_ == other.article_ && qty_ == other.qty_ && price_ == other.price_;
}

bool UsedPart::operator!=(const UsedPart& other) const { return !(*this == other); }

std::ostream& operator<<(std::ostream& os, const UsedPart& p) {
    return os << p.article_ << " x" << p.qty_ << " по " << p.price_ << " = " << p.sum();
}
