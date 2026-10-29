#pragma once
#include <iostream>
#include <string>

/** @brief Объём данных. Хранится в байтах. */
class DataSize {
public:
    explicit DataSize(long long bytes = 0);
    static DataSize fromKB(double kb);
    static DataSize fromMB(double mb);
    static DataSize fromGB(double gb);

    long long getBytes() const;
    bool isZero() const;
    /** @brief Какую долю (0..100%) составляет от total. */
    double percentOf(const DataSize& total) const;
    /** @brief Человекочитаемо: "512 B", "1.5 KB", "3.2 GB". */
    std::string toString() const;

    DataSize operator+(const DataSize& o) const;
    DataSize operator-(const DataSize& o) const;
    DataSize operator*(double k) const;
    DataSize& operator+=(const DataSize& o);
    DataSize& operator-=(const DataSize& o);
    bool operator<(const DataSize& o) const;
    bool operator<=(const DataSize& o) const;
    bool operator==(const DataSize& o) const;
    bool operator!=(const DataSize& o) const;
    friend std::ostream& operator<<(std::ostream& os, const DataSize& s);

private:
    long long bytes_;
};
