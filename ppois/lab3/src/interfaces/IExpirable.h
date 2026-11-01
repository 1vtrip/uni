#pragma once
#include "core/Timestamp.h"

/**
 * @brief То, что имеет срок действия: сессия, ссылка, запись в корзине, блокировка, гостевой доступ.
 * Пять не связанных между собой классов с одним общим поведением «истёк ли срок».
 */
class IExpirable {
public:
    virtual ~IExpirable() = default;
    virtual bool isExpired(const Timestamp& now) const = 0;
};
