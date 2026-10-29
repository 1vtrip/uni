#include "ui/DemoData.h"
#include "devices/Smartphone.h"
#include "devices/Laptop.h"
#include "devices/WashingMachine.h"

/** @brief Список моделей одной строкой кода: для запчасти, подходящей к одной или двум моделям. */
static std::vector<std::string> models(const std::string& a, const std::string& b = "") {
    std::vector<std::string> list;
    list.push_back(a);
    if (!b.empty()) list.push_back(b);
    return list;
}

DeviceCategory phoneCategory() { return DeviceCategory("Смартфоны", Money::fromRubles(15), 3); }
DeviceCategory laptopCategory() { return DeviceCategory("Ноутбуки", Money::fromRubles(25), 5); }
DeviceCategory applianceCategory() { return DeviceCategory("Бытовая техника", Money::fromRubles(30), 4); }

/**
 * @brief Состав демо-фирмы:
 * - склад из 6 запчастей; экран Galaxy S24 на грани минимума, чтобы сразу был виден дозаказ;
 * - прайс: ставка 25 руб./ч, срочность +50%, 5 услуг;
 * - мастерская на 3 места; термофен давно не калибровали (для отчёта ICertifiable);
 * - сотрудники всех типов, ID через newId(), чтобы не пересекались с клиентами и заказами;
 *   два мастера покрывают смартфоны, ноутбуки и бытовую технику; Максим недавно аттестован,
 *   Ольге аттестация нужна (для отчёта ICertifiable); сотрудник-клиент — ромб;
 * - 2 обычных клиента и 1 корпоративный с договором;
 * - заказ Ивана (смартфон) готов к выдаче: осталось выставить счёт, оплатить и выдать;
 * - заказ Марии (ноутбук) в ремонте: можно выдать запчасть и закрыть задачу;
 * - запись Марии на приём на завтра.
 * Для ввода смартфона в меню подойдёт IMEI 356938035643809.
 */
