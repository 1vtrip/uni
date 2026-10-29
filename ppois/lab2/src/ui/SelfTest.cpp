#include "ui/SelfTest.h"
#include <iostream>
#include <sstream>
#include "ui/DemoData.h"
#include "devices/Smartphone.h"
#include "devices/Laptop.h"
#include "devices/WashingMachine.h"
#include "repair/QualityCheck.h"
#include "repair/WarrantyClaim.h"
#include "service/ReportGenerator.h"
#include "exceptions/Exceptions.h"

using std::cout;
using S = OrderStatus::Status;

static int passed = 0;
static int failed = 0;

/** @brief Одна проверка: вывести результат и посчитать его. */
static void check(const std::string& name, bool ok) {
    cout << (ok ? "  [OK]   " : "  [FAIL] ") << name << "\n";
    if (ok) ++passed; else ++failed;
}

/** @brief Заголовок группы проверок. */
static void section(const std::string& title) { cout << "\n" << title << "\n"; }

/** @brief Есть ли в списке дефект с таким кодом. */
static bool hasDefect(const std::vector<Defect>& list, const std::string& code) {
    for (size_t i = 0; i < list.size(); ++i)
        if (list[i].getCode() == code) return true;
    return false;
}

/** @brief Проверки core: Date, Money, ContactInfo. */
static void testCore() {
    section("Этап 1. core");
    check("31.01.2026 + 1 месяц = 28.02.2026", Date(31, 1, 2026).addMonths(1) == Date(28, 2, 2026));
    check("31.12.2025 + 1 день = 01.01.2026", Date(31, 12, 2025).addDays(1) == Date(1, 1, 2026));
    check("високосные: 2024 и 2000 да, 1900 нет", Date::isLeap(2024) && Date::isLeap(2000) && !Date::isLeap(1900));
    check("15.01 -> 14.03 = 1 полный месяц", Date(15, 1, 2026).monthsTo(Date(14, 3, 2026)) == 1);
    bool thrown = false;
    try { Date(31, 2, 2026); } catch (const InvalidDateException&) { thrown = true; }
    check("Date(31.02.2026) бросает InvalidDateException", thrown);
    check("Money::fromRubles(0.29) = 29 коп. (округление)", Money::fromRubles(0.29).getCoins() == 29);
    check("Money(999) * 0.5 = 500 коп.", (Money(999) * 0.5).getCoins() == 500);
    check("10% от 10.50 = 1.05", Money(1050).percent(10).getCoins() == 105);
    ContactInfo ok("+375291234567", "ivan@mail.ru", Address("Минск", "Ленина", "5"));
    check("телефон +375291234567 и e-mail верные", ok.isPhoneValid() && ok.isEmailValid());
    thrown = false;
    try { ContactInfo("12345", "ivan@mail.ru", Address("Минск", "Ленина", "5")).validate(); }
    catch (const InvalidClientDataException&) { thrown = true; }
    check("телефон 12345 -> InvalidClientDataException", thrown);
}

/** @brief Проверки devices: IMEI и полиморфизм через const Device&. */
static void testDevices() {
    section("Этап 2. devices (полиморфизм)");
    Brand sam("Samsung", "Южная Корея", true), asus("ASUS", "Тайвань", true), lg("LG", "Южная Корея", true);
    Smartphone wet("SN1", "Galaxy S24", sam, phoneCategory(), Date(1, 3, 2026), "490154203237518", 6.2, true);
    Smartphone badImei("SN2", "Galaxy S24", sam, phoneCategory(), Date(1, 3, 2026), "490154203237519", 6.2, false);
    check("IMEI 490154203237518 верный, ...519 — нет", wet.isImeiValid() && !badImei.isImeiValid());
    const Device& d = wet;
    check("через const Device&: typeName() = «Смартфон»", d.typeName() == "Смартфон");
    check("через const Device&: срок мокрого смартфона 3 + 5 = 8 дней", d.estimateRepairDays() == 8);
    Laptop hdd("LT1", "X550", asus, laptopCategory(), Date(1, 1, 2018), "Intel i3", 2, "HDD");
    const Device& l = hdd;
    check("через const Device&: диагностика ноутбука 25 * 1.5 = 37.5", l.diagnosticPrice() == Money::fromRubles(37.5));
    std::vector<Defect> found = l.quickTest(Date(29, 10, 2026));
    check("quickTest ноутбука: AGE (базовый), RAM и HDD (свои)", hasDefect(found, "AGE") && hasDefect(found, "RAM") && hasDefect(found, "HDD"));
    WashingMachine wm("WM1", "F2J3", lg, applianceCategory(), Date(1, 6, 2026), 2000, 60, false, 7, "Повреждён");
    const Device& w = wm;
    check("стиральная машина: два уровня наследования, quickTest находит SPIN", hasDefect(w.quickTest(Date(29, 10, 2026)), "SPIN"));
    check("стиральная машина 60 кг: нужен выезд, диагностика 30 + 20", w.needsHomeVisit() && w.diagnosticPrice() == Money::fromRubles(50));
    check("перевозка 60 кг по городу: 15 + 0.5 * 60 = 45", wm.transportCost(Address("Минск", "A", "1"), Address("Минск", "B", "2")) == Money::fromRubles(45));
}

/** @brief Проверки inventory и finance. */
static void testMoney() {
    section("Этап 3. склад и деньги");
    Date today(29, 10, 2026);
    std::vector<std::string> models;
    models.push_back("Galaxy S24");
    SparePart bat("BAT-S24", "Аккумулятор", models, Money::fromRubles(40), Money::fromRubles(70));
    Warehouse w(Address("Минск", "Притыцкого", "20"));
    w.add(bat, 3, 1);
    bool thrown = false;
    try { w.take("BAT-S24", 5); } catch (const OutOfStockException&) { thrown = true; }
    check("взять 5 из 3 -> OutOfStockException, на складе по-прежнему 3", thrown && w.find("BAT-S24").getQty() == 3);
    PurchaseOrder po(1, 1, today);
    po.addLine(bat, 4);
    po.receive(w, 1);
    po.receive(w, 1);
    check("поставка 4 шт., принятая дважды, засчитана один раз", w.find("BAT-S24").getQty() == 7);
    Supplier sup(1, "ЗапчастиПро", ContactInfo("+375291234567", "parts@mail.by", Address("Минск", "A", "1")), 7);
    check("50 шт. у поставщика: скидка 10%", sup.quote(SparePart("X", "x", models, Money::fromRubles(100), Money()), 50) == Money::fromRubles(4500));
    Invoice inv(1, 1, 1, today);
    inv.addLine(InvoiceLine("Работа", 1, Money::fromRubles(37.5)));
    inv.addLine(InvoiceLine("Аккумулятор", 1, Money::fromRubles(70)));
    Discount d("AUTUMN10", 10, Date(31, 10, 2026), Money::fromRubles(50));
    thrown = false;
    try { inv.applyDiscount(d, today); } catch (const InvalidDiscountException&) { thrown = true; }
    check("неодобренная скидка -> InvalidDiscountException", thrown);
    d.approve();
    inv.applyDiscount(d, today);
    check("счёт 107.5 со скидкой 10% = 96.75", inv.total() == Money::fromRubles(96.75));
    CashRegister cash;
    thrown = false;
    try { Payment(1, Money::fromRubles(10), PaymentMethod("Наличные", 0), today).process(inv, cash); }
    catch (const InsufficientFundsException&) { thrown = true; }
    check("оплата 10 rub за 96.75 -> InsufficientFundsException", thrown && !inv.isPaid());
    Payment pay(1, Money::fromRubles(100), PaymentMethod("Карта", 2.5), today);
    pay.process(inv, cash);
    check("оплата картой: в кассу сумма минус 2.5%", inv.isPaid() && cash.getBalance() == Money::fromRubles(94.33));
    cash.withdraw(Money::fromRubles(90), "расход");
    thrown = false;
    try { pay.refund(cash); } catch (const InsufficientFundsException&) { thrown = true; }
    check("возврат при нехватке в кассе: исключение, платёж не помечен возвращённым", thrown && !pay.isRefunded());
    LoyaltyCard card("LC-1");
    card.addPoints(Money::fromRubles(600));
    check("600 баллов -> «Серебряный», скидка 3%", card.getLevel() == "Серебряный" && card.discountPercent() == 3);
    thrown = false;
    try { card.spendPoints(1000); } catch (const InsufficientFundsException&) { thrown = true; }
    check("списать 1000 баллов из 600 -> InsufficientFundsException", thrown);
    check("зарплата 1000 -> на руки 870 (налог 13%)", Salary(1, 10, Money::fromRubles(1000)).net() == Money::fromRubles(870));
}

