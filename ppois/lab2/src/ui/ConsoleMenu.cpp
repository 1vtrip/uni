#include "ui/ConsoleMenu.h"
#include <iostream>
#include "ui/Input.h"
#include "ui/DemoData.h"
#include "service/ReportGenerator.h"
#include "repair/QualityCheck.h"
#include "repair/WarrantyClaim.h"
#include "exceptions/Exceptions.h"

using std::cout;
using S = OrderStatus::Status;

/** @brief Отчёт за текущий месяц «сегодняшней» даты фирмы. */
static ReportGenerator currentMonth(const Date& today) {
    return ReportGenerator(Date(1, today.getMonth(), today.getYear()),
                           Date(Date::daysInMonth(today.getMonth(), today.getYear()), today.getMonth(), today.getYear()));
}

/** @brief Одна строка о сотруднике и его зарплате. Принимает базовый класс: формула зарплаты — наследника. */
static void printEmployee(const Employee& e, int month) {
    cout << "  " << e << "\n      зарплата за месяц: " << e.calculateSalary(month) << "\n";
}

/** @brief Строка о результате одного исключения в демонстрации. */
static void showCaught(int n, const std::string& type, const std::exception& e) {
    cout << "  " << (n < 10 ? " " : "") << n << ". " << type << "\n      " << e.what() << "\n";
}

ConsoleMenu::ConsoleMenu(RepairFirm& firm) : firm_(firm) {}

/** @brief Номер пункта читается вне try: если ввод закрыт, исключение выходит из run() и программа завершается. */
void ConsoleMenu::run() {
    while (true) {
        showMainMenu();
        int choice = readInt("Выбор: ", 0, 12);
        if (choice == 0) break;
        try {
            switch (choice) {
                case 1:  clientsMenu(); break;
                case 2:  acceptDeviceDialog(); break;
                case 3:  diagnosisDialog(); break;
                case 4:  repairMenu(); break;
                case 5:  qualityDialog(); break;
                case 6:  billingMenu(); break;
                case 7:  ordersMenu(); break;
                case 8:  warehouseMenu(); break;
                case 9:  staffMenu(); break;
                case 10: reportsMenu(); break;
                case 11: shiftDateDialog(); break;
                case 12: exceptionsDemo(); break;
            }
        } catch (const OutOfStockException& e) {
            cout << "[Склад] " << e.what() << "\nОформите поставку: пункт 8 -> 3.\n";
        } catch (const RepairFirmException& e) {
            cout << "[Ошибка] " << e.what() << "\n";
        } catch (const std::exception& e) {
            cout << "[Системная ошибка] " << e.what() << "\n";
        }
    }
}

void ConsoleMenu::showMainMenu() const {
    cout << "\n=== " << firm_.getName() << " — сегодня " << firm_.today() << " ===\n"
         << " 1. Клиенты                 7. Заказы\n"
         << " 2. Принять устройство      8. Склад\n"
         << " 3. Диагностика             9. Сотрудники и зарплаты\n"
         << " 4. Ремонт                 10. Отчёты\n"
         << " 5. Контроль качества      11. Сдвинуть дату\n"
         << " 6. Счёт, оплата, выдача   12. Демонстрация исключений\n"
         << " 0. Выход\n";
}

int ConsoleMenu::chooseClient() const {
    cout << "Клиенты:\n";
    for (size_t i = 0; i < firm_.getClients().size(); ++i)
        cout << "  #" << firm_.getClients()[i].getId() << " " << firm_.getClients()[i].getName() << "\n";
    for (size_t i = 0; i < firm_.getCorporateClients().size(); ++i)
        cout << "  #" << firm_.getCorporateClients()[i].getId() << " " << firm_.getCorporateClients()[i].getName()
             << " («" << firm_.getCorporateClients()[i].getCompany() << "»)\n";
    for (size_t i = 0; i < firm_.getEmployeeClients().size(); ++i)
        cout << "  #" << firm_.getEmployeeClients()[i].getId() << " " << firm_.getEmployeeClients()[i].getName() << " (сотрудник)\n";
    int id = readInt("ID клиента: ", 1, 1000000);
    firm_.findClient(id);
    return id;
}

