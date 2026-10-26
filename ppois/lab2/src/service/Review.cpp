#include "service/Review.h"

Review::Review(int clientId, int orderId, int rating, const std::string& text)
    : clientId_(clientId), orderId_(orderId), rating_(rating), text_(text) {}

bool Review::validate() const {
    if (rating_ < 1 || rating_ > 5) return false;   // TODO(exc): InvalidReviewException(rating_)
    if (rating_ < 3 && text_.empty()) return false;   // TODO(exc): InvalidReviewException(rating_)
    return true;
}

bool Review::isPositive() const { return rating_ >= 4; }

int Review::getClientId() const { return clientId_; }
int Review::getOrderId() const { return orderId_; }
int Review::getRating() const { return rating_; }
const std::string& Review::getText() const { return text_; }

bool Review::operator==(const Review& other) const {
    return clientId_ == other.clientId_ && orderId_ == other.orderId_;
}

bool Review::operator!=(const Review& other) const { return !(*this == other); }

std::ostream& operator<<(std::ostream& os, const Review& r) {
    os << "Отзыв клиента #" << r.clientId_ << " о заказе №" << r.orderId_ << ": " << r.rating_ << "/5";
    if (!r.text_.empty()) os << " «" << r.text_ << "»";
    return os;
}
