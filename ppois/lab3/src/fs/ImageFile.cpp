#include "fs/ImageFile.h"
#include "exceptions/Exceptions.h"
#include <cctype>
#include <cmath>

/** @brief Строка в верхнем регистре: "jpg" -> "JPG". */
static std::string toUpper(const std::string& s) {
    std::string result = s;
    for (size_t i = 0; i < result.size(); ++i)
        result[i] = (char)std::toupper((unsigned char)result[i]);
    return result;
}

ImageFile::ImageFile(int id, const std::string& name, int parentId, int ownerId,
                     const Permission& permission, const Timestamp& now,
                     int width, int height, const std::string& format, int colorDepth)
    : File(id, name, parentId, ownerId, permission, now, MimeType::fromExtension(format)),
      width_(width), height_(height), format_(toUpper(format)), colorDepth_(colorDepth) {
    if (width <= 0 || height <= 0)
        throw InvalidOperationException("размеры изображения должны быть положительными");
    if (colorDepth <= 0)
        throw InvalidOperationException("глубина цвета должна быть положительной");
}

std::string ImageFile::typeName() const { return "Изображение"; }

/** @brief 8000 × 8000 × 32 ≈ 2 млрд — больше максимума int, поэтому умножение идёт в long long. */
DataSize ImageFile::size() const {
    long long bytes = (long long)width_ * height_ * colorDepth_ / 8;
    if (isCompressedFormat()) return DataSize(bytes) * 0.1;
    return DataSize(bytes);
}

std::string ImageFile::preview(int) const {
    return "[Изображение " + std::to_string(width_) + "x" + std::to_string(height_) + " " + format_ + "]";
}

void ImageFile::resize(int w, int h, const Timestamp& now) {
    if (w <= 0 || h <= 0) throw InvalidOperationException("размеры изображения должны быть положительными");
    width_ = w;
    height_ = h;
    touch(now);
}

double ImageFile::aspectRatio() const { return (double)width_ / height_; }
double ImageFile::megapixels() const { return (double)width_ * height_ / 1000000.0; }

/**
 * @brief Коэффициент = maxSide / большая сторона (в double, иначе целое деление даст 0).
 * Маленькие картинки не увеличиваются; каждая сторона после округления не меньше 1,
 * иначе у картинки 10000x10 высота стала бы 0.
 */
std::string ImageFile::thumbnailSize(int maxSide) const {
    int longest = width_ > height_ ? width_ : height_;
    double scale = longest > maxSide ? (double)maxSide / longest : 1.0;
    long long w = std::llround(width_ * scale);
    long long h = std::llround(height_ * scale);
    if (w < 1) w = 1;
    if (h < 1) h = 1;
    return std::to_string(w) + "x" + std::to_string(h);
}

bool ImageFile::isCompressedFormat() const { return format_ == "JPG" || format_ == "JPEG"; }

int ImageFile::getWidth() const { return width_; }
int ImageFile::getHeight() const { return height_; }
const std::string& ImageFile::getFormat() const { return format_; }
int ImageFile::getColorDepth() const { return colorDepth_; }
