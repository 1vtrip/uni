#pragma once
#include <iostream>
#include <string>
#include "core/DataSize.h"
#include "core/Timestamp.h"

class FsEntry;   // нужен только как const& в matches, include — в .cpp

/**
 * @brief Условия поиска: шаблон имени (* и ?), расширение, размер, дата изменения, владелец, скрытые.
 * Пустой фильтр не ограничивает. Сеттеры возвращают *this, поэтому их можно писать цепочкой:
 * q.byName("*.txt").bySize(min, max).byOwner(7).
 */
class SearchQuery {
public:
    /** @brief Без фильтров: подходит всё, кроме скрытых. */
    SearchQuery();

    /** @brief Подходит ли text под шаблон: * — любая последовательность, ? — один символ. С учётом регистра. */
    static bool wildcardMatch(const std::string& pattern, const std::string& text);
    /**
     * @brief Выполнены ли все заданные фильтры. Имя и расширение сравниваются без учёта регистра.
     * Размер берётся из виртуального e.size(): у папки это 4 KB без содержимого.
     */
    bool matches(const FsEntry& e) const;

    /** @brief Шаблон имени, например "*.txt" или "report_??.md"; пустой — любое имя. */
    SearchQuery& byName(const std::string& pattern);
    /** @brief Расширение без учёта регистра и точки: "TXT", ".txt" и "txt" одинаковы; пустое — любое. */
    SearchQuery& byExtension(const std::string& ext);
    /** @brief Размер от min до max включительно. @throws InvalidOperationException min больше max. */
    SearchQuery& bySize(const DataSize& min, const DataSize& max);
    /** @brief Изменённые не раньше since. */
    SearchQuery& modifiedSince(const Timestamp& since);
    /** @brief Только элементы владельца; 0 — любой владелец. */
    SearchQuery& byOwner(int ownerId);
    /** @brief Показывать ли скрытые (имя с точки). */
    SearchQuery& withHidden(bool include = true);

    const std::string& getNamePattern() const;
    const std::string& getExtension() const;
    const DataSize& getMinSize() const;
    const DataSize& getMaxSize() const;
    int getOwnerId() const;
    bool includesHidden() const;

    /** @brief Описание фильтров: "имя *.txt, размер 0 B..1.0 MB, владелец #7"; без фильтров — "всё". */
    friend std::ostream& operator<<(std::ostream& os, const SearchQuery& q);

private:
    /** @brief Копия строки в нижнем регистре (латиница). */
    static std::string lower(const std::string& s);

    std::string namePattern_;
    std::string extension_;
    DataSize minSize_;
    DataSize maxSize_;
    Timestamp modifiedAfter_;
    bool useDateFilter_;
    int ownerId_;
    bool includeHidden_;
};
