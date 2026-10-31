#include "ops/Compressor.h"
#include "exceptions/Exceptions.h"
#include "fs/File.h"

Compressor::Compressor(const std::string& algorithm, int level) : algorithm_(algorithm), level_(level) {
    if (algorithm.empty()) throw InvalidOperationException("имя алгоритма сжатия не может быть пустым");
    checkLevel(level);
}

void Compressor::checkLevel(int level) {
    if (level < MIN_LEVEL || level > MAX_LEVEL)
        throw InvalidOperationException("уровень сжатия должен быть от 1 до 9, а не " + std::to_string(level));
}

/**
 * @brief «Улучшает на 2%» понимается как умножение на 0,98, а не вычитание 0,02:
 * так степень сжатия остаётся положительной при любых исходных числах.
 */
double Compressor::estimateRatio(const MimeType& m) const {
    if (m.isArchive()) return 1.0;
    double ratio = m.isText() ? 0.3 : m.isImage() ? 0.95 : 0.6;
    for (int level = 6; level <= level_; ++level) ratio *= 0.98;
    return ratio;
}

ArchiveEntry Compressor::compress(const File& f) const {
    DataSize original = f.size();
    return ArchiveEntry(f.getName(), original, original * estimateRatio(f.getMime()));
}

double Compressor::estimateSeconds(const DataSize& s) const {
    double mbPerSecond = 100.0 / level_;
    return s.getBytes() / (mbPerSecond * 1024 * 1024);
}

void Compressor::setLevel(int level) {
    checkLevel(level);
    level_ = level;
}

const std::string& Compressor::getAlgorithm() const { return algorithm_; }
int Compressor::getLevel() const { return level_; }

std::ostream& operator<<(std::ostream& os, const Compressor& c) {
    return os << c.algorithm_ << ", уровень " << c.level_;
}
