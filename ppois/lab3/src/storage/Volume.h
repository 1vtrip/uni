#pragma once
#include <iostream>
#include <string>
#include "core/Path.h"

/** @brief Том: часть дерева, смонтированная в mountPath_ и лежащая на устройстве deviceId_. */
class Volume {
public:
    /** @brief Том создаётся смонтированным. @throws InvalidPathException путь недопустимый. */
    Volume(const std::string& name, const std::string& mountPath, int deviceId, bool readOnly = false);

    void mount();
    void unmount();
    void setReadOnly(bool v);
    /** @brief Можно ли писать. @throws ReadOnlyVolumeException том только для чтения или не смонтирован. */
    void checkWritable() const;
    /** @brief Лежит ли путь внутри тома: p начинается с mountPath_. */
    bool contains(const Path& p) const;

    const std::string& getName() const;
    const Path& getMountPath() const;
    int getDeviceId() const;
    bool isReadOnly() const;
    bool isMounted() const;

    /** @brief Тома равны, если совпадают имена. */
    bool operator==(const Volume& o) const;
    bool operator!=(const Volume& o) const;
    /** @brief "data -> /home (устройство #1, чтение и запись)". */
    friend std::ostream& operator<<(std::ostream& os, const Volume& v);

private:
    std::string name_;
    Path mountPath_;
    int deviceId_;
    bool readOnly_;
    bool mounted_;
};
