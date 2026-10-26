#pragma once
#include <string>
#include <vector>
#include "core/Date.h"
#include "core/Money.h"
#include "core/Address.h"
#include "core/ContactInfo.h"
#include "devices/Device.h"
#include "inventory/Warehouse.h"
#include "inventory/PurchaseOrder.h"
#include "finance/PriceList.h"
#include "finance/Invoice.h"
#include "finance/Payment.h"
#include "finance/PaymentMethod.h"
#include "finance/CashRegister.h"
#include "repair/RepairOrder.h"
#include "repair/Warranty.h"
#include "people/Client.h"
#include "people/CorporateClient.h"
#include "people/ServiceContract.h"
#include "people/Technician.h"
#include "people/Receptionist.h"
#include "people/Manager.h"
#include "people/Accountant.h"
#include "people/Courier.h"
#include "people/EmployeeClient.h"
#include "workshop/Workshop.h"
#include "workshop/Appointment.h"
#include "workshop/Delivery.h"
#include "service/NotificationService.h"
#include "service/Review.h"

/**
 * @brief Фирма по ремонту: владеет всеми объектами и связывает их по ID.
 * Методы с ошибкой возвращают false или -1 и ничего не меняют. Правило всех методов:
 * сначала все проверки, потом изменения. Ссылки на элементы векторов не хранятся.
 */
class RepairFirm {
public:
    RepairFirm(const std::string& name, const Warehouse& w, const PriceList& p,
               const Workshop& ws, const Date& today);

    /** @brief Новый уникальный ID (общий счётчик для клиентов, заказов, задач, счетов и т.д.). */
    int newId();

    /** @name Клиенты */
    /** @brief Добавить клиента и вернуть его ID; -1, если телефон или e-mail неверные. */
    int addClient(const std::string& name, const ContactInfo& c, const Date& birth);   // TODO(exc): InvalidClientDataException
    /** @brief Добавить корпоративного клиента; -1, если контакты или УНП неверные. */
    int addCorporateClient(const std::string& name, const ContactInfo& c, const Date& birth,
                           const std::string& company, const std::string& taxId, const ServiceContract& contract);
    /** @brief Есть ли клиент (обычный, корпоративный или сотрудник-клиент) с таким ID. */
    bool hasClient(int id) const;
    /** @brief Клиент по ID. Сначала проверить hasClient! Для наследников вернётся их часть Client. */
    Client& clientById(int id);   // TODO(exc): вместе с hasClient -> findClient + ClientNotFoundException
    const Client& clientById(int id) const;

    /** @name Сотрудники (перегрузка) */
    /** @brief Нанять сотрудника; false, если сотрудник с таким ID уже есть в этой должности. */
    bool hire(const Technician& t);
    bool hire(const Receptionist& r);
    bool hire(const Manager& m);
    bool hire(const Accountant& a);
    bool hire(const Courier& c);
    /** @brief Нанять сотрудника-клиента: он одновременно сотрудник (зарплата) и клиент (заказы). */
    bool hire(const EmployeeClient& ec);
    /** @brief Индекс мастера по ID или -1. */
    int indexOfTechnician(int id) const;
    /** @brief Индекс наименее загруженного мастера, который может работать с устройством, или -1. */
    int freeTechnicianIndex(const Device& d) const;   // TODO(exc): TechnicianNotAvailableException

    /** @name Заказы */
    /**
     * @brief Принять устройство: заказ создаёт приёмщик, срок считается по настоящему типу d.
     * -1, если клиента нет, у него слишком много активных заказов или нет приёмщика.
     */
    int createOrder(int clientId, const Device& d, const std::string& problem, bool urgent);
    /** @brief Индекс заказа по ID или -1. */
    int indexOfOrder(int id) const;   // TODO(exc): вместе с orderAt -> findOrder + OrderNotFoundException
    /** @brief Заказ по индексу из indexOfOrder. Ссылку не хранить: после createOrder она может устареть. */
    RepairOrder& orderAt(int index);
    const RepairOrder& orderAt(int index) const;
    /**
     * @brief Диагностика. d — настоящее устройство (в заказе лежит срезанная копия), поэтому
     * quickTest вызывается виртуально у d. false, если заказа нет, устройство чужое, мастера нет
     * или устройство неремонтопригодно (тогда заказ отменяется).
     */
    bool runDiagnosis(int orderId, const Device& d);
    /** @brief Добавить работу, назначить мастера через менеджера, перевести в ремонт. ID задачи или -1. */
    int addTask(int orderId, const std::string& description);
    /**
     * @brief Выдать запчасть со склада в задачу. Порядок: найти, проверить совместимость и остаток,
     * только потом списать. Если запчасти нет, заказ переходит в "Ожидание запчастей" (если можно).
     */
    bool issuePart(int orderId, int taskId, const std::string& article, int qty);   // TODO(exc): OutOfStockException / IncompatiblePartException
    /** @brief Закрыть задачу у заказа и у мастера; если все готовы — на проверку качества. */
    bool finishTask(int orderId, int taskId, double hours);
    /** @brief Проверка качества: успех — "Готов" и уведомление, провал — обратно в ремонт и false. */
    bool qualityCheck(int orderId, bool ok);   // TODO(exc): QualityCheckFailedException
    /** @brief Просроченные заказы на сегодня. */
    std::vector<RepairOrder> overdueOrders() const;

