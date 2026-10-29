#include "core/DataSize.h"
#include <cmath>
#include <sstream>
#include <iomanip>

DataSize::DataSize(long long bytes) : bytes_(bytes < 0 ? 0 : bytes) {}
DataSize DataSize::fromKB(double kb) { return DataSize(std::llround(kb * 1024)); }
DataSize DataSize::fromMB(double mb) { return DataSize(std::llround(mb * 1024 * 1024)); }
DataSize DataSize::fromGB(double gb) { return DataSize(std::llround(gb * 1024 * 1024 * 1024)); }

long long DataSize::getBytes() const { return bytes_; }
bool DataSize::isZero() const { return bytes_ == 0; }

double DataSize::percentOf(const DataSize& total) const {
    if (total.bytes_ == 0) return 0.0;
    return 100.0 * bytes_ / total.bytes_;
}

std::string DataSize::toString() const {
    const char* units[] = {"B", "KB", "MB", "GB", "TB"};
    double value = (double)bytes_;
    int unit = 0;
    while (value >= 1024.0 && unit < 4) { value /= 1024.0; ++unit; }
    std::ostringstream out;
    if (unit == 0) out << bytes_ << " B";
    else out << std::fixed << std::setprecision(1) << value << " " << units[unit];
    return out.str();
}

DataSize DataSize::operator+(const DataSize& o) const { return DataSize(bytes_ + o.bytes_); }
DataSize DataSize::operator-(const DataSize& o) const { return DataSize(bytes_ - o.bytes_); } // не ниже 0
DataSize DataSize::operator*(double k) const { return DataSize(std::llround(bytes_ * k)); }
DataSize& DataSize::operator+=(const DataSize& o) { bytes_ += o.bytes_; return *this; }
DataSize& DataSize::operator-=(const DataSize& o) { bytes_ = bytes_ > o.bytes_ ? bytes_ - o.bytes_ : 0; return *this; }
bool DataSize::operator<(const DataSize& o) const { return bytes_ < o.bytes_; }
bool DataSize::operator<=(const DataSize& o) const { return bytes_ <= o.bytes_; }
bool DataSize::operator==(const DataSize& o) const { return bytes_ == o.bytes_; }
bool DataSize::operator!=(const DataSize& o) const { return !(*this == o); }
std::ostream& operator<<(std::ostream& os, const DataSize& s) { return os << s.toString(); }
