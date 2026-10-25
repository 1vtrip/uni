#include "finance/ServiceItem.h"

ServiceItem::ServiceItem(const std::string& code, const std::string& name, const Money& price,
                         const DeviceCategory& category)
    : code_(code), name_(name), price_(price), category_(category) {}

Money ServiceItem::priceFor(bool urgent, double urgentMarkup) const {
    if (urgent) return price_ + price_.percent(urgentMarkup);
    return price_;
}

const std::string& ServiceItem::getCode() const { return code_; }
const std::string& ServiceItem::getName() const { return name_; }
const Money& ServiceItem::getPrice() const { return price_; }
const DeviceCategory& ServiceItem::getCategory() const { return category_; }
void ServiceItem::setPrice(const Money& price) { price_ = price; }

bool ServiceItem::operator==(const ServiceItem& other) const { return code_ == other.code_; }
bool ServiceItem::operator!=(const ServiceItem& other) const { return !(*this == other); }

std::ostream& operator<<(std::ostream& os, const ServiceItem& s) {
    return os << s.code_ << " " << s.name_ << " (" << s.category_.getName() << "): " << s.price_;
}
