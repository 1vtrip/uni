#include "meta/ArchiveEntry.h"
#include <iomanip>

ArchiveEntry::ArchiveEntry(const std::string& name, const DataSize& originalSize,
                           const DataSize& compressedSize)
    : name_(name), originalSize_(originalSize), compressedSize_(compressedSize) {}

/** @brief Приведение к double обязательно: иначе 300 / 1000 при делении целых даст 0. */
double ArchiveEntry::ratio() const {
    if (originalSize_.isZero()) return 0.0;
    return (double)compressedSize_.getBytes() / originalSize_.getBytes();
}

/** @brief DataSize::operator- не опускается ниже нуля, поэтому выросший файл даёт 0 B. */
DataSize ArchiveEntry::saved() const { return originalSize_ - compressedSize_; }

double ArchiveEntry::savedPercent() const {
    if (originalSize_.isZero()) return 0.0;
    double percent = (1.0 - ratio()) * 100.0;
    return percent < 0 ? 0.0 : percent;
}

bool ArchiveEntry::isCompressed() const { return compressedSize_ < originalSize_; }

const std::string& ArchiveEntry::getName() const { return name_; }
const DataSize& ArchiveEntry::getOriginalSize() const { return originalSize_; }
const DataSize& ArchiveEntry::getCompressedSize() const { return compressedSize_; }

bool ArchiveEntry::operator==(const ArchiveEntry& o) const { return name_ == o.name_; }
bool ArchiveEntry::operator!=(const ArchiveEntry& o) const { return !(*this == o); }

std::ostream& operator<<(std::ostream& os, const ArchiveEntry& e) {
    os << e.name_ << ' ' << e.originalSize_ << " -> " << e.compressedSize_;
    if (e.isCompressed()) {
        std::ios::fmtflags oldFlags = os.flags();
        std::streamsize oldPrecision = os.precision();
        os << " (сжато на " << std::fixed << std::setprecision(0) << e.savedPercent() << "%)";
        os.flags(oldFlags);
        os.precision(oldPrecision);
    } else {
        os << " (без сжатия)";
    }
    return os;
}
