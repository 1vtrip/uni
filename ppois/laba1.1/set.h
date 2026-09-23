//
// Created by Ivan Triputko on 21.09.2026.
//

#pragma once
#include <vector>
#include <string>

class Set {
public:
    Set() = default;
    void parseString(const std::string& str);
    bool isEmpty() const;
    size_t getSize() const;
    Set& addElement(char natom);
    Set& addElement(const Set& nsubset);
    Set& deleteElement(char ch);
    Set& deleteElement(const Set& subset);
    Set operator+(const Set& other) const;
    bool operator==(const Set& other) const;

private:
    std::vector<char> atom;
    std::vector<Set> subsets;
};