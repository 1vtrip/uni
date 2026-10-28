#include "service/RepairFirm.h"
#include "repair/Diagnosis.h"
#include "repair/QualityCheck.h"
#include "service/ReportGenerator.h"

RepairFirm::RepairFirm(const std::string& name, const Warehouse& w, const PriceList& p,
                       const Workshop& ws, const Date& today)
    : name_(name), today_(today), warehouse_(w), prices_(p), workshop_(ws), cash_(), notifier_(), nextId_(1) {}

int RepairFirm::newId() { return nextId_++; }

int RepairFirm::indexOfClient(int id) const {
    for (size_t i = 0; i < clients_.size(); ++i)
        if (clients_[i].getId() == id) return (int)i;
    return -1;
}

int RepairFirm::indexOfCorporate(int id) const {
    for (size_t i = 0; i < corporateClients_.size(); ++i)
        if (corporateClients_[i].getId() == id) return (int)i;
    return -1;
}

int RepairFirm::indexOfEmployeeClient(int id) const {
    for (size_t i = 0; i < employeeClients_.size(); ++i)
        if (employeeClients_[i].getId() == id) return (int)i;
    return -1;
}

int RepairFirm::addClient(const std::string& name, const ContactInfo& c, const Date& birth) {
    c.validate();
    int id = newId();
    clients_.push_back(Client(id, name, c, birth, today_));
    return id;
}

int RepairFirm::addCorporateClient(const std::string& name, const ContactInfo& c, const Date& birth,
                                   const std::string& company, const std::string& taxId,
                                   const ServiceContract& contract) {
    c.validate();
    CorporateClient cc(0, name, c, birth, today_, company, taxId, contract);
    if (!cc.isTaxIdValid()) return -1;   // TODO(exc): InvalidClientDataException("Неверный УНП")
    int id = newId();
    corporateClients_.push_back(CorporateClient(id, name, c, birth, today_, company, taxId, contract));
    return id;
}

bool RepairFirm::hasClient(int id) const {
    return indexOfClient(id) != -1 || indexOfCorporate(id) != -1 || indexOfEmployeeClient(id) != -1;
}

/** @brief Возвращает ссылку на базовую часть: для корпоративного клиента виртуальные методы останутся его. */
Client& RepairFirm::clientById(int id) {
    int i = indexOfClient(id);
    if (i != -1) return clients_[i];
    int e = indexOfEmployeeClient(id);
    if (e != -1) return employeeClients_[e];
    return corporateClients_[indexOfCorporate(id)];   // TODO(exc): ClientNotFoundException, если нет и здесь
}

const Client& RepairFirm::clientById(int id) const {
    int i = indexOfClient(id);
    if (i != -1) return clients_[i];
    int e = indexOfEmployeeClient(id);
    if (e != -1) return employeeClients_[e];
    return corporateClients_[indexOfCorporate(id)];
}

bool RepairFirm::hire(const Technician& t) {
    if (indexOfTechnician(t.getId()) != -1) return false;
    technicians_.push_back(t);
    return true;
}

bool RepairFirm::hire(const Receptionist& r) {
    for (size_t i = 0; i < receptionists_.size(); ++i)
        if (receptionists_[i] == r) return false;
    receptionists_.push_back(r);
    return true;
}

bool RepairFirm::hire(const Manager& m) {
    for (size_t i = 0; i < managers_.size(); ++i)
        if (managers_[i] == m) return false;
    managers_.push_back(m);
    return true;
}

bool RepairFirm::hire(const Accountant& a) {
    for (size_t i = 0; i < accountants_.size(); ++i)
        if (accountants_[i] == a) return false;
    accountants_.push_back(a);
    return true;
}

bool RepairFirm::hire(const Courier& c) {
    for (size_t i = 0; i < couriers_.size(); ++i)
        if (couriers_[i] == c) return false;
    couriers_.push_back(c);
    return true;
}

bool RepairFirm::hire(const EmployeeClient& ec) {
    if (indexOfEmployeeClient(ec.getId()) != -1) return false;
    employeeClients_.push_back(ec);
    return true;
}

int RepairFirm::indexOfTechnician(int id) const {
    for (size_t i = 0; i < technicians_.size(); ++i)
        if (technicians_[i].getId() == id) return (int)i;
    return -1;
}