    /** @name Деньги */
    /** @brief Выставить счёт по готовому заказу (скидка клиента — отдельной строкой). ID счёта или -1. */
    int billOrder(int orderId);
    /** @brief Индекс счёта по ID или -1. */
    int indexOfInvoice(int id) const;
    /** @brief Индекс счёта по заказу или -1. */
    int indexOfInvoiceForOrder(int orderId) const;
    /** @brief Оплатить счёт: платёж, деньги в кассу, баллы на карту клиента. */
    bool payInvoice(int invoiceId, const Money& sum, const PaymentMethod& m);   // TODO(exc): InsufficientFundsException
    /** @brief Выдать устройство клиенту (счёт должен быть оплачен) и оформить гарантию 3 месяца. */
    bool issueToClient(int orderId);
    /** @brief Выручка за месяц: оплаченные счета с датой в этом месяце. */
    Money monthRevenue(int month, int year) const;
    /** @brief Фонд зарплаты за месяц: сумма calculateSalary всех сотрудников (полиморфно). */
    Money salaryFund(int month) const;

    /** @name Запись, доставка, склад, отзывы */
    /** @brief Записать клиента на приём; -1, если клиента нет, час нерабочий или время занято. */
    int bookAppointment(int clientId, const Date& date, int hour, const std::string& purpose);   // TODO(exc): ScheduleConflictException
    /** @brief Отправить курьера с устройством по заказу на адрес to (наименее занятый курьер). ID или -1. */
    int arrangeDelivery(int orderId, const Address& to);
    /** @brief Отметить доставку выполненной у доставки и у курьера. */
    bool completeDelivery(int deliveryId);
    /** @brief Принять поставку от поставщика на склад. false, если уже принята. */
    bool receivePurchase(PurchaseOrder& po);
    /** @brief Оставить отзыв о выданном заказе; false, если заказ чужой, не выдан или отзыв некорректен. */
    bool addReview(int clientId, int orderId, int rating, const std::string& text);   // TODO(exc): InvalidReviewException

    /** @name Дата */
    const Date& today() const;
    void setToday(const Date& d);
    /** @brief Сдвинуть «сегодня» на n дней вперёд (для демонстрации просрочек и гарантий). */
    void advanceDays(int n);

    /** @name Доступ для меню */
    const std::string& getName() const;
    const std::vector<Client>& getClients() const;
    const std::vector<CorporateClient>& getCorporateClients() const;
    const std::vector<Technician>& getTechnicians() const;
    const std::vector<Receptionist>& getReceptionists() const;
    const std::vector<Manager>& getManagers() const;
    const std::vector<Accountant>& getAccountants() const;
    const std::vector<Courier>& getCouriers() const;
    const std::vector<EmployeeClient>& getEmployeeClients() const;
    const std::vector<RepairOrder>& getOrders() const;
    const std::vector<Invoice>& getInvoices() const;
    const std::vector<Payment>& getPayments() const;
    const std::vector<Warranty>& getWarranties() const;
    const std::vector<Appointment>& getAppointments() const;
    const std::vector<Delivery>& getDeliveries() const;
    const std::vector<Review>& getReviews() const;
    const Warehouse& getWarehouse() const;
    const PriceList& getPriceList() const;
    const Workshop& getWorkshop() const;
    const CashRegister& getCash() const;
    const NotificationService& getNotifier() const;

private:
    /** @brief Индекс обычного клиента по ID или -1. */
    int indexOfClient(int id) const;
    /** @brief Индекс корпоративного клиента по ID или -1. */
    int indexOfCorporate(int id) const;
    /** @brief Индекс сотрудника-клиента по ID или -1. */
    int indexOfEmployeeClient(int id) const;
    /** @brief Отправить клиенту заказа уведомление о текущем статусе. */
    void notifyStatus(int orderIndex);

    std::string name_;
    Date today_;
    std::vector<Client> clients_;
    std::vector<CorporateClient> corporateClients_;
    std::vector<Technician> technicians_;
    std::vector<Receptionist> receptionists_;
    std::vector<Manager> managers_;
    std::vector<Accountant> accountants_;
    std::vector<Courier> couriers_;
    std::vector<EmployeeClient> employeeClients_;
    std::vector<RepairOrder> orders_;
    std::vector<Invoice> invoices_;
    std::vector<Payment> payments_;
    std::vector<Warranty> warranties_;
    std::vector<Appointment> appointments_;
    std::vector<Delivery> deliveries_;
    std::vector<Review> reviews_;
    Warehouse warehouse_;
    PriceList prices_;
    Workshop workshop_;
    CashRegister cash_;
    NotificationService notifier_;
    int nextId_;
};