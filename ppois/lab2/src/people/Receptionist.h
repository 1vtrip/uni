#pragma once
#include <string>
#include "people/Employee.h"
#include "repair/RepairOrder.h"

/** @brief Forward declaration: в заголовке нужны только как ссылки. */
class Client;
class Invoice;

/** @brief Приёмщик: принимает устройства в ремонт и выдаёт их клиентам. */
class Receptionist : public Employee {
public:
    Receptionist(int id, const std::string& name, const ContactInfo& contacts, const Date& birth,
                 const Date& hired, const Money& baseSalary);

    std::string getRole() const override;
    /** @brief Вывод сотрудника + сколько устройств принято. */
    void print(std::ostream& os) const override;
    /** @brief Оклад + 2 руб. за каждую приёмку. */
    Money calculateSalary(int month) const override;

    /**
     * @brief Принять устройство: создать заказ. Срок и цену диагностики заказ считает сам,
     * пока device ещё ссылается на наследника. Лимит заказов клиента проверяет RepairFirm.
     */
    RepairOrder acceptDevice(int orderId, const Client& c, const Device& d, const std::string& problem,
                             const Date& today, bool urgent);
    /** @brief Выдать устройство. Счёт не от этого заказа или не оплачен -> RepairFirmException; заказ не готов -> InvalidStatusTransitionException. */
    void issueDevice(RepairOrder& o, const Invoice& inv) const;

    int getAcceptedCount() const;

private:
    int acceptedCount_;
};