int RepairFirm::freeTechnicianIndex(const Device& d) const {
    int best = -1;
    for (size_t i = 0; i < technicians_.size(); ++i) {
        if (!technicians_[i].canWorkOn(d) || !technicians_[i].isAvailable()) continue;
        if (best == -1 || technicians_[i].workload() < technicians_[best].workload()) best = (int)i;
    }
    return best;   // TODO(exc): TechnicianNotAvailableException, если -1
}

int RepairFirm::indexOfOrder(int id) const {
    for (size_t i = 0; i < orders_.size(); ++i)
        if (orders_[i].getId() == id) return (int)i;
    return -1;
}

RepairOrder& RepairFirm::orderAt(int index) { return orders_[index]; }
const RepairOrder& RepairFirm::orderAt(int index) const { return orders_[index]; }

void RepairFirm::notifyStatus(int orderIndex) {
    const RepairOrder& o = orders_[orderIndex];
    if (hasClient(o.getClientId())) notifier_.statusChanged(o, clientById(o.getClientId()), today_);
}

/** @brief Клиента после push_back ищем заново, а не храним ссылку. Срок заказа считается по настоящему типу d. */
int RepairFirm::createOrder(int clientId, const Device& d, const std::string& problem, bool urgent) {
    if (!hasClient(clientId)) return -1;   // TODO(exc): ClientNotFoundException(clientId)
    if (!clientById(clientId).canPlaceOrder(orders_))
        return -1;   // TODO(exc): RepairFirmException("У клиента слишком много активных заказов")
    if (receptionists_.empty()) return -1;   // TODO(exc): RepairFirmException("Нет приёмщика")
    int id = newId();
    orders_.push_back(receptionists_[0].acceptDevice(id, clientById(clientId), d, problem, today_, urgent));
    clientById(clientId).addOrder(id);
    notifyStatus((int)orders_.size() - 1);
    return id;
}

/** @brief quickTest вызывается виртуально у d, а не у срезанной копии в заказе. При неремонтопригодности setDiagnosis отменяет заказ. */
bool RepairFirm::runDiagnosis(int orderId, const Device& d) {
    int i = indexOfOrder(orderId);
    if (i == -1) return false;   // TODO(exc): OrderNotFoundException(orderId)
    if (orders_[i].getDevice().getSerial() != d.getSerial()) return false;   // TODO(exc): RepairFirmException("Чужое устройство")
    if (!orders_[i].getStatus().canMoveTo(OrderStatus::DIAGNOSTICS))
        return false;   // TODO(exc): InvalidStatusTransitionException
    int t = freeTechnicianIndex(d);
    if (t == -1) return false;   // TODO(exc): TechnicianNotAvailableException
    orders_[i].setStatus(OrderStatus::DIAGNOSTICS);
    Diagnosis diag(technicians_[t].getId(), today_);
    diag.addDefects(d.quickTest(today_));
    diag.decideRepairable();
    bool ok = orders_[i].setDiagnosis(diag);
    notifyStatus(i);
    return ok;   // TODO(exc): DeviceNotRepairableException придёт из setDiagnosis
}

/** @brief Все проверки до изменений: после них assignTechnician гарантированно проходит. */
int RepairFirm::addTask(int orderId, const std::string& description) {
    int i = indexOfOrder(orderId);
    if (i == -1) return -1;   // TODO(exc): OrderNotFoundException(orderId)
    const OrderStatus& st = orders_[i].getStatus();
    if (st.get() != OrderStatus::IN_REPAIR && !st.canMoveTo(OrderStatus::IN_REPAIR))
        return -1;   // TODO(exc): InvalidStatusTransitionException
    if (!orders_[i].getDiagnosis().isDone()) return -1;   // TODO(exc): RepairFirmException("Нет диагностики")
    if (managers_.empty()) return -1;   // TODO(exc): RepairFirmException("Нет менеджера")
    int t = freeTechnicianIndex(orders_[i].getDevice());
    if (t == -1) return -1;   // TODO(exc): TechnicianNotAvailableException
    int taskId = newId();
    orders_[i].addTask(RepairTask(taskId, description));
    managers_[0].assignTechnician(orders_[i], taskId, technicians_[t]);
    workshop_.assignBench(technicians_[t].getId());
    if (st.get() != OrderStatus::IN_REPAIR) {
        orders_[i].setStatus(OrderStatus::IN_REPAIR);
        notifyStatus(i);
    }
    return taskId;
}

