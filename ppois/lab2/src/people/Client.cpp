#include "people/Client.h"
#include "repair/RepairOrder.h"
#include "finance/Invoice.h"

Client::Client(int id, const std::string& name, const ContactInfo& contacts, const Date& birth,
               const Date& registered)
    : Person(id, name, contacts, birth), registered_(registered),
      card_("LC-" + std::to_string(id)) {}

std::string Client::getRole() const { return "Клиент"; }

void Client::print(std::ostream& os) const {
    Person::print(os);
    os << ", карта " << card_.getLevel() << " (" << card_.getPoints() << " баллов), заказов " << orderIds_.size();
}

Money Client::applyDiscount(const Money& sum) const {
    return sum - sum.percent(card_.discountPercent());
}

int Client::maxActiveOrders() const { return 3; }

void Client::addOrder(int orderId) {
    for (size_t i = 0; i < orderIds_.size(); ++i)
        if (orderIds_[i] == orderId) return;
    orderIds_.push_back(orderId);
}

int Client::activeOrders(const std::vector<RepairOrder>& all) const {
    int n = 0;
    for (size_t i = 0; i < all.size(); ++i)
        if (all[i].getClientId() == id_ && !all[i].getStatus().isFinal()) ++n;
    return n;
}

bool Client::canPlaceOrder(const std::vector<RepairOrder>& all) const {
    return activeOrders(all) < maxActiveOrders();
}

Money Client::totalSpent(const std::vector<Invoice>& all) const {
    Money sum;
    for (size_t i = 0; i < all.size(); ++i)
        if (all[i].getClientId() == id_ && all[i].isPaid()) sum += all[i].total();
    return sum;
}

const Date& Client::getRegistered() const { return registered_; }
LoyaltyCard& Client::getCard() { return card_; }
const LoyaltyCard& Client::getCard() const { return card_; }
const std::vector<int>& Client::getOrderIds() const { return orderIds_; }