/** @brief Проверки repair: статусы, срезка, диагноз, качество, гарантия. */
static void testRepair() {
    section("Этап 4. repair");
    Date today(29, 10, 2026);
    OrderStatus st;
    st.moveTo(S::Diagnostics); st.moveTo(S::InRepair); st.moveTo(S::QualityCheck);
    st.moveTo(S::InRepair); st.moveTo(S::QualityCheck); st.moveTo(S::Ready); st.moveTo(S::Issued);
    check("полный путь статусов (с возвратом из проверки в ремонт) до «Выдан»", st.get() == S::Issued && st.isFinal());
    bool thrown = false;
    try { st.moveTo(S::InRepair); } catch (const InvalidStatusTransitionException&) { thrown = true; }
    check("«Выдан» -> «В ремонте» бросает InvalidStatusTransitionException", thrown && st.get() == S::Issued);
    Laptop lap("LT1", "X550", Brand("ASUS", "Тайвань", true), laptopCategory(), Date(1, 1, 2018), "Intel i3", 8, "HDD");
    RepairOrder o(1, 1, lap, "Шумит", today, false);
    check("срезка: копия в заказе — «Устройство», но описание и цена — от ноутбука",
          o.getDevice().typeName() == "Устройство" && o.getDeviceInfo().find("Ноутбук") == 0
          && o.getDiagnosticPrice() == Money::fromRubles(37.5));
    check("срок по настоящему типу: 5 + 2 (HDD) = 7 дней", o.getDeadline() == today.addDays(7));
    Diagnosis dead(1, today);
    dead.addDefect(Defect("A", "a", 5)); dead.addDefect(Defect("B", "b", 5)); dead.addDefect(Defect("C", "c", 4));
    dead.decideRepairable();
    o.setStatus(S::Diagnostics);
    thrown = false;
    try { o.setDiagnosis(dead); } catch (const DeviceNotRepairableException&) { thrown = true; }
    check("3 критических дефекта -> DeviceNotRepairableException, заказ отменён", thrown && o.getStatus().get() == S::Cancelled);
    std::vector<std::string> s5;
    s5.push_back("Galaxy S5");
    RepairTask t(1, "АКБ");
    t.start(1);
    thrown = false;
    try { t.addPart(SparePart("BAT-S5", "АКБ S5", s5, Money(), Money()), 1, lap); } catch (const IncompatiblePartException&) { thrown = true; }
    check("запчасть от другой модели -> IncompatiblePartException", thrown);
    QualityCheck qc(1, 1);
    qc.addItem("Включается");
    thrown = false;
    try { qc.confirm(); } catch (const QualityCheckFailedException&) { thrown = true; }
    check("passed() спрашивает (false), confirm() требует (QualityCheckFailedException)", !qc.passed() && thrown);
    Warranty w(1, Date(31, 1, 2026), 1);
    check("гарантия с 31.01 на 1 месяц заканчивается 28.02", w.endDate() == Date(28, 2, 2026));
    Warranty old(2, Date(1, 1, 2025), 3);
    old.addArticle("BAT-S24");
    thrown = false;
    try { WarrantyClaim(old, today, "x").submit(today, "BAT-S24"); } catch (const WarrantyExpiredException&) { thrown = true; }
    check("заявка по истёкшей гарантии -> WarrantyExpiredException", thrown);
}

