#pragma once
#include <vector>
#include "people/Employee.h"
#include "people/Specialization.h"

/** @brief Мастер: специализации, уровень 1-5 и задачи, над которыми он работает. */
class Technician : public Employee {
public:
    Technician(int id, const std::string& name, const ContactInfo& contacts, const Date& birth,
               const Date& hired, const Money& baseSalary, int level, int maxTasks);

    std::string getRole() const override;
    /** @brief Вывод сотрудника + уровень и загрузка. */
    void print(std::ostream& os) const override;
    /** @brief Оклад + 5% за каждый уровень + 15 руб. за каждую закрытую задачу. */
    Money calculateSalary(int month) const override;
    /** @brief Есть ли специализация под категорию устройства при текущем уровне. */
    bool canWorkOn(const Device& d) const override;

    /** @brief Добавить специализацию (без повторов). */
    void addSpecialization(const Specialization& s);
    /** @brief Взять задачу; false, если мастер загружен полностью или задача уже у него. */
    bool takeTask(int taskId);   // TODO(exc): TechnicianNotAvailableException
    /** @brief Закрыть задачу; каждые 20 закрытых задач — новый уровень. false, если такой задачи нет. */
    bool finishTask(int taskId);   // TODO(exc): RepairFirmException
    /** @brief Может ли взять ещё задачу. */
    bool isAvailable() const;
    /** @brief Загрузка от 0 до 1: активные задачи / максимум. */
    double workload() const;
    /** @brief Повысить уровень на 1, но не выше 5. */
    void upgradeLevel();

    int getLevel() const;
    int getCompletedTasks() const;
    int getMaxTasks() const;
    const std::vector<int>& getActiveTaskIds() const;
    const std::vector<Specialization>& getSpecializations() const;

private:
    std::vector<Specialization> specs_;
    int level_;
    std::vector<int> activeTaskIds_;
    int completedTasks_;
    int maxTasks_;
};