/** @brief Переход в "Ожидание запчастей" возможен только сразу после диагностики; из ремонта статус не меняется. */
bool RepairFirm::issuePart(int orderId, int taskId, const std::string& article, int qty) {
    int i = indexOfOrder(orderId);
    if (i == -1 || qty <= 0) return false;   // TODO(exc): OrderNotFoundException(orderId)
    int k = orders_[i].indexOfTask(taskId);
    if (k == -1 || orders_[i].taskAt(k).isDone()) return false;   // TODO(exc): RepairFirmException("Нет такой задачи")
    if (!warehouse_.contains(article) || warehouse_.find(article).available() < qty) {
        orders_[i].setStatus(OrderStatus::WAITING_PARTS);
        return false;   // TODO(exc): OutOfStockException(article, qty, остаток)
    }
    SparePart part = warehouse_.find(article).getPart();
    if (!part.fits(orders_[i].getDevice()))
        return false;   // TODO(exc): IncompatiblePartException(article, модель)
    warehouse_.take(article, qty);
    orders_[i].taskAt(k).addPart(part, qty, orders_[i].getDevice());
    return true;
}

bool RepairFirm::finishTask(int orderId, int taskId, double hours) {
    int i = indexOfOrder(orderId);
    if (i == -1) return false;   // TODO(exc): OrderNotFoundException(orderId)
    int k = orders_[i].indexOfTask(taskId);
    if (k == -1) return false;   // TODO(exc): RepairFirmException("Нет такой задачи")
    int techId = orders_[i].taskAt(k).getTechnicianId();
    if (!orders_[i].taskAt(k).finish(hours)) return false;   // TODO(exc): исключение придёт из RepairTask::finish
    int t = indexOfTechnician(techId);
    if (t != -1) {
        technicians_[t].finishTask(taskId);
        if (technicians_[t].getActiveTaskIds().empty())
            workshop_.releaseBench(workshop_.benchOf(techId));
    }
    if (orders_[i].allTasksDone() && orders_[i].setStatus(OrderStatus::QUALITY_CHECK))
        notifyStatus(i);
    return true;
}

bool RepairFirm::qualityCheck(int orderId, bool ok) {
    int i = indexOfOrder(orderId);
    if (i == -1) return false;   // TODO(exc): OrderNotFoundException(orderId)
    if (orders_[i].getStatus().get() != OrderStatus::QUALITY_CHECK)
        return false;   // TODO(exc): InvalidStatusTransitionException
    QualityCheck qc(orderId, managers_.empty() ? 0 : managers_[0].getId());
    qc.addItem("Устройство включается");
    qc.addItem("Неисправность устранена");
    qc.addItem("Внешний вид не пострадал");
    qc.mark(0, true);
    qc.mark(1, ok);
    qc.mark(2, true);
    if (!qc.confirm()) {
        orders_[i].setStatus(OrderStatus::IN_REPAIR);
        notifyStatus(i);
        return false;   // TODO(exc): QualityCheckFailedException(orderId)
    }
    orders_[i].setStatus(OrderStatus::READY);
    const RepairOrder& o = orders_[i];
    if (hasClient(o.getClientId())) notifier_.readyForPickup(o, clientById(o.getClientId()), today_);
    return true;
}

std::vector<RepairOrder> RepairFirm::overdueOrders() const {
    std::vector<RepairOrder> result;
    for (size_t i = 0; i < orders_.size(); ++i)
        if (orders_[i].isOverdue(today_)) result.push_back(orders_[i]);
    return result;
}

int RepairFirm::indexOfInvoice(int id) const {
    for (size_t i = 0; i < invoices_.size(); ++i)
        if (invoices_[i].getId() == id) return (int)i;
    return -1;
}

int RepairFirm::indexOfInvoiceForOrder(int orderId) const {
    for (size_t i = 0; i < invoices_.size(); ++i)
        if (invoices_[i].getOrderId() == orderId) return (int)i;
    return -1;
}

