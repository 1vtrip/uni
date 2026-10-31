#include "storage/StorageDevice.h"
#include "exceptions/Exceptions.h"

StorageDevice::StorageDevice(int id, const std::string& name, const std::string& type,
                             const DataSize& capacity, bool online)
    : id_(id), name_(name), type_(type), capacity_(capacity), used_(0), online_(online) {
    if (type != "SSD" && type != "HDD")
        throw InvalidOperationException("тип накопителя должен быть SSD или HDD, а не " + type);
}

/** @brief Сначала проверяются оба условия, и только потом меняется used_: при ошибке ничего не занято. */
void StorageDevice::allocate(const DataSize& s) {
    if (!online_) throw InsufficientStorageException(name_ + " (отключено)");
    if (freeSpace() < s)
        throw InsufficientStorageException(name_ + " (нужно " + s.toString()
                                           + ", свободно " + freeSpace().toString() + ")");
    used_ += s;
}

void StorageDevice::free(const DataSize& s) { used_ -= s; }
DataSize StorageDevice::freeSpace() const { return capacity_ - used_; }
double StorageDevice::usagePercent() const { return used_.percentOf(capacity_); }

/** @brief Скорость переводится в байты в секунду, деление в double. */
double StorageDevice::estimateWriteSeconds(const DataSize& s) const {
    double mbPerSecond = type_ == "SSD" ? 500.0 : 120.0;
    return s.getBytes() / (mbPerSecond * 1024 * 1024);
}

void StorageDevice::setOnline(bool v) { online_ = v; }
bool StorageDevice::isOnline() const { return online_; }

int StorageDevice::getId() const { return id_; }
const std::string& StorageDevice::getName() const { return name_; }
const std::string& StorageDevice::getType() const { return type_; }
const DataSize& StorageDevice::getCapacity() const { return capacity_; }
const DataSize& StorageDevice::getUsed() const { return used_; }

bool StorageDevice::operator==(const StorageDevice& o) const { return id_ == o.id_; }
bool StorageDevice::operator!=(const StorageDevice& o) const { return !(*this == o); }

std::ostream& operator<<(std::ostream& os, const StorageDevice& d) {
    os << d.name_ << " (" << d.type_ << "): " << d.used_ << " / " << d.capacity_
       << ", " << (int)d.usagePercent() << "%";
    if (!d.online_) os << " [ОТКЛЮЧЕНО]";
    return os;
}
