#pragma once
#include <string>
#include <vector>
#include "service/RepairFirm.h"
#include "devices/Smartphone.h"
#include "devices/Laptop.h"
#include "devices/WashingMachine.h"

/**
 * @brief Консольное меню фирмы. Весь ввод-вывод программы — здесь и в Input; модель ничего не печатает.
 * Меню не владеет фирмой (поле-ссылка), а работает с той, что создана в main.
 * Принятые за сессию устройства хранятся в трёх векторах по типам: указателей нет, а диагностике
 * нужен настоящий объект наследника, а не срезанная копия из заказа.
 */
class ConsoleMenu {
public:
    explicit ConsoleMenu(RepairFirm& firm);

    /** @brief Главный цикл. Ошибки любого пункта ловятся здесь: OutOfStock -> RepairFirmException -> std::exception. */
    void run();

private:
    void showMainMenu() const;
    /** @brief 1. Клиенты: список, добавить обычного или корпоративного, записать на приём. */
    void clientsMenu();
    /** @brief 2. Принять устройство: выбор типа -> объект наследника -> createOrder (полиморфизм). */
    void acceptDeviceDialog();
    /** @brief 3. Диагностика: настоящее устройство берётся из векторов меню по серийному номеру. */
    void diagnosisDialog();
    /** @brief 4. Ремонт: добавить работу, выдать запчасть, закрыть работу. */
    void repairMenu();
    /** @brief 5. Контроль качества. */
    void qualityDialog();
    /** @brief 6. Счёт, оплата, выдача, доставка. */
    void billingMenu();
    /** @brief 7. Заказы: все, просроченные, подробно. */
    void ordersMenu() const;
    /** @brief 8. Склад: остатки, дозаказ, приём поставки. */
    void warehouseMenu();
    /** @brief 9. Сотрудники и зарплаты (вывод через const Employee&). */
    void staffMenu() const;
    /** @brief 10. Отчёты: выручка, мастера, аттестация и калибровка, касса, уведомления, отзывы. */
    void reportsMenu() const;
    /** @brief 11. Сдвинуть «сегодня» на N дней. */
    void shiftDateDialog();
    /** @brief 12. Вызвать каждое из 17 исключений и вывести what(). Работает на копии фирмы. */
    void exceptionsDemo() const;

    /** @brief Показать заказы кратко (только незавершённые, если activeOnly) и спросить номер. */
    int chooseOrder(bool activeOnly) const;
    /** @brief Показать клиентов кратко и спросить ID. */
    int chooseClient() const;
    /** @brief Показать работы заказа и спросить номер работы. */
    int chooseTask(int orderId) const;
    /** @brief Настоящее устройство, принятое в этой сессии, по серийному номеру; нет -> RepairFirmException. */
    const Device& findDevice(const std::string& serial) const;
    /** @brief Есть ли уже принятое устройство с таким серийным номером. */
    bool hasDevice(const std::string& serial) const;

    RepairFirm& firm_;
    std::vector<Smartphone> phones_;
    std::vector<Laptop> laptops_;
    std::vector<WashingMachine> washers_;
};
