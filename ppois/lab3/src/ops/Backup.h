#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "core/DataSize.h"
#include "core/Timestamp.h"

/**
 * @brief Одна резервная копия: какие файлы в неё вошли, общий размер и статус.
 * Статус — маленький конечный автомат: InProgress -> Completed или InProgress -> Failed;
 * оба конечных состояния окончательные.
 */
class Backup {
public:
    enum class Status { InProgress, Completed, Failed };

    /** @brief Новая копия в статусе InProgress. */
    Backup(int id, const Timestamp& created);

    /**
     * @brief Добавить файл; повтор того же ID пропускается, чтобы размер не посчитался дважды.
     * @throws InvalidOperationException копия уже завершена или провалена.
     */
    void addEntry(int id, const DataSize& s);
    /** @brief Завершить успешно (в том числе пустую). @throws InvalidOperationException статус не InProgress. */
    void complete();
    /** @brief Отметить ошибку; пустая причина заменяется на "причина не указана". @throws InvalidOperationException статус не InProgress. */
    void fail(const std::string& reason);
    bool contains(int entryId) const;
    /** @brief Можно ли восстанавливать из копии: только Completed. */
    bool isUsable() const;
    /** @brief Общий размер файлов в копии. */
    DataSize size() const;
    int entryCount() const;
    /** @brief "выполняется", "готова" или "ошибка". */
    static const char* statusName(Status s);

    int getId() const;
    const Timestamp& getCreated() const;
    const std::vector<int>& getEntryIds() const;
    Status getStatus() const;
    const std::string& getFailReason() const;

    /** @brief Копии равны, если совпадают ID. */
    bool operator==(const Backup& o) const;
    bool operator!=(const Backup& o) const;
    /** @brief "Копия #3 от 29.10.2026 10:15: готова, 12 файлов, 4.5 MB" (у ошибки — причина). */
    friend std::ostream& operator<<(std::ostream& os, const Backup& b);

private:
    /** @brief @throws InvalidOperationException статус не InProgress; action — что пытались сделать. */
    void requireInProgress(const std::string& action) const;

    int id_;
    Timestamp created_;
    std::vector<int> entryIds_;
    DataSize totalSize_;
    Status status_;
    std::string failReason_;
};