int ConsoleMenu::chooseOrder(bool activeOnly) const {
    cout << "Заказы:\n";
    int shown = 0;
    for (size_t i = 0; i < firm_.getOrders().size(); ++i) {
        const RepairOrder& o = firm_.getOrders()[i];
        if (activeOnly && o.getStatus().isFinal()) continue;
        cout << "  №" << o.getId() << " [" << o.getStatus() << "] " << o.getDeviceInfo() << "\n";
        ++shown;
    }
    if (shown == 0) throw RepairFirmException("Подходящих заказов нет");
    int id = readInt("Номер заказа: ", 1, 1000000);
    firm_.findOrder(id);
    return id;
}

int ConsoleMenu::chooseTask(int orderId) const {
    const std::vector<RepairTask>& tasks = firm_.findOrder(orderId).getTasks();
    if (tasks.empty()) throw RepairFirmException("В заказе №" + std::to_string(orderId) + " ещё нет работ");
    cout << "Работы заказа:\n";
    for (size_t i = 0; i < tasks.size(); ++i) cout << "  " << tasks[i] << "\n";
    return readInt("Номер работы: ", 1, 1000000);
}

bool ConsoleMenu::hasDevice(const std::string& serial) const {
    for (size_t i = 0; i < phones_.size(); ++i)   if (phones_[i].getSerial() == serial) return true;
    for (size_t i = 0; i < laptops_.size(); ++i)  if (laptops_[i].getSerial() == serial) return true;
    for (size_t i = 0; i < washers_.size(); ++i)  if (washers_[i].getSerial() == serial) return true;
    return false;
}

/** @brief Возвращается ссылка на базу Device, но объект — настоящий наследник: виртуальные вызовы работают. */
const Device& ConsoleMenu::findDevice(const std::string& serial) const {
    for (size_t i = 0; i < phones_.size(); ++i)   if (phones_[i].getSerial() == serial) return phones_[i];
    for (size_t i = 0; i < laptops_.size(); ++i)  if (laptops_[i].getSerial() == serial) return laptops_[i];
    for (size_t i = 0; i < washers_.size(); ++i)  if (washers_[i].getSerial() == serial) return washers_[i];
    throw RepairFirmException("Устройство " + serial + " принято не в этой сессии меню (заказы из демо уже продиагностированы)");
}

void ConsoleMenu::clientsMenu() {
    cout << "1. Список  2. Добавить клиента  3. Добавить компанию  4. Записать на приём  0. Назад\n";
    int c = readInt("Выбор: ", 0, 4);
    if (c == 1) {
        for (size_t i = 0; i < firm_.getClients().size(); ++i) cout << "  " << firm_.getClients()[i] << "\n";
        for (size_t i = 0; i < firm_.getCorporateClients().size(); ++i) cout << "  " << firm_.getCorporateClients()[i] << "\n";
        for (size_t i = 0; i < firm_.getEmployeeClients().size(); ++i) cout << "  " << firm_.getEmployeeClients()[i] << "\n";
    } else if (c == 2 || c == 3) {
        std::string name = readLine("ФИО: ");
        std::string phone = readLine("Телефон (+375XXXXXXXXX): ");
        std::string email = readLine("E-mail: ");
        std::string city = readLine("Город: ");
        std::string street = readLine("Улица: ");
        std::string house = readLine("Дом: ");
        std::string flat = readLine("Квартира (Enter — нет): ", true);
        Date birth = readDate("Дата рождения (ДД.ММ.ГГГГ): ");
        ContactInfo contacts(phone, email, Address(city, street, house, flat));
        int id;
        if (c == 2) {
            id = firm_.addClient(name, contacts, birth);
        } else {
            std::string company = readLine("Компания: ");
            std::string taxId = readLine("УНП (9 цифр): ");
            std::string number = readLine("Номер договора: ");
            double discount = readDouble("Скидка по договору, %: ", 0, 50);
            int sla = readInt("Срок ремонта по договору, дней: ", 1, 30);
            int months = readInt("Договор на сколько месяцев: ", 1, 60);
            Date today = firm_.today();
            id = firm_.addCorporateClient(name, contacts, birth, company, taxId,
                                          ServiceContract(number, today, today.addMonths(months), discount, sla));
        }
        cout << "Добавлен: " << firm_.findClient(id) << "\n";
    } else if (c == 4) {
        int client = chooseClient();
        Date date = readDate("Дата (ДД.ММ.ГГГГ): ");
        int hour = readInt("Час (9-18): ", 0, 23);
        std::string purpose = readLine("Цель визита: ");
        int id = firm_.bookAppointment(client, date, hour, purpose);
        cout << "Записано: запись №" << id << ", клиенту отправлено напоминание.\n";
    }
}

