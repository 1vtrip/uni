#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "meta/Tag.h"

/**
 * @brief Дополнительные сведения о файле: пары «ключ — значение» и метки.
 * Пары хранятся в двух параллельных векторах: keys_[i] соответствует values_[i].
 */
class Metadata {
public:
    Metadata();

    /** @brief Записать значение; если ключ уже есть, значение заменяется. @throws InvalidOperationException пустой ключ. */
    void set(const std::string& key, const std::string& value);
    /** @brief Значение по ключу; нет ключа -> пустая строка (отличить от пустого значения через has). */
    std::string get(const std::string& key) const;
    /** @brief Удалить пару; false, если ключа нет. */
    bool remove(const std::string& key);
    /** @brief Есть ли такой ключ. */
    bool has(const std::string& key) const;

    /** @brief Добавить метку; false, если метка с таким именем (без учёта регистра) уже есть. */
    bool addTag(const Tag& t);
    /** @brief Удалить метку по имени без учёта регистра; false, если её нет. */
    bool removeTag(const std::string& name);
    /** @brief Есть ли метка с таким именем без учёта регистра. */
    bool hasTag(const std::string& name) const;

    /** @brief Количество пар «ключ — значение». */
    int size() const;
    const std::vector<std::string>& keys() const;
    const std::vector<Tag>& tags() const;

    /** @brief Вывод вида "author=ivan; #work(blue)", без данных — "(пусто)". */
    friend std::ostream& operator<<(std::ostream& os, const Metadata& m);

private:
    /** @brief Индекс ключа в keys_, -1 если ключа нет. */
    int indexOf(const std::string& key) const;
    /** @brief Индекс метки в tags_, -1 если метки нет. */
    int tagIndexOf(const std::string& name) const;

    std::vector<std::string> keys_;
    std::vector<std::string> values_;
    std::vector<Tag> tags_;
};
