#include "storage/Volume.h"
#include "exceptions/Exceptions.h"

Volume::Volume(const std::string& name, const std::string& mountPath, int deviceId, bool readOnly)
    : name_(name), mountPath_(Path::parse(mountPath)), deviceId_(deviceId),
      readOnly_(readOnly), mounted_(true) {}

void Volume::mount() { mounted_ = true; }
void Volume::unmount() { mounted_ = false; }
void Volume::setReadOnly(bool v) { readOnly_ = v; }

void Volume::checkWritable() const {
    if (!mounted_) throw ReadOnlyVolumeException(name_ + " (не смонтирован)");
    if (readOnly_) throw ReadOnlyVolumeException(name_);
}

bool Volume::contains(const Path& p) const { return p.startsWith(mountPath_); }

const std::string& Volume::getName() const { return name_; }
const Path& Volume::getMountPath() const { return mountPath_; }
int Volume::getDeviceId() const { return deviceId_; }
bool Volume::isReadOnly() const { return readOnly_; }
bool Volume::isMounted() const { return mounted_; }

bool Volume::operator==(const Volume& o) const { return name_ == o.name_; }
bool Volume::operator!=(const Volume& o) const { return !(*this == o); }

std::ostream& operator<<(std::ostream& os, const Volume& v) {
    os << v.name_ << " -> " << v.mountPath_ << " (устройство #" << v.deviceId_ << ", "
       << (v.readOnly_ ? "только чтение" : "чтение и запись") << ")";
    if (!v.mounted_) os << " [НЕ СМОНТИРОВАН]";
    return os;
}