/**
 * @brief Создаётся настоящий объект нужного наследника и передаётся в createOrder как const Device&:
 * срок и цена диагностики считаются его виртуальными методами. После успеха объект сохраняется
 * в векторе своего типа — для диагностики.
 */
void ConsoleMenu::acceptDeviceDialog() {
    int client = chooseClient();
    cout << "Тип: 1. Смартфон  2. Ноутбук  3. Стиральная машина\n";
    int type = readInt("Выбор: ", 1, 3);
    std::string serial = readLine("Серийный номер: ");
    if (hasDevice(serial)) throw RepairFirmException("Устройство " + serial + " уже принято");
    std::string model = readLine("Модель: ");
    std::string brandName = readLine("Бренд: ");
    bool authorized = readYesNo("Фирма — авторизованный сервис этого бренда? (д/н): ");
    Brand brand(brandName, "—", authorized);
    Date purchased = readDate("Дата покупки (ДД.ММ.ГГГГ): ");
    std::string problem = readLine("Что случилось: ");
    bool urgent = readYesNo("Срочно? (д/н): ");
    int id;
    if (type == 1) {
        std::string imei = readLine("IMEI (15 цифр, например 356938035643809): ");
        double screen = readDouble("Диагональ экрана, дюймов: ", 3, 10);
        bool wet = readYesNo("Был в воде? (д/н): ");
        Smartphone d(serial, model, brand, phoneCategory(), purchased, imei, screen, wet);
        id = firm_.createOrder(client, d, problem, urgent);
        phones_.push_back(d);
    } else if (type == 2) {
        std::string cpu = readLine("Процессор: ");
        int ram = readInt("Память, ГБ: ", 1, 256);
        int disk = readInt("Диск: 1. HDD  2. SSD: ", 1, 2);
        Laptop d(serial, model, brand, laptopCategory(), purchased, cpu, ram, disk == 1 ? "HDD" : "SSD");
        id = firm_.createOrder(client, d, problem, urgent);
        laptops_.push_back(d);
    } else {
        int power = readInt("Мощность, Вт: ", 100, 10000);
        double weight = readDouble("Вес, кг: ", 1, 200);
        bool builtIn = readYesNo("Встраиваемая? (д/н): ");
        double load = readDouble("Загрузка, кг: ", 1, 20);
        int drum = readInt("Барабан: 1. Исправен  2. Повреждён: ", 1, 2);
        WashingMachine d(serial, model, brand, applianceCategory(), purchased, power, weight, builtIn,
                         load, drum == 1 ? "Исправен" : "Повреждён");
        id = firm_.createOrder(client, d, problem, urgent);
        washers_.push_back(d);
    }
    const RepairOrder& o = firm_.findOrder(id);
    cout << "Принято:\n" << o << "\n  Диагностика: " << o.getDiagnosticPrice() << ", срок " << o.getDeadline() << "\n";
}

void ConsoleMenu::diagnosisDialog() {
    int id = chooseOrder(true);
    const Device& d = findDevice(firm_.findOrder(id).getDevice().getSerial());
    try {
        firm_.runDiagnosis(id, d);
    } catch (const DeviceNotRepairableException&) {
        cout << firm_.findOrder(id).getDiagnosis() << "\n";
        throw;
    }
    cout << firm_.findOrder(id).getDiagnosis() << "\n  Предварительно за работу: "
         << firm_.findOrder(id).getDiagnosis().estimate(firm_.getPriceList()) << "\n";
}

void ConsoleMenu::repairMenu() {
    cout << "1. Добавить работу  2. Выдать запчасть  3. Закрыть работу  0. Назад\n";
    int c = readInt("Выбор: ", 0, 3);
    if (c == 0) return;
    int id = chooseOrder(true);
    if (c == 1) {
        int task = firm_.addTask(id, readLine("Описание работы: "));
        cout << "Добавлено: " << firm_.findOrder(id).findTask(task) << "\n";
    } else if (c == 2) {
        int task = chooseTask(id);
        cout << firm_.getWarehouse() << "\n";
        std::string article = readLine("Артикул: ");
        int qty = readInt("Количество: ", 1, 100);
        firm_.issuePart(id, task, article, qty);
        cout << "Выдано: " << firm_.findOrder(id).findTask(task) << "\n";
    } else {
        int task = chooseTask(id);
        firm_.finishTask(id, task, readDouble("Сколько часов заняла работа: ", 0.1, 100));
        cout << "Закрыто. Статус заказа: " << firm_.findOrder(id).getStatus() << "\n";
    }
}

