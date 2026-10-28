#pragma once
#include <string>
#include <vector>
#include "people/Employee.h"

/**
 * @brief Курьер: забирает и доставляет технику. Хранит только ID доставок,
 * чтобы не было циклической зависимости с Delivery (она сама хранит id курьера).
 */
class Courier : public Employee {
public:
    Courier(int id, const std::string& name, const ContactInfo& contacts, const Date& birth,
            const Date& hired, const Money& baseSalary, const std::string& carNumber);

    std::string getRole() const override;
    /** @brief Вывод сотрудника + машина, доставки в пути и выполненные. */
    void print(std::ostream& os) const override;
    /** @brief Оклад + 5 руб. за каждую выполненную доставку. */
    Money calculateSalary(int month) const override;

    /** @brief Забрать доставку; false, если она уже у курьера. */
    bool pickUp(int deliveryId);
    /** @brief Доставить; такой доставки у курьера нет -> RepairFirmException. */
    void deliver(int deliveryId);
    /** @brief Сколько доставок сейчас в пути. */
    int inTransit() const;

    const std::string& getCarNumber() const;
    const std::vector<int>& getDeliveryIds() const;
    int getDeliveredCount() const;

private:
    std::string carNumber_;
    std::vector<int> deliveryIds_;
    int deliveredCount_;
};