/** @brief Скидка клиента считается виртуальным applyDiscount: для компании — по договору или карте. */
int RepairFirm::billOrder(int orderId) {
    int i = indexOfOrder(orderId);
    if (i == -1) return -1;   // TODO(exc): OrderNotFoundException(orderId)
    if (orders_[i].getStatus().get() != OrderStatus::READY)
        return -1;   // TODO(exc): RepairFirmException("Заказ ещё не готов")
    if (indexOfInvoiceForOrder(orderId) != -1) return -1;   // TODO(exc): RepairFirmException("Счёт уже выставлен")
    if (accountants_.empty() || !hasClient(orders_[i].getClientId()))
        return -1;   // TODO(exc): RepairFirmException("Нет бухгалтера")
    int id = newId();
    Invoice inv = accountants_[0].makeInvoice(id, orders_[i], prices_, today_);
    Money sub = inv.subtotal();
    Money discount = sub - clientById(orders_[i].getClientId()).applyDiscount(sub);
    if (!discount.isZero()) inv.addLine(InvoiceLine("Скидка клиента", 1, Money(-discount.getCoins())));
    invoices_.push_back(inv);
    return id;
}

bool RepairFirm::payInvoice(int invoiceId, const Money& sum, const PaymentMethod& m) {
    int i = indexOfInvoice(invoiceId);
    if (i == -1) return false;   // TODO(exc): RepairFirmException("Нет такого счёта")
    Payment p(invoiceId, sum, m, today_);
    p.process(invoices_[i], cash_);
    payments_.push_back(p);
    int clientId = invoices_[i].getClientId();
    if (hasClient(clientId)) clientById(clientId).getCard().addPoints(invoices_[i].total());
    return true;
}

bool RepairFirm::issueToClient(int orderId) {
    int i = indexOfOrder(orderId);
    if (i == -1) return false;   // TODO(exc): OrderNotFoundException(orderId)
    int inv = indexOfInvoiceForOrder(orderId);
    if (inv == -1 || receptionists_.empty()) return false;   // TODO(exc): RepairFirmException("Нет счёта")
    if (!receptionists_[0].issueDevice(orders_[i], invoices_[inv]))
        return false;   // TODO(exc): исключение придёт из Receptionist::issueDevice
    Warranty w(orderId, today_, 3);
    const std::vector<RepairTask>& tasks = orders_[i].getTasks();
    for (size_t k = 0; k < tasks.size(); ++k) {
        const std::vector<UsedPart>& parts = tasks[k].getParts();
        for (size_t p = 0; p < parts.size(); ++p) w.addArticle(parts[p].getArticle());
    }
    warranties_.push_back(w);
    notifyStatus(i);
    return true;
}

Money RepairFirm::monthRevenue(int month, int year) const {
    ReportGenerator r(Date(1, month, year), Date(Date::daysInMonth(month, year), month, year));
    return r.revenue(invoices_);
}

Money RepairFirm::salaryFund(int month) const {
    Money total;
    for (size_t i = 0; i < technicians_.size(); ++i)   total += technicians_[i].calculateSalary(month);
    for (size_t i = 0; i < receptionists_.size(); ++i) total += receptionists_[i].calculateSalary(month);
    for (size_t i = 0; i < managers_.size(); ++i)      total += managers_[i].calculateSalary(month);
    for (size_t i = 0; i < accountants_.size(); ++i)   total += accountants_[i].calculateSalary(month);
    for (size_t i = 0; i < couriers_.size(); ++i)      total += couriers_[i].calculateSalary(month);
    for (size_t i = 0; i < employeeClients_.size(); ++i) total += employeeClients_[i].calculateSalary(month);
    return total;
}

int RepairFirm::bookAppointment(int clientId, const Date& date, int hour, const std::string& purpose) {
    if (!hasClient(clientId)) return -1;   // TODO(exc): ClientNotFoundException(clientId)
    Appointment a(0, clientId, date, hour, purpose);
    if (!a.isValid()) return -1;   // TODO(exc): ScheduleConflictException
    for (size_t i = 0; i < appointments_.size(); ++i)
        if (appointments_[i].conflictsWith(a)) return -1;   // TODO(exc): ScheduleConflictException
    int id = newId();
    appointments_.push_back(Appointment(id, clientId, date, hour, purpose));
    notifier_.remind(appointments_.back(), clientById(clientId), today_);
    return id;
}