void ConsoleMenu::qualityDialog() {
    int id = chooseOrder(true);
    bool ok = readYesNo("Неисправность устранена? (д/н): ");
    firm_.qualityCheck(id, ok);
    cout << "Проверка пройдена, заказ готов к выдаче. Клиенту отправлено уведомление.\n";
}

void ConsoleMenu::billingMenu() {
    cout << "1. Выставить счёт  2. Оплатить  3. Выдать клиенту  4. Отправить курьером  5. Доставка выполнена  0. Назад\n";
    int c = readInt("Выбор: ", 0, 5);
    if (c == 1) {
        int id = chooseOrder(true);
        cout << firm_.findInvoice(firm_.billOrder(id)) << "\n";
    } else if (c == 2) {
        cout << "Счета:\n";
        for (size_t i = 0; i < firm_.getInvoices().size(); ++i) cout << firm_.getInvoices()[i] << "\n";
        int inv = readInt("Номер счёта: ", 1, 1000000);
        const Invoice& invoice = firm_.findInvoice(inv);
        cout << "К оплате: " << invoice.amountDue() << "\n";
        Money sum = Money::fromRubles(readDouble("Внесено, руб.: ", 0, 1000000));
        cout << "Способ: 1. Наличные  2. Карта (2.5%)  3. Перевод (1%)\n";
        int m = readInt("Выбор: ", 1, 3);
        PaymentMethod method(m == 1 ? "Наличные" : (m == 2 ? "Карта" : "Перевод"), m == 1 ? 0 : (m == 2 ? 2.5 : 1));
        Money due = invoice.total();
        firm_.payInvoice(inv, sum, method);
        cout << "Оплачено. Сдача: " << (sum - due) << ". Касса: " << firm_.getCash().getBalance() << "\n";
    } else if (c == 3) {
        int id = chooseOrder(true);
        firm_.issueToClient(id);
        cout << "Выдано. " << firm_.getWarranties().back() << "\n";
    } else if (c == 4) {
        int id = chooseOrder(false);
        std::string city = readLine("Город: ");
        std::string street = readLine("Улица: ");
        std::string house = readLine("Дом: ");
        int dl = firm_.arrangeDelivery(id, Address(city, street, house, readLine("Квартира (Enter — нет): ", true)));
        cout << firm_.getDeliveries().back() << "\n";
        (void)dl;
    } else if (c == 5) {
        for (size_t i = 0; i < firm_.getDeliveries().size(); ++i) cout << "  " << firm_.getDeliveries()[i] << "\n";
        firm_.completeDelivery(readInt("Номер доставки: ", 1, 1000000));
        cout << "Доставка отмечена выполненной.\n";
    }
}

void ConsoleMenu::ordersMenu() const {
    cout << "1. Все заказы  2. Просроченные  3. Подробно о заказе  0. Назад\n";
    int c = readInt("Выбор: ", 0, 3);
    if (c == 1) {
        for (size_t i = 0; i < firm_.getOrders().size(); ++i) cout << firm_.getOrders()[i] << "\n";
    } else if (c == 2) {
        std::vector<RepairOrder> late = firm_.overdueOrders();
        if (late.empty()) cout << "Просроченных заказов нет.\n";
        for (size_t i = 0; i < late.size(); ++i)
            cout << late[i] << "\n  Просрочен на " << -late[i].daysLeft(firm_.today()) << " дн.\n";
    } else if (c == 3) {
        const RepairOrder& o = firm_.findOrder(chooseOrder(false));
        cout << o << "\n" << o.getDiagnosis() << "\n  Стоимость сейчас: " << o.totalCost(firm_.getPriceList()) << "\n";
    }
}

void ConsoleMenu::warehouseMenu() {
    cout << "1. Остатки  2. Что дозаказать  3. Принять поставку  0. Назад\n";
    int c = readInt("Выбор: ", 0, 3);
    if (c == 1) {
        cout << firm_.getWarehouse() << "\n";
    } else if (c == 2) {
        cout << currentMonth(firm_.today()).stockReport(firm_.getWarehouse()) << "\n";
    } else if (c == 3) {
        cout << firm_.getWarehouse() << "\n";
        std::string article = readLine("Артикул: ");
        int qty = readInt("Количество: ", 1, 1000);
        PurchaseOrder po(firm_.newId(), 0, firm_.today());
        po.addLine(firm_.getWarehouse().find(article).getPart(), qty);
        firm_.receivePurchase(po);
        cout << "Принято: " << firm_.getWarehouse().find(article) << "\n";
    }
}

