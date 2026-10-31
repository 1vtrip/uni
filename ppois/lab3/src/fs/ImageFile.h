#pragma once
#include <string>
#include "fs/File.h"

/** @brief Изображение: размеры в пикселях, формат и глубина цвета. Размер файла считается из них. */
class ImageFile : public File {
public:
    /**
     * @brief Формат приводится к верхнему регистру: "png" -> "PNG".
     * @throws InvalidOperationException ширина, высота или глубина цвета не положительные.
     */
    ImageFile(int id, const std::string& name, int parentId, int ownerId,
              const Permission& permission, const Timestamp& now,
              int width, int height, const std::string& format = "PNG", int colorDepth = 24);

    std::string typeName() const override;
    /** @brief width × height × colorDepth / 8 байт; JPG сжат, поэтому × 0,1. Считается в long long. */
    DataSize size() const override;
    /** @brief "[Изображение 1920x1080 PNG]". */
    std::string preview(int maxChars) const override;

    /** @brief Изменить размеры. @throws InvalidOperationException неположительная ширина или высота. */
    void resize(int w, int h, const Timestamp& now);
    /** @brief Отношение ширины к высоте: 1920x1080 -> 1.78. */
    double aspectRatio() const;
    /** @brief Число мегапикселей: 1920x1080 -> 2.07. */
    double megapixels() const;
    /** @brief Размер миниатюры, у которой большая сторона не больше maxSide: 1920x1080, 160 -> "160x90". */
    std::string thumbnailSize(int maxSide) const;
    /** @brief Сжатый ли формат (JPG или JPEG). */
    bool isCompressedFormat() const;

    int getWidth() const;
    int getHeight() const;
    const std::string& getFormat() const;
    int getColorDepth() const;

private:
    int width_;
    int height_;
    std::string format_;
    int colorDepth_;
};