/** @brief Проверки people: полиморфная зарплата, лимиты клиентов, ромб. */
static void testPeople() {
    section("Этапы 5 и 7. people, ромб");
    Date today(29, 10, 2026);
    ContactInfo ci("+375291111111", "staff@firm.by", Address("Минск", "Ленина", "1"));
    Technician t(1, "Максим", ci, Date(1, 1, 1995), Date(1, 3, 2022), Money::fromRubles(1000), 2, 1);
    Manager m(2, "Сергей", ci, Date(1, 1, 1985), Date(1, 1, 2020), Money::fromRubles(1500), "Сервис", 20);
    Courier c(3, "Дмитрий", ci, Date(1, 1, 1997), Date(1, 6, 2024), Money::fromRubles(700), "1234 AB-7");
    const Employee& e1 = t;
    const Employee& e2 = m;
    const Employee& e3 = c;
    check("через const Employee&: мастер 1000 + 10% = 1100", e1.calculateSalary(10) == Money::fromRubles(1100));
    check("через const Employee&: менеджер 1500 + 20% = 1800", e2.calculateSalary(10) == Money::fromRubles(1800));
    check("через const Employee&: курьер без доставок = 700", e3.calculateSalary(10) == Money::fromRubles(700));
    t.takeTask(1);
    bool thrown = false;
    try { t.takeTask(2); } catch (const TechnicianNotAvailableException&) { thrown = true; }
    check("мастер с лимитом 1 берёт вторую задачу -> TechnicianNotAvailableException", thrown);
    thrown = false;
    try { t.addShift(Shift(today, 9, 13)); t.addShift(Shift(today, 12, 18)); } catch (const ScheduleConflictException&) { thrown = true; }
    check("пересекающиеся смены -> ScheduleConflictException", thrown && t.hoursWorked() == 4);
    Client client(4, "Иван", ci, Date(1, 1, 1990), today);
    CorporateClient corp(5, "Анна", ci, Date(1, 1, 1985), today, "ТехноСервис", "190123456",
                         ServiceContract("SC-1", Date(1, 1, 2026), Date(31, 12, 2026), 10, 2));
    const Client& c1 = client;
    const Client& c2 = corp;
    check("через const Client&: лимит заказов 3 и 20", c1.maxActiveOrders() == 3 && c2.maxActiveOrders() == 20);
    check("скидка компании по договору: 100 -> 90", c2.applyDiscount(Money::fromRubles(100)) == Money::fromRubles(90));
    EmployeeClient ec(6, "Павел", ci, Date(1, 1, 1996), "Инженер", Date(1, 4, 2023), Money::fromRubles(950), today);
    const Employee& asEmployee = ec;
    const Client& asClient = ec;
    check("ромб: зарплата через Employee& и скидка 30% через Client&",
          asEmployee.calculateSalary(10) == Money::fromRubles(950) && asClient.applyDiscount(Money::fromRubles(100)) == Money::fromRubles(70));
    const Person& p1 = asEmployee;
    const Person& p2 = asClient;
    check("ромб: Person через Employee и через Client — один объект", &p1 == &p2 && ec.getName() == "Павел");
}

/** @brief Проверки интерфейсов: IPrintable, IPayable, ICertifiable. */
static void testInterfaces() {
    section("Этап 7. интерфейсы");
    Date today(29, 10, 2026);
    Smartphone p("SN1", "Galaxy S24", Brand("Samsung", "Корея", true), phoneCategory(), Date(1, 3, 2026), "490154203237518", 6.2, false);
    const IPrintable& printable = p;
    std::ostringstream out;
    out << printable;
    check("IPrintable: один operator<< выводит версию смартфона", out.str().find("Смартфон Samsung") == 0);
    Invoice inv(1, 1, 1, today);
    inv.addLine(InvoiceLine("Работа", 1, Money::fromRubles(100)));
    Salary s(1, 10, Money::fromRubles(1000));
    ReportGenerator r(Date(1, 10, 2026), Date(31, 10, 2026));
    bool before = r.unpaid(inv) == Money::fromRubles(100) && r.unpaid(s) == Money::fromRubles(870);
    inv.markPaid();
    s.markPaid();
    check("IPayable: unpaid() для счёта и зарплаты, после markPaid — 0", before && r.unpaid(inv).isZero() && r.unpaid(s).isZero());
    Tool old("INV-1", "Термофен", Date(1, 1, 2026));
    Technician t(1, "Максим", ContactInfo("+375291111111", "m@firm.by", Address("Минск", "A", "1")),
                 Date(1, 1, 1995), Date(1, 3, 2022), Money::fromRubles(1000), 2, 3);
    bool need = r.checkStatus(old, today) == "требуется проверка" && r.checkStatus(t, today) == "требуется проверка";
    old.certify(today);
    t.certify(today);
    check("ICertifiable: checkStatus() для инструмента и мастера, после certify — в порядке",
          need && r.checkStatus(old, today) == "в порядке" && r.checkStatus(t, today) == "в порядке");
}