void ConsoleMenu::staffMenu() const {
    int month = firm_.today().getMonth();
    cout << "Сотрудники (зарплата через const Employee& — у каждого своя формула):\n";
    for (size_t i = 0; i < firm_.getTechnicians().size(); ++i) printEmployee(firm_.getTechnicians()[i], month);
    for (size_t i = 0; i < firm_.getReceptionists().size(); ++i) printEmployee(firm_.getReceptionists()[i], month);
    for (size_t i = 0; i < firm_.getManagers().size(); ++i) printEmployee(firm_.getManagers()[i], month);
    for (size_t i = 0; i < firm_.getAccountants().size(); ++i) printEmployee(firm_.getAccountants()[i], month);
    for (size_t i = 0; i < firm_.getCouriers().size(); ++i) printEmployee(firm_.getCouriers()[i], month);
    for (size_t i = 0; i < firm_.getEmployeeClients().size(); ++i) printEmployee(firm_.getEmployeeClients()[i], month);
    cout << "Фонд зарплаты: " << firm_.salaryFund(month) << "\n";
}

void ConsoleMenu::reportsMenu() const {
    cout << "1. Выручка  2. Мастера  3. Аттестация и калибровка  4. Касса  5. Уведомления  6. Отзывы  0. Назад\n";
    int c = readInt("Выбор: ", 0, 6);
    ReportGenerator r = currentMonth(firm_.today());
    if (c == 1) {
        int month = readInt("Месяц: ", 1, 12);
        int year = readInt("Год: ", 2000, 2100);
        cout << "Выручка: " << firm_.monthRevenue(month, year) << "\n";
    } else if (c == 2) {
        cout << r << "\n";
        for (size_t i = 0; i < firm_.getTechnicians().size(); ++i)
            cout << "  " << r.technicianStats(firm_.getTechnicians()[i], firm_.getOrders()) << "\n";
        cout << "  Просрочено заказов: " << r.overdueCount(firm_.getOrders(), firm_.today()) << "\n";
    } else if (c == 3) {
        cout << "Одна функция checkStatus(const ICertifiable&) и для мастеров, и для инструментов:\n";
        for (size_t i = 0; i < firm_.getTechnicians().size(); ++i)
            cout << "  мастер " << firm_.getTechnicians()[i].getName() << ": "
                 << r.checkStatus(firm_.getTechnicians()[i], firm_.today()) << "\n";
        for (size_t i = 0; i < firm_.getWorkshop().getTools().size(); ++i)
            cout << "  инструмент " << firm_.getWorkshop().getTools()[i].getName() << ": "
                 << r.checkStatus(firm_.getWorkshop().getTools()[i], firm_.today()) << "\n";
    } else if (c == 4) {
        cout << firm_.getCash().report() << "\n";
        cout << "Не оплачено по счетам (unpaid(const IPayable&)):\n";
        for (size_t i = 0; i < firm_.getInvoices().size(); ++i)
            cout << "  счёт №" << firm_.getInvoices()[i].getId() << ": " << r.unpaid(firm_.getInvoices()[i]) << "\n";
    } else if (c == 5) {
        const std::vector<Notification>& h = firm_.getNotifier().history();
        size_t from = h.size() > 15 ? h.size() - 15 : 0;
        cout << "Последние уведомления (" << h.size() << " всего):\n";
        for (size_t i = from; i < h.size(); ++i) cout << "  " << h[i] << "\n";
    } else if (c == 6) {
        if (firm_.getReviews().empty()) cout << "Отзывов пока нет.\n";
        for (size_t i = 0; i < firm_.getReviews().size(); ++i) cout << "  " << firm_.getReviews()[i] << "\n";
    }
}

void ConsoleMenu::shiftDateDialog() {
    firm_.advanceDays(readInt("На сколько дней вперёд: ", 1, 365));
    cout << "Сегодня " << firm_.today() << ". Просрочено заказов: " << firm_.overdueOrders().size() << "\n";
}

/**
 * @brief Каждое исключение — свой try и catch по точному типу (по const&, без срезки).
 * Действия с фирмой идут на копии demo, поэтому настоящие данные не меняются.
 */
