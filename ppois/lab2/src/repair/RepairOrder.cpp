#include "repair/RepairOrder.h"
#include <sstream>
#include "exceptions/Exceptions.h"

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

void RepairOrder::setStatus(OrderStatus::Status s) { status_.moveTo(s); }

/** @brief Сначала заказ отменяется (если ещё можно), потом бросается исключение: заказ не остаётся "висеть". */
void RepairOrder::setDiagnosis(const Diagnosis& d) {
    diagnosis_ = d;
    if (!diagnosis_.isRepairable()) {
        if (status_.canMoveTo(OrderStatus::Status::Cancelled)) status_.moveTo(OrderStatus::Status::Cancelled);
        throw DeviceNotRepairableException(device_.getSerial());
    }
}

void RepairOrder::addTask(const RepairTask& t) {
    if (status_.isFinal()) throw RepairFirmException("Заказ №" + std::to_string(id_) + " уже завершён");
    tasks_.push_back(t);
}

int RepairOrder::indexOfTask(int taskId) const {
    for (size_t i = 0; i < tasks_.size(); ++i)
        if (tasks_[i].getId() == taskId) return (int)i;
    return -1;
}

bool RepairOrder::hasTask(int taskId) const { return indexOfTask(taskId) != -1; }

RepairTask& RepairOrder::findTask(int taskId) {
    int i = indexOfTask(taskId);
    if (i == -1) throw RepairFirmException("В заказе №" + std::to_string(id_) + " нет задачи №" + std::to_string(taskId));
    return tasks_[i];
}

const RepairTask& RepairOrder::findTask(int taskId) const {
    int i = indexOfTask(taskId);
    if (i == -1) throw RepairFirmException("В заказе №" + std::to_string(id_) + " нет задачи №" + std::to_string(taskId));
    return tasks_[i];
}

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

void RepairOrder::cancel() { status_.moveTo(OrderStatus::Status::Cancelled); }

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

void RepairOrder::print(std::ostream& os) const {
    os << "Заказ №" << id_ << (urgent_ ? " [СРОЧНО]" : "") << ": " << deviceInfo_
       << "\n  Проблема: " << problem_ << "\n  Статус: " << status_
       << ", принят " << created_ << ", срок " << deadline_ << ", клиент #" << clientId_;
    for (size_t i = 0; i < tasks_.size(); ++i)
        os << "\n  " << tasks_[i];
}
