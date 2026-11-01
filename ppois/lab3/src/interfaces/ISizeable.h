#pragma once
#include "core/DataSize.h"

/** @brief То, у чего есть размер на диске: элемент файловой системы, корзина, резервная копия. */
class ISizeable {
public:
    virtual ~ISizeable() = default;
    virtual DataSize size() const = 0;
};
