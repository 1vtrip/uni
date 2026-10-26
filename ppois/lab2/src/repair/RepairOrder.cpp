#include "repair/RepairOrder.h"
#include <sstream>

/**
 * @brief Здесь device ещё ссылается на наследника, поэтому виртуальные вызовы дают его версии:
 * срок, цену диагностики и полное описание. В поле device_ попадает только часть Device.
 * Срочный заказ делается вдвое быстрее, но не меньше чем за 1 день.
 */
RepairOrder::RepairOrder(int id, int clientId, const Device& device, const std::string& problem,
                         const Date& created, bool urgent)
    : id_(id), clientId_(clientId), device_(device), problem_(problem), status_(),
      created_(created), deadline_(created), diagnosis_(), diagnosticPrice_(device.diagnosticPrice()),
      deviceInfo_(), urgent_(urgent) {
    int repairDays = device.estimateRepairDays();
    int days = urgent ? repairDays / 2 : repairDays;
    if (days < 1) days = 1;
    deadline_ = created.addDays(days);
    std::ostringstream out;
    device.print(out);
    deviceInfo_ = out.str();
}

bool RepairOrder::setStatus(int s) {
    return status_.moveTo(s);   // TODO(exc): исключение придёт из OrderStatus::moveTo
}

bool RepairOrder::setDiagnosis(const Diagnosis& d) {
    diagnosis_ = d;
    if (!diagnosis_.isRepairable()) {
        cancel();
        return false;   // TODO(exc): DeviceNotRepairableException(device_.getSerial())
    }
    return true;
}

bool RepairOrder::addTask(const RepairTask& t) {
    if (status_.isFinal()) return false;   // TODO(exc): RepairFirmException("Заказ уже завершён")
    tasks_.push_back(t);
    return true;
}

int RepairOrder::indexOfTask(int taskId) const {
    for (size_t i = 0; i < tasks_.size(); ++i)
        if (tasks_[i].getId() == taskId) return (int)i;
    return -1;
}

RepairTask& RepairOrder::taskAt(int index) { return tasks_[index]; }
const RepairTask& RepairOrder::taskAt(int index) const { return tasks_[index]; }

bool RepairOrder::allTasksDone() const {
    if (tasks_.empty()) return false;
    for (size_t i = 0; i < tasks_.size(); ++i)
        if (!tasks_[i].isDone()) return false;
    return true;
}

Money RepairOrder::totalCost(const PriceList& prices) const {
    Money sum = diagnosticPrice_;
    for (size_t i = 0; i < tasks_.size(); ++i) sum += tasks_[i].cost(prices, urgent_);
    return sum;
}

bool RepairOrder::isOverdue(const Date& today) const {
    return !status_.isFinal() && deadline_ < today;
}

int RepairOrder::daysLeft(const Date& today) const { return today.daysTo(deadline_); }

bool RepairOrder::cancel() { return status_.moveTo(OrderStatus::CANCELLED); }

int RepairOrder::getId() const { return id_; }
int RepairOrder::getClientId() const { return clientId_; }
const Device& RepairOrder::getDevice() const { return device_; }
const std::string& RepairOrder::getProblem() const { return problem_; }
const OrderStatus& RepairOrder::getStatus() const { return status_; }
const Date& RepairOrder::getCreated() const { return created_; }
const Date& RepairOrder::getDeadline() const { return deadline_; }
const Diagnosis& RepairOrder::getDiagnosis() const { return diagnosis_; }
const std::vector<RepairTask>& RepairOrder::getTasks() const { return tasks_; }
const Money& RepairOrder::getDiagnosticPrice() const { return diagnosticPrice_; }
const std::string& RepairOrder::getDeviceInfo() const { return deviceInfo_; }
bool RepairOrder::isUrgent() const { return urgent_; }

bool RepairOrder::operator==(const RepairOrder& other) const { return id_ == other.id_; }
bool RepairOrder::operator!=(const RepairOrder& other) const { return !(*this == other); }

std::ostream& operator<<(std::ostream& os, const RepairOrder& o) {
    os << "Заказ №" << o.id_ << (o.urgent_ ? " [СРОЧНО]" : "") << ": " << o.deviceInfo_
       << "\n  Проблема: " << o.problem_ << "\n  Статус: " << o.status_
       << ", принят " << o.created_ << ", срок " << o.deadline_ << ", клиент #" << o.clientId_;
    for (size_t i = 0; i < o.tasks_.size(); ++i)
        os << "\n  " << o.tasks_[i];
    return os;
}