/** @brief Связь доставка-курьер обновляется с обеих сторон: assignCourier и pickUp. */
int RepairFirm::arrangeDelivery(int orderId, const Address& to) {
    if (indexOfOrder(orderId) == -1) return -1;   // TODO(exc): OrderNotFoundException(orderId)
    int best = -1;
    for (size_t i = 0; i < couriers_.size(); ++i)
        if (best == -1 || couriers_[i].inTransit() < couriers_[best].inTransit()) best = (int)i;
    if (best == -1) return -1;   // TODO(exc): RepairFirmException("Нет курьера")
    int id = newId();
    Delivery d(id, orderId, workshop_.getAddress(), to);
    d.assignCourier(couriers_[best]);
    couriers_[best].pickUp(id);
    deliveries_.push_back(d);
    return id;
}

bool RepairFirm::completeDelivery(int deliveryId) {
    for (size_t i = 0; i < deliveries_.size(); ++i) {
        if (deliveries_[i].getId() != deliveryId) continue;
        if (!deliveries_[i].complete()) return false;   // TODO(exc): исключение придёт из Delivery::complete
        for (size_t c = 0; c < couriers_.size(); ++c)
            if (couriers_[c].getId() == deliveries_[i].getCourierId()) couriers_[c].deliver(deliveryId);
        return true;
    }
    return false;   // TODO(exc): RepairFirmException("Нет такой доставки")
}

/** @brief После поставки заказы в "Ожидании запчастей" возвращаются в ремонт. */
bool RepairFirm::receivePurchase(PurchaseOrder& po) {
    if (po.isReceived()) return false;
    po.receive(warehouse_, 2);
    for (size_t i = 0; i < orders_.size(); ++i)
        if (orders_[i].getStatus().get() == OrderStatus::WAITING_PARTS && orders_[i].setStatus(OrderStatus::IN_REPAIR))
            notifyStatus((int)i);
    return true;
}

bool RepairFirm::addReview(int clientId, int orderId, int rating, const std::string& text) {
    int i = indexOfOrder(orderId);
    if (i == -1 || orders_[i].getClientId() != clientId) return false;   // TODO(exc): OrderNotFoundException(orderId)
    if (orders_[i].getStatus().get() != OrderStatus::ISSUED) return false;   // TODO(exc): RepairFirmException("Заказ не выдан")
    Review r(clientId, orderId, rating, text);
    if (!r.validate()) return false;   // TODO(exc): исключение придёт из Review::validate
    for (size_t k = 0; k < reviews_.size(); ++k)
        if (reviews_[k] == r) return false;
    reviews_.push_back(r);
    return true;
}

const Date& RepairFirm::today() const { return today_; }
void RepairFirm::setToday(const Date& d) { today_ = d; }
void RepairFirm::advanceDays(int n) { today_ = today_.addDays(n); }

const std::string& RepairFirm::getName() const { return name_; }
const std::vector<Client>& RepairFirm::getClients() const { return clients_; }
const std::vector<CorporateClient>& RepairFirm::getCorporateClients() const { return corporateClients_; }
const std::vector<Technician>& RepairFirm::getTechnicians() const { return technicians_; }
const std::vector<Receptionist>& RepairFirm::getReceptionists() const { return receptionists_; }
const std::vector<Manager>& RepairFirm::getManagers() const { return managers_; }
const std::vector<Accountant>& RepairFirm::getAccountants() const { return accountants_; }
const std::vector<Courier>& RepairFirm::getCouriers() const { return couriers_; }
const std::vector<EmployeeClient>& RepairFirm::getEmployeeClients() const { return employeeClients_; }
const std::vector<RepairOrder>& RepairFirm::getOrders() const { return orders_; }
const std::vector<Invoice>& RepairFirm::getInvoices() const { return invoices_; }
const std::vector<Payment>& RepairFirm::getPayments() const { return payments_; }
const std::vector<Warranty>& RepairFirm::getWarranties() const { return warranties_; }
const std::vector<Appointment>& RepairFirm::getAppointments() const { return appointments_; }
const std::vector<Delivery>& RepairFirm::getDeliveries() const { return deliveries_; }
const std::vector<Review>& RepairFirm::getReviews() const { return reviews_; }
const Warehouse& RepairFirm::getWarehouse() const { return warehouse_; }
const PriceList& RepairFirm::getPriceList() const { return prices_; }
const Workshop& RepairFirm::getWorkshop() const { return workshop_; }
const CashRegister& RepairFirm::getCash() const { return cash_; }
const NotificationService& RepairFirm::getNotifier() const { return notifier_; }
