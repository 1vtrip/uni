#pragma once
#include <iostream>

/** @brief Объект, который умеет вывести себя в поток. */
class IPrintable {
public:
    virtual ~IPrintable() = default;
    virtual void print(std::ostream& os) const = 0;
};

/** @brief Один оператор вывода для всех классов с IPrintable: вызывает виртуальный print(). */
inline std::ostream& operator<<(std::ostream& os, const IPrintable& obj) {
    obj.print(os);
    return os;
}
