#include "service/RepairFirm.h"
#include <sstream>
#include "repair/Diagnosis.h"
#include "repair/QualityCheck.h"
#include "service/ReportGenerator.h"
#include "exceptions/Exceptions.h"

using S = OrderStatus::Status;

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

int RepairFirm::indexOfTechnician(int id) const {
    for (size_t i = 0; i < technicians_.size(); ++i)
        if (technicians_[i].getId() == id) return (int)i;
    return -1;
}

int RepairFirm::indexOfOrder(int id) const {
    for (size_t i = 0; i < orders_.size(); ++i)
        if (orders_[i].getId() == id) return (int)i;
    return -1;
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

void RepairFirm::notifyStatus(const RepairOrder& o) {
    if (hasClient(o.getClientId())) notifier_.statusChanged(o, findClient(o.getClientId()), today_);
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
    if (!cc.isTaxIdValid()) throw InvalidClientDataException("УНП " + taxId);
    int id = newId();
    corporateClients_.push_back(CorporateClient(id, name, c, birth, today_, company, taxId, contract));
    return id;
}

bool RepairFirm::hasClient(int id) const {
    return indexOfClient(id) != -1 || indexOfCorporate(id) != -1 || indexOfEmployeeClient(id) != -1;
}

/** @brief Возвращает ссылку на базовую часть: для наследников виртуальные методы останутся их. */
Client& RepairFirm::findClient(int id) {
    int i = indexOfClient(id);
    if (i != -1) return clients_[i];
    i = indexOfEmployeeClient(id);
    if (i != -1) return employeeClients_[i];
    i = indexOfCorporate(id);
    if (i != -1) return corporateClients_[i];
    throw ClientNotFoundException(id);
}

const Client& RepairFirm::findClient(int id) const {
    int i = indexOfClient(id);
    if (i != -1) return clients_[i];
    i = indexOfEmployeeClient(id);
    if (i != -1) return employeeClients_[i];
    i = indexOfCorporate(id);
    if (i != -1) return corporateClients_[i];
    throw ClientNotFoundException(id);
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

Technician& RepairFirm::freeTechnician(const Device& d) {
    int best = -1;
    for (size_t i = 0; i < technicians_.size(); ++i) {
        if (!technicians_[i].canWorkOn(d) || !technicians_[i].isAvailable()) continue;
        if (best == -1 || technicians_[i].workload() < technicians_[best].workload()) best = (int)i;
    }
    if (best == -1) throw TechnicianNotAvailableException("нет свободного мастера для «" + d.getCategory().getName() + "»");
    return technicians_[best];
}

bool RepairFirm::hasOrder(int id) const { return indexOfOrder(id) != -1; }

RepairOrder& RepairFirm::findOrder(int id) {
    int i = indexOfOrder(id);
    if (i == -1) throw OrderNotFoundException(id);
    return orders_[i];
}

const RepairOrder& RepairFirm::findOrder(int id) const {
    int i = indexOfOrder(id);
    if (i == -1) throw OrderNotFoundException(id);
    return orders_[i];
}

/** @brief Ссылку на клиента после push_back в orders_ берём заново, а не храним. */
int RepairFirm::createOrder(int clientId, const Device& d, const std::string& problem, bool urgent) {
    const Client& c = findClient(clientId);
    if (!c.canPlaceOrder(orders_))
        throw RepairFirmException("У клиента " + c.getName() + " уже " + std::to_string(c.activeOrders(orders_))
                                  + " активных заказов (лимит " + std::to_string(c.maxActiveOrders()) + ")");
    if (receptionists_.empty()) throw RepairFirmException("В фирме нет приёмщика");
    int id = newId();
    orders_.push_back(receptionists_[0].acceptDevice(id, c, d, problem, today_, urgent));
    findClient(clientId).addOrder(id);
    notifyStatus(orders_.back());
    return id;
}

/**
 * @brief Сначала поиск мастера и смена статуса (обе могут бросить до других изменений).
 * При неремонтопригодности setDiagnosis отменяет заказ и бросает исключение: оно ловится,
 * клиенту уходит уведомление об отмене, и исключение пробрасывается дальше (throw;).
 */
void RepairFirm::runDiagnosis(int orderId, const Device& d) {
    RepairOrder& o = findOrder(orderId);
    if (o.getDevice().getSerial() != d.getSerial())
        throw RepairFirmException("Устройство " + d.getSerial() + " не относится к заказу №" + std::to_string(orderId));
    Technician& t = freeTechnician(d);
    o.setStatus(S::Diagnostics);
    Diagnosis diag(t.getId(), today_);
    diag.addDefects(d.quickTest(today_));
    diag.decideRepairable();
    try {
        o.setDiagnosis(diag);
    } catch (const DeviceNotRepairableException&) {
        notifyStatus(o);
        throw;
    }
    notifyStatus(o);
}

/**
 * @brief Все проверки до изменений: статус, диагноз, менеджер, свободный мастер.
 * После них assignTechnician гарантированно проходит. Место в мастерской необязательно:
 * занимается, только если есть свободное.
 */
int RepairFirm::addTask(int orderId, const std::string& description) {
    RepairOrder& o = findOrder(orderId);
    bool inRepair = o.getStatus().get() == S::InRepair;
    if (!inRepair && !o.getStatus().canMoveTo(S::InRepair))
        throw InvalidStatusTransitionException(o.getStatus().toString(), OrderStatus::toString(S::InRepair));
    if (!o.getDiagnosis().isDone()) throw RepairFirmException("Заказ №" + std::to_string(orderId) + " ещё не прошёл диагностику");
    if (managers_.empty()) throw RepairFirmException("В фирме нет менеджера");
    Technician& t = freeTechnician(o.getDevice());
    int taskId = newId();
    o.addTask(RepairTask(taskId, description));
    managers_[0].assignTechnician(o, taskId, t);
    if (workshop_.freeBenches() > 0 || workshop_.benchOf(t.getId()) != -1)
        workshop_.assignBench(t.getId());
    if (!inRepair) {
        o.setStatus(S::InRepair);
        notifyStatus(o);
    }
    return taskId;
}

/** @brief Переход в "Ожидание запчастей" возможен только сразу после диагностики; из ремонта статус не меняется. */
void RepairFirm::issuePart(int orderId, int taskId, const std::string& article, int qty) {
    RepairOrder& o = findOrder(orderId);
    RepairTask& task = o.findTask(taskId);
    if (task.isDone()) throw RepairFirmException("Задача №" + std::to_string(taskId) + " уже закрыта");
    int available = warehouse_.contains(article) ? warehouse_.find(article).available() : 0;
    if (available < qty) {
        if (o.getStatus().canMoveTo(S::WaitingParts)) {
            o.setStatus(S::WaitingParts);
            notifyStatus(o);
        }
        throw OutOfStockException(article, qty, available);
    }
    SparePart part = warehouse_.find(article).getPart();
    if (!part.fits(o.getDevice())) throw IncompatiblePartException(article, o.getDevice().getModel());
    warehouse_.take(article, qty);
    task.addPart(part, qty, o.getDevice());
}

void RepairFirm::finishTask(int orderId, int taskId, double hours) {
    RepairOrder& o = findOrder(orderId);
    RepairTask& task = o.findTask(taskId);
    int techId = task.getTechnicianId();
    task.finish(hours);
    int t = indexOfTechnician(techId);
    if (t != -1) {
        technicians_[t].finishTask(taskId);
        if (technicians_[t].getActiveTaskIds().empty())
            workshop_.releaseBench(workshop_.benchOf(techId));
    }
    if (o.allTasksDone() && o.getStatus().canMoveTo(S::QualityCheck)) {
        o.setStatus(S::QualityCheck);
        notifyStatus(o);
    }
}

/** @brief При провале confirm() бросает исключение: заказ возвращается в ремонт, клиент уведомлён, исключение уходит дальше. */
void RepairFirm::qualityCheck(int orderId, bool ok) {
    RepairOrder& o = findOrder(orderId);
    if (o.getStatus().get() != S::QualityCheck)
        throw InvalidStatusTransitionException(o.getStatus().toString(), OrderStatus::toString(S::Ready));
    QualityCheck qc(orderId, managers_.empty() ? 0 : managers_[0].getId());
    qc.addItem("Устройство включается");
    qc.addItem("Неисправность устранена");
    qc.addItem("Внешний вид не пострадал");
    qc.mark(0, true);
    qc.mark(1, ok);
    qc.mark(2, true);
    try {
        qc.confirm();
    } catch (const QualityCheckFailedException&) {
        o.setStatus(S::InRepair);
        notifyStatus(o);
        throw;
    }
    o.setStatus(S::Ready);
    if (hasClient(o.getClientId())) notifier_.readyForPickup(o, findClient(o.getClientId()), today_);
}

std::vector<RepairOrder> RepairFirm::overdueOrders() const {
    std::vector<RepairOrder> result;
    for (size_t i = 0; i < orders_.size(); ++i)
        if (orders_[i].isOverdue(today_)) result.push_back(orders_[i]);
    return result;
}

/** @brief Скидка клиента считается виртуальным applyDiscount: для компании — по договору или карте. */
int RepairFirm::billOrder(int orderId) {
    const RepairOrder& o = findOrder(orderId);
    if (o.getStatus().get() != S::Ready)
        throw RepairFirmException("Заказ №" + std::to_string(orderId) + " ещё не готов (статус «" + o.getStatus().toString() + "»)");
    if (indexOfInvoiceForOrder(orderId) != -1) throw RepairFirmException("Счёт по заказу №" + std::to_string(orderId) + " уже выставлен");
    if (accountants_.empty()) throw RepairFirmException("В фирме нет бухгалтера");
    const Client& c = findClient(o.getClientId());
    int id = newId();
    Invoice inv = accountants_[0].makeInvoice(id, o, prices_, today_);
    Money sub = inv.subtotal();
    Money discount = sub - c.applyDiscount(sub);
    if (!discount.isZero()) inv.addLine(InvoiceLine("Скидка клиента", 1, Money(-discount.getCoins())));
    invoices_.push_back(inv);
    return id;
}

const Invoice& RepairFirm::findInvoice(int id) const {
    int i = indexOfInvoice(id);
    if (i == -1) throw RepairFirmException("Счёт №" + std::to_string(id) + " не найден");
    return invoices_[i];
}

void RepairFirm::payInvoice(int invoiceId, const Money& sum, const PaymentMethod& m) {
    int i = indexOfInvoice(invoiceId);
    if (i == -1) throw RepairFirmException("Счёт №" + std::to_string(invoiceId) + " не найден");
    Payment p(invoiceId, sum, m, today_);
    p.process(invoices_[i], cash_);
    payments_.push_back(p);
    int clientId = invoices_[i].getClientId();
    if (hasClient(clientId)) findClient(clientId).getCard().addPoints(invoices_[i].total());
}

void RepairFirm::issueToClient(int orderId) {
    RepairOrder& o = findOrder(orderId);
    int inv = indexOfInvoiceForOrder(orderId);
    if (inv == -1) throw RepairFirmException("По заказу №" + std::to_string(orderId) + " не выставлен счёт");
    if (receptionists_.empty()) throw RepairFirmException("В фирме нет приёмщика");
    receptionists_[0].issueDevice(o, invoices_[inv]);
    Warranty w(orderId, today_, 3);
    const std::vector<RepairTask>& tasks = o.getTasks();
    for (size_t k = 0; k < tasks.size(); ++k) {
        const std::vector<UsedPart>& parts = tasks[k].getParts();
        for (size_t p = 0; p < parts.size(); ++p) w.addArticle(parts[p].getArticle());
    }
    warranties_.push_back(w);
    notifyStatus(o);
}

Money RepairFirm::monthRevenue(int month, int year) const {
    ReportGenerator r(Date(1, month, year), Date(Date::daysInMonth(month, year), month, year));
    return r.revenue(invoices_);
}

Money RepairFirm::salaryFund(int month) const {
    Money total;
    for (size_t i = 0; i < technicians_.size(); ++i)     total += technicians_[i].calculateSalary(month);
    for (size_t i = 0; i < receptionists_.size(); ++i)   total += receptionists_[i].calculateSalary(month);
    for (size_t i = 0; i < managers_.size(); ++i)        total += managers_[i].calculateSalary(month);
    for (size_t i = 0; i < accountants_.size(); ++i)     total += accountants_[i].calculateSalary(month);
    for (size_t i = 0; i < couriers_.size(); ++i)        total += couriers_[i].calculateSalary(month);
    for (size_t i = 0; i < employeeClients_.size(); ++i) total += employeeClients_[i].calculateSalary(month);
    return total;
}

int RepairFirm::bookAppointment(int clientId, const Date& date, int hour, const std::string& purpose) {
    const Client& c = findClient(clientId);
    std::ostringstream when;
    when << date << " в " << hour << ":00";
    if (!Appointment::isWorkingHour(hour)) throw ScheduleConflictException(when.str() + " — нерабочее время");
    Appointment a(0, clientId, date, hour, purpose);
    for (size_t i = 0; i < appointments_.size(); ++i)
        if (appointments_[i].conflictsWith(a)) throw ScheduleConflictException(when.str() + " уже занято");
    int id = newId();
    appointments_.push_back(Appointment(id, clientId, date, hour, purpose));
    notifier_.remind(appointments_.back(), c, today_);
    return id;
}

/** @brief Связь доставка-курьер обновляется с обеих сторон: assignCourier и pickUp. */
int RepairFirm::arrangeDelivery(int orderId, const Address& to) {
    findOrder(orderId);
    if (couriers_.empty()) throw RepairFirmException("В фирме нет курьера");
    size_t best = 0;
    for (size_t i = 1; i < couriers_.size(); ++i)
        if (couriers_[i].inTransit() < couriers_[best].inTransit()) best = i;
    int id = newId();
    Delivery d(id, orderId, workshop_.getAddress(), to);
    d.assignCourier(couriers_[best]);
    couriers_[best].pickUp(id);
    deliveries_.push_back(d);
    return id;
}

void RepairFirm::completeDelivery(int deliveryId) {
    for (size_t i = 0; i < deliveries_.size(); ++i) {
        if (deliveries_[i].getId() != deliveryId) continue;
        deliveries_[i].complete();
        for (size_t c = 0; c < couriers_.size(); ++c)
            if (couriers_[c].getId() == deliveries_[i].getCourierId()) couriers_[c].deliver(deliveryId);
        return;
    }
    throw RepairFirmException("Доставка №" + std::to_string(deliveryId) + " не найдена");
}

/** @brief После поставки заказы в "Ожидании запчастей" возвращаются в ремонт. */
void RepairFirm::receivePurchase(PurchaseOrder& po) {
    if (po.isReceived()) throw RepairFirmException("Поставка №" + std::to_string(po.getId()) + " уже принята");
    po.receive(warehouse_, 2);
    for (size_t i = 0; i < orders_.size(); ++i) {
        if (orders_[i].getStatus().get() == S::WaitingParts) {
            orders_[i].setStatus(S::InRepair);
            notifyStatus(orders_[i]);
        }
    }
}

void RepairFirm::addReview(int clientId, int orderId, int rating, const std::string& text) {
    const RepairOrder& o = findOrder(orderId);
    if (o.getClientId() != clientId)
        throw RepairFirmException("Заказ №" + std::to_string(orderId) + " не принадлежит клиенту #" + std::to_string(clientId));
    if (o.getStatus().get() != S::Issued) throw RepairFirmException("Заказ №" + std::to_string(orderId) + " ещё не выдан");
    Review r(clientId, orderId, rating, text);
    r.validate();
    for (size_t k = 0; k < reviews_.size(); ++k)
        if (reviews_[k] == r) throw RepairFirmException("Отзыв о заказе №" + std::to_string(orderId) + " уже оставлен");
    reviews_.push_back(r);
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
