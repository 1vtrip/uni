#pragma once
#include <string>
#include "people/Client.h"
#include "people/ServiceContract.h"

/** @brief Корпоративный клиент: компания с договором на обслуживание. Конец иерархии (final). */
class CorporateClient final : public Client {
public:
    CorporateClient(int id, const std::string& name, const ContactInfo& contacts, const Date& birth,
                    const Date& registered, const std::string& company, const std::string& taxId,
                    const ServiceContract& contract);

    std::string getRole() const override;
    /** @brief Базовый вывод клиента + компания и номер договора. */
    void print(std::ostream& os) const override;
    /** @brief Большая из двух скидок: по договору или по карте. Срок договора здесь не проверяется (нет даты). */
    Money applyDiscount(const Money& sum) const override;
    /** @brief У компании лимит незавершённых заказов 20. */
    int maxActiveOrders() const override;

    /** @brief Действует ли договор в день today. */
    bool isContractValid(const Date& today) const;
    /** @brief Срок ремонта по договору для заказа, принятого в день from. */
    Date slaDeadline(const Date& from) const;
    /** @brief УНП: ровно 9 цифр. */
    bool isTaxIdValid() const;

    const std::string& getCompany() const;
    const std::string& getTaxId() const;
    const ServiceContract& getContract() const;
    ServiceContract& getContract();

private:
    std::string company_;
    std::string taxId_;
    ServiceContract contract_;
};