void ConsoleMenu::exceptionsDemo() const {
    RepairFirm demo = firm_;
    Date today = firm_.today();
    ContactInfo staff("+375291111111", "staff@firm.by", Address("Минск", "Притыцкого", "20"));
    cout << "Демонстрация всех 17 исключений:\n";

    try { demo.findInvoice(999); }
    catch (const RepairFirmException& e) { showCaught(1, "RepairFirmException (база)", e); }

    try { demo.addClient("Тест", ContactInfo("12345", "test@mail.ru", Address("Минск", "A", "1")), Date(1, 1, 1990)); }
    catch (const InvalidClientDataException& e) { showCaught(2, "InvalidClientDataException", e); }

    try { demo.findClient(999); }
    catch (const ClientNotFoundException& e) { showCaught(3, "ClientNotFoundException", e); }

    try { demo.findOrder(999); }
    catch (const OrderNotFoundException& e) { showCaught(4, "OrderNotFoundException", e); }

    try { OrderStatus s(S::Issued); s.moveTo(S::InRepair); }
    catch (const InvalidStatusTransitionException& e) { showCaught(5, "InvalidStatusTransitionException", e); }

    Smartphone phone("DEMO-1", "Galaxy S24", Brand("Samsung", "Корея", true), phoneCategory(), Date(1, 3, 2026),
                     "490154203237518", 6.2, true);
    try {
        RepairOrder o(1, 1, phone, "демо", today, false);
        Diagnosis dead(1, today);
        dead.addDefect(Defect("WATER", "Влага", 5));
        dead.addDefect(Defect("BOARD", "Плата", 5));
        dead.addDefect(Defect("SCREEN", "Экран", 4));
        dead.decideRepairable();
        o.setStatus(S::Diagnostics);
        o.setDiagnosis(dead);
    } catch (const DeviceNotRepairableException& e) { showCaught(6, "DeviceNotRepairableException", e); }

    try {
        RepairTask t(1, "демо");
        t.start(1);
        t.addPart(demo.getWarehouse().find("SSD-512").getPart(), 1, phone);
    } catch (const IncompatiblePartException& e) { showCaught(7, "IncompatiblePartException", e); }

    try { Warehouse w = demo.getWarehouse(); w.take("BAT-S24", 999); }
    catch (const OutOfStockException& e) { showCaught(8, "OutOfStockException", e); }

    try {
        Technician t(1, "Тест", staff, Date(1, 1, 1990), Date(1, 1, 2020), Money::fromRubles(1000), 1, 1);
        t.takeTask(1);
        t.takeTask(2);
    } catch (const TechnicianNotAvailableException& e) { showCaught(9, "TechnicianNotAvailableException", e); }

    try { Schedule s; s.add(Shift(today, 9, 13)); s.add(Shift(today, 12, 18)); }
    catch (const ScheduleConflictException& e) { showCaught(10, "ScheduleConflictException", e); }

    try { Workbench b(1, "пайка"); b.occupy(1); b.occupy(2); }
    catch (const WorkbenchOccupiedException& e) { showCaught(11, "WorkbenchOccupiedException", e); }

    try {
        Warranty old(1, Date(1, 1, 2025), 3);
        old.addArticle("BAT-S24");
        WarrantyClaim claim(old, today, "демо");
        claim.submit(today, "BAT-S24");
    } catch (const WarrantyExpiredException& e) { showCaught(12, "WarrantyExpiredException", e); }

    try {
        Invoice inv(1, 1, 1, today);
        inv.addLine(InvoiceLine("Работа", 1, Money::fromRubles(10)));
        inv.applyDiscount(Discount("NEW10", 10, today.addDays(30), Money()), today);
    } catch (const InvalidDiscountException& e) { showCaught(13, "InvalidDiscountException", e); }

    try { CashRegister empty; empty.withdraw(Money::fromRubles(100), "демо"); }
    catch (const InsufficientFundsException& e) { showCaught(14, "InsufficientFundsException", e); }

    try { QualityCheck qc(1, 1); qc.addItem("Включается"); qc.confirm(); }
    catch (const QualityCheckFailedException& e) { showCaught(15, "QualityCheckFailedException", e); }

    try { Review r(1, 1, 0, ""); r.validate(); }
    catch (const InvalidReviewException& e) { showCaught(16, "InvalidReviewException", e); }

    try { Date bad(31, 2, 2026); (void)bad; }
    catch (const InvalidDateException& e) { showCaught(17, "InvalidDateException", e); }

    cout << "Данные фирмы не изменились: заказов " << firm_.getOrders().size()
         << ", клиентов " << firm_.getClients().size() << ".\n";
}