RepairFirm createDemoFirm() {
    Date today(27, 10, 2026);
    Address shop("Минск", "Притыцкого", "20");

    DeviceCategory phones = phoneCategory();
    DeviceCategory laptops = laptopCategory();
    DeviceCategory home = applianceCategory();

    Warehouse w(shop);
    w.add(SparePart("BAT-S24", "Аккумулятор Galaxy S24", models("Galaxy S24", "Galaxy S24+"),
                    Money::fromRubles(40), Money::fromRubles(70)), 5, 2);
    w.add(SparePart("SCR-S24", "Экран Galaxy S24", models("Galaxy S24"),
                    Money::fromRubles(90), Money::fromRubles(150)), 1, 2);
    w.add(SparePart("BAT-IP15", "Аккумулятор iPhone 15", models("iPhone 15"),
                    Money::fromRubles(50), Money::fromRubles(85)), 3, 1);
    w.add(SparePart("SSD-512", "SSD 512 ГБ", models("ZenBook 14", "X550"),
                    Money::fromRubles(80), Money::fromRubles(120)), 4, 1);
    w.add(SparePart("RAM-8", "Память 8 ГБ", models("ZenBook 14", "X550"),
                    Money::fromRubles(35), Money::fromRubles(60)), 6, 2);
    w.add(SparePart("DRUM-F2", "Подшипник барабана", models("F2J3"),
                    Money::fromRubles(25), Money::fromRubles(45)), 2, 1);

    PriceList prices(Money::fromRubles(25), 50);
    prices.addItem(ServiceItem("DIAG-PH", "Диагностика смартфона", Money::fromRubles(15), phones));
    prices.addItem(ServiceItem("SCR-REP", "Замена экрана", Money::fromRubles(40), phones));
    prices.addItem(ServiceItem("DIAG-LT", "Диагностика ноутбука", Money::fromRubles(25), laptops));
    prices.addItem(ServiceItem("OS-INST", "Установка системы", Money::fromRubles(30), laptops));
    prices.addItem(ServiceItem("HOME-VISIT", "Выезд мастера на дом", Money::fromRubles(20), home));

    Workshop ws("Центральная", shop);
    ws.addBench(Workbench(1, "паяльная станция"));
    ws.addBench(Workbench(2, "сборочный стол"));
    ws.addBench(Workbench(3, "стенд бытовой техники"));
    ws.addTool(Tool("INV-001", "Мультиметр", Date(1, 9, 2026)));
    ws.addTool(Tool("INV-002", "Термофен", Date(10, 1, 2026)));
    ws.addTool(Tool("INV-003", "Паяльник", Date(15, 7, 2026)));

    RepairFirm f("Ремонт-Сервис", w, prices, ws, today);

    ContactInfo staff("+375291000000", "staff@remont.by", shop);
    Technician maxim(f.newId(), "Максим Ковалёв", ContactInfo("+375291000001", "maxim@remont.by", shop),
                     Date(12, 5, 1995), Date(1, 3, 2022), Money::fromRubles(1000), 2, 3);
    maxim.addSpecialization(Specialization("Ремонт смартфонов", phones, 1));
    maxim.addSpecialization(Specialization("Ремонт ноутбуков", laptops, 2));
    maxim.certify(Date(1, 9, 2026));
    Technician olga(f.newId(), "Ольга Новик", ContactInfo("+375291000002", "olga@remont.by", shop),
                    Date(3, 8, 1990), Date(1, 9, 2024), Money::fromRubles(1100), 3, 3);
    olga.addSpecialization(Specialization("Бытовая техника", home, 1));
    olga.addSpecialization(Specialization("Ремонт смартфонов", phones, 1));
    f.hire(maxim);
    f.hire(olga);
    f.hire(Receptionist(f.newId(), "Анна Белова", staff, Date(20, 2, 1999), Date(1, 1, 2025), Money::fromRubles(800)));
    f.hire(Manager(f.newId(), "Сергей Орлов", staff, Date(7, 11, 1984), Date(1, 6, 2020), Money::fromRubles(1500),
                   "Сервисный отдел", 20));
    f.hire(Accountant(f.newId(), "Елена Жук", staff, Date(14, 4, 1980), Date(1, 2, 2019), Money::fromRubles(1200),
                      "АБ-2019-0042"));
    f.hire(Courier(f.newId(), "Дмитрий Лис", staff, Date(30, 6, 1997), Date(1, 6, 2024), Money::fromRubles(700),
                   "1234 AB-7"));
    f.hire(EmployeeClient(f.newId(), "Павел Ким", ContactInfo("+375291000007", "pavel@remont.by", shop),
                          Date(9, 9, 1996), "Инженер склада", Date(1, 4, 2023), Money::fromRubles(950), today));

    int ivan = f.addClient("Иван Петров", ContactInfo("+375291234567", "ivan.petrov@mail.ru",
                           Address("Минск", "Ленина", "5", "12")), Date(15, 3, 1990));
    int maria = f.addClient("Мария Соколова", ContactInfo("+375447654321", "m.sokolova@gmail.com",
                            Address("Минск", "Сурганова", "28", "41")), Date(2, 12, 1985));
    f.addCorporateClient("Андрей Лукин", ContactInfo("+375173210000", "office@technoservice.by",
                         Address("Минск", "Немига", "3")), Date(21, 1, 1978),
                         "ТехноСервис", "190123456",
                         ServiceContract("SC-2026-01", Date(1, 1, 2026), Date(31, 12, 2026), 10, 2));

    Brand samsung("Samsung", "Южная Корея", true);
    Smartphone s24("SN-S24-001", "Galaxy S24", samsung, phones, Date(1, 3, 2026),
                   "490154203237518", 6.2, false);
    int o1 = f.createOrder(ivan, s24, "Быстро садится батарея", false);
    f.runDiagnosis(o1, s24);
    int t1 = f.addTask(o1, "Замена аккумулятора");
    f.issuePart(o1, t1, "BAT-S24", 1);
    f.finishTask(o1, t1, 1);
    f.qualityCheck(o1, true);

    Brand asus("ASUS", "Тайвань", true);
    Laptop zen("SN-ZB-014", "ZenBook 14", asus, laptops, Date(10, 2, 2024), "Intel Core i5", 8, "HDD");
    int o2 = f.createOrder(maria, zen, "Медленно работает, шумит диск", false);
    f.runDiagnosis(o2, zen);
    f.addTask(o2, "Замена HDD на SSD");

    f.bookAppointment(maria, Date(28, 10, 2026), 11, "Забрать ноутбук после ремонта");

    return f;
}
