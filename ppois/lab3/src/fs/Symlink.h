#pragma once
#include <string>
#include "fs/FsEntry.h"

/**
 * @brief Символическая ссылка: путь к цели и её ID. targetId_ = 0 — цель удалена, ссылка битая.
 * Циклы ссылок (a -> b -> a) ссылка сама не видит, их ловит FileSystem.
 */
class Symlink final : public FsEntry {
public:
    /**
     * @brief Путь нормализуется через Path::parse: "/home/./ivan" -> "/home/ivan".
     * @throws InvalidPathException путь недопустимый.
     * @throws InvalidOperationException targetId отрицательный.
     */
    Symlink(int id, const std::string& name, int parentId, int ownerId,
            const Permission& permission, const Timestamp& now,
            const std::string& targetPath, int targetId);

    std::string typeName() const override;
    /** @brief Длина пути цели в байтах, как у ссылок в настоящих файловых системах. */
    DataSize size() const override;
    char typeChar() const override;
    /** @brief "→ /home/ivan/a.txt", у битой ссылки с пометкой "(битая)". */
    std::string preview(int maxChars) const override;

    /** @brief ID цели. @throws EntryNotFoundException ссылка битая (цель удалена). */
    int resolve() const;
    bool isBroken() const;
    /** @brief Отметить, что цель удалена: targetId_ = 0. */
    void markBroken();
    /**
     * @brief Направить ссылку на другую цель.
     * @throws InvalidOperationException id не положительный. @throws InvalidPathException путь недопустимый.
     */
    void retarget(int id, const std::string& path, const Timestamp& now);

    const std::string& getTargetPath() const;
    int getTargetId() const;

private:
    std::string targetPath_;
    int targetId_;
};
