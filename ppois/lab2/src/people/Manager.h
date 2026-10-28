#pragma once
#include <string>
#include <vector>
#include "people/Employee.h"

/** @brief Forward declaration: в заголовке нужны только как ссылки. */
class Technician;
class RepairOrder;
class Discount;

/** @brief Менеджер: распределяет задачи между мастерами, одобряет скидки и оценивает работу. */
class Manager : public Employee {
public:
    Manager(int id, const std::string& name, const ContactInfo& contacts, const Date& birth,
            const Date& hired, const Money& baseSalary, const std::string& department, double bonusPercent);

    std::string getRole() const override;
    /** @brief Вывод сотрудника + отдел и премия. */
    void print(std::ostream& os) const override;
    /** @brief Оклад + bonusPercent процентов. */
    Money calculateSalary(int month) const override;

    /**
     * @brief Назначить мастера на задачу заказа. Сначала все проверки, потом изменения:
     * задача есть, свободна и не закрыта (иначе RepairFirmException), мастер умеет работать
     * с устройством и не загружен (иначе TechnicianNotAvailableException).
     */
    void assignTechnician(RepairOrder& o, int taskId, Technician& t) const;
    /** @brief Одобрить скидку. */
    void approveDiscount(Discount& d) const;
    /**
     * @brief Оценка мастера 1-5: старт 3, +1 от 10 закрытых задач, ещё +1 от 30,
     * -1 за каждый просроченный заказ, где у мастера есть задача.
     */
    int evaluate(const Technician& t, const std::vector<RepairOrder>& orders, const Date& today) const;

    const std::string& getDepartment() const;
    double getBonusPercent() const;

private:
    std::string department_;
    double bonusPercent_;
};