#pragma once
#include <iostream>
#include <vector>
#include "core/DataSize.h"

class FsEntry;   // нужен только как const& в add, include — в .cpp

/**
 * @brief Результат поиска: ID найденных элементов и их размеры в двух параллельных векторах
 * (ids_[i] и sizes_[i] — один элемент), плюс общий размер.
 */
class SearchResult {
public:
    SearchResult();

    /** @brief Добавить найденный элемент; размер из виртуального e.size(). Повтор того же ID пропускается. */
    void add(const FsEntry& e);
    bool contains(int id) const;
    int count() const;
    bool isEmpty() const;
    const DataSize& totalSize() const;
    /** @brief Отсортировать по убыванию размера; одинаковые остаются в порядке нахождения. */
    void sortBySizeDesc();
    /** @brief ID n самых больших элементов; сортируется копия, поэтому ответ верный и без sortBySizeDesc. */
    std::vector<int> top(int n) const;

    const std::vector<int>& getIds() const;
    const std::vector<DataSize>& getSizes() const;

    /** @brief "Найдено: 3, всего 6.0 MB" и элементы построчно. */
    friend std::ostream& operator<<(std::ostream& os, const SearchResult& r);

private:
    /**
     * @brief Сортировка вставками по убыванию размера. Двигаются оба вектора вместе, иначе размеры
     * перестали бы соответствовать своим ID. Вставки устойчивы: равные элементы не меняются местами.
     */
    static void sortDesc(std::vector<int>& ids, std::vector<DataSize>& sizes);

    std::vector<int> ids_;
    std::vector<DataSize> sizes_;
    DataSize totalSize_;
};
