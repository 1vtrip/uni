#pragma once
#include "core/Date.h"

/** @brief То, что требует периодической проверки: аттестация мастера, калибровка инструмента. */
class ICertifiable {
public:
    virtual ~ICertifiable() = default;
    /** @brief Нужна ли проверка на дату today. */
    virtual bool needsCheck(const Date& today) const = 0;
    /** @brief Провести проверку в день today. */
    virtual void certify(const Date& today) = 0;
};
