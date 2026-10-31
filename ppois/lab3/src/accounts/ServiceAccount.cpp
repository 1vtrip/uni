#include "accounts/ServiceAccount.h"
#include "core/Checksum.h"
#include "exceptions/Exceptions.h"

ServiceAccount::ServiceAccount(int id, const std::string& login, const std::string& displayName,
                               const Credentials& credentials, const Timestamp& created,
                               const std::string& serviceName)
    : Account(id, login, displayName, credentials, created),
      serviceName_(serviceName), allowedOperations_(), apiKey_() {
    if (serviceName.empty()) throw InvalidOperationException("имя сервиса не может быть пустым");
    rotateKey(id);
}

std::string ServiceAccount::role() const { return "Служебная учётная запись"; }

void ServiceAccount::print(std::ostream& os) const {
    Account::print(os);
    os << ", сервис " << serviceName_ << ", операции: ";
    if (allowedOperations_.empty()) os << "нет";
    for (size_t i = 0; i < allowedOperations_.size(); ++i) {
        if (i > 0) os << ", ";
        os << allowedOperations_[i];
    }
}

int ServiceAccount::maxSessionMinutes() const { return 1440; }
bool ServiceAccount::canReadAnyFile() const { return canPerform("backup"); }

bool ServiceAccount::canPerform(const std::string& op) const {
    for (size_t i = 0; i < allowedOperations_.size(); ++i)
        if (allowedOperations_[i] == op) return true;
    return false;
}

void ServiceAccount::allow(const std::string& op) {
    if (op.empty()) throw InvalidOperationException("имя операции не может быть пустым");
    if (!canPerform(op)) allowedOperations_.push_back(op);
}

bool ServiceAccount::disallow(const std::string& op) {
    for (size_t i = 0; i < allowedOperations_.size(); ++i) {
        if (allowedOperations_[i] == op) {
            allowedOperations_.erase(allowedOperations_.begin() + i);
            return true;
        }
    }
    return false;
}

void ServiceAccount::rotateKey(int seed) {
    apiKey_ = Checksum::of(serviceName_ + apiKey_ + std::to_string(seed)).toHex();
}

bool ServiceAccount::verifyKey(const std::string& key) const { return !key.empty() && key == apiKey_; }

const std::string& ServiceAccount::getServiceName() const { return serviceName_; }
const std::vector<std::string>& ServiceAccount::getAllowedOperations() const { return allowedOperations_; }
const std::string& ServiceAccount::getApiKey() const { return apiKey_; }
