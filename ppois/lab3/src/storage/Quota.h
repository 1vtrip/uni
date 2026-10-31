#pragma once
#include <iostream>
#include "core/DataSize.h"

/** @brief Лимит дискового пространства пользователя. */
class Quota {
public:
    Quota(int userId = 0, const DataSize& limit = DataSize::fromGB(1), double warnPercent = 90.0);

    /** @brief Поместится ли ещё size. */
    bool canAllocate(const DataSize& size) const;
    /** @brief Занять место. @throws QuotaExceededException не помещается. */
    void consume(const DataSize& size);
    /** @brief Освободить место; занятое не уходит ниже 0. */
    void release(const DataSize& size);
    DataSize remaining() const;
    double usagePercent() const;
    /** @brief Занято больше warnPercent процентов. */
    bool isNearLimit() const;
    /** @brief Новый лимит. @throws InvalidOperationException лимит меньше уже занятого. */
    void setLimit(const DataSize& limit);

    int getUserId() const;
    const DataSize& getLimit() const;
    const DataSize& getUsed() const;

    /** @brief Квоты равны, если принадлежат одному пользователю. */
    bool operator==(const Quota& o) const;
    bool operator!=(const Quota& o) const;
    /** @brief "60.0 MB / 100.0 MB (60%)", при почти полной квоте с пометкой. */
    friend std::ostream& operator<<(std::ostream& os, const Quota& q);

private:
    int userId_;
    DataSize limit_;
    DataSize used_;
    double warnPercent_;
};