/** @brief Проверки workshop и service: места, записи, отзывы и полный сценарий через RepairFirm. */
static void testFirm() {
    section("Этап 6. мастерская и полный цикл через RepairFirm");
    Workbench b(1, "пайка");
    b.occupy(1);
    bool thrown = false;
    try { b.occupy(2); } catch (const WorkbenchOccupiedException&) { thrown = true; }
    check("занятое место -> WorkbenchOccupiedException", thrown);
    Appointment a(1, 1, Date(29, 10, 2026), 10, "x");
    thrown = false;
    try { a.reschedule(Date(29, 10, 2026), 20); } catch (const ScheduleConflictException&) { thrown = true; }
    check("запись на 20:00 -> ScheduleConflictException", thrown);
    thrown = false;
    try { Review(1, 1, 0, "").validate(); } catch (const InvalidReviewException&) { thrown = true; }
    check("отзыв с оценкой 0 -> InvalidReviewException", thrown);

    RepairFirm f = createDemoFirm();
    thrown = false;
    try { f.findClient(999); } catch (const ClientNotFoundException&) { thrown = true; }
    check("findClient(999) -> ClientNotFoundException", thrown);
    thrown = false;
    try { f.findOrder(999); } catch (const OrderNotFoundException&) { thrown = true; }
    check("findOrder(999) -> OrderNotFoundException", thrown);
    thrown = false;
    try { f.findInvoice(999); } catch (const RepairFirmException&) { thrown = true; }
    check("findInvoice(999) ловится базовым RepairFirmException", thrown);

    int o1 = f.getOrders()[0].getId();
    int bill = f.billOrder(o1);
    check("счёт по заказу Ивана: 15 + 25 + 70 = 110", f.findInvoice(bill).total() == Money::fromRubles(110));
    f.payInvoice(bill, Money::fromRubles(200), PaymentMethod("Наличные", 0));
    f.issueToClient(o1);
    check("оплата и выдача: «Выдан», гарантия на аккумулятор", f.findOrder(o1).getStatus().get() == S::Issued
          && f.getWarranties().size() == 1 && f.getWarranties()[0].covers("BAT-S24"));

    int o2 = f.getOrders()[1].getId();
    int t2 = f.getOrders()[1].getTasks()[0].getId();
    f.issuePart(o2, t2, "SSD-512", 1);
    f.finishTask(o2, t2, 2);
    check("ноутбук Марии: запчасть выдана, задача закрыта -> «Проверка качества»", f.findOrder(o2).getStatus().get() == S::QualityCheck);
    thrown = false;
    try { f.qualityCheck(o2, false); } catch (const QualityCheckFailedException&) { thrown = true; }
    check("провал проверки качества: исключение и заказ обратно «В ремонте»", thrown && f.findOrder(o2).getStatus().get() == S::InRepair);
    check("фонд зарплаты > 0 и уведомления отправлены", !f.salaryFund(10).isZero() && f.getNotifier().history().size() > 5);
}

int runSelfTest() {
    passed = 0;
    failed = 0;
    cout << "Самопроверка «Фирма по ремонту»\n";
    testCore();
    testDevices();
    testMoney();
    testRepair();
    testPeople();
    testInterfaces();
    testFirm();
    cout << "\nПройдено " << passed << " из " << (passed + failed) << (failed == 0 ? " — всё работает.\n" : ".\n");
    return failed;
}
