#include "finance/PriceList.h"

PriceList::PriceList(const Money& hourRate, double urgentMarkup)
    : hourRate_(hourRate), urgentMarkup_(urgentMarkup) {}

bool PriceList::addItem(const ServiceItem& item) {
    if (indexOf(item.getCode()) != -1) return false;
    items_.push_back(item);
    return true;
}

int PriceList::indexOf(const std::string& code) const {
    for (size_t i = 0; i < items_.size(); ++i)
        if (items_[i].getCode() == code) return (int)i;
    return -1;
}

const ServiceItem& PriceList::itemAt(int index) const { return items_[index]; }

Money PriceList::laborCost(double hours, bool urgent) const {
    Money cost = hourRate_ * hours;
    if (urgent) cost += cost.percent(urgentMarkup_);
    return cost;
}

/** @brief Цены меняются прямо в items_[i]: копия элемента прайс бы не изменила. */
void PriceList::raise(double percent) {
    hourRate_ += hourRate_.percent(percent);
    for (size_t i = 0; i < items_.size(); ++i) {
        Money price = items_[i].getPrice();
        items_[i].setPrice(price + price.percent(percent));
    }
}

const Money& PriceList::getHourRate() const { return hourRate_; }
double PriceList::getUrgentMarkup() const { return urgentMarkup_; }
const std::vector<ServiceItem>& PriceList::getItems() const { return items_; }

std::ostream& operator<<(std::ostream& os, const PriceList& p) {
    os << "Прайс-лист: ставка " << p.hourRate_ << ", срочность +" << p.urgentMarkup_ << "%";
    for (size_t i = 0; i < p.items_.size(); ++i)
        os << "\n  " << p.items_[i];
    return os;
}
