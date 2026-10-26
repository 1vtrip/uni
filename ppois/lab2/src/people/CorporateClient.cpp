#include "people/CorporateClient.h"

CorporateClient::CorporateClient(int id, const std::string& name, const ContactInfo& contacts, const Date& birth,
                                 const Date& registered, const std::string& company, const std::string& taxId,
                                 const ServiceContract& contract)
    : Person(id, name, contacts, birth), Client(id, name, contacts, birth, registered), company_(company), taxId_(taxId), contract_(contract) {}

std::string CorporateClient::getRole() const { return "Корпоративный клиент"; }

void CorporateClient::print(std::ostream& os) const {
    Client::print(os);
    os << ", компания «" << company_ << "» (УНП " << taxId_ << "), договор " << contract_.getNumber();
}

Money CorporateClient::applyDiscount(const Money& sum) const {
    Money byContract = sum - contract_.discountFor(sum);
    Money byCard = Client::applyDiscount(sum);
    return byCard < byContract ? byCard : byContract;
}

int CorporateClient::maxActiveOrders() const { return 20; }

bool CorporateClient::isContractValid(const Date& today) const { return contract_.isActive(today); }

Date CorporateClient::slaDeadline(const Date& from) const { return contract_.deadline(from); }

bool CorporateClient::isTaxIdValid() const {
    if (taxId_.size() != 9) return false;
    for (size_t i = 0; i < taxId_.size(); ++i)
        if (taxId_[i] < '0' || taxId_[i] > '9') return false;
    return true;
}

const std::string& CorporateClient::getCompany() const { return company_; }
const std::string& CorporateClient::getTaxId() const { return taxId_; }
const ServiceContract& CorporateClient::getContract() const { return contract_; }
ServiceContract& CorporateClient::getContract() { return contract_; }
