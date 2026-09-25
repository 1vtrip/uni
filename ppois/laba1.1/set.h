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
    void parseString(const char* str);
    bool isEmpty() const;
    size_t getSize() const;
    Set& addElement(const std::string& natom);
    Set& addElement(const Set& nsubset);
    Set& deleteElement(const std::string& natom);
    Set& deleteElement(const Set& subset);
    Set operator+(const Set& other);
    Set operator-(const Set& other);
    Set& operator+=(const Set& other);
    Set& operator-=(const Set& other);
    Set operator*(const Set& other);
    Set& operator*=(const Set& other);
    bool operator==(const Set& other) const;
    bool operator[](const std::string& atom) const;
    bool operator[](const Set& subset) const;
    Set powerSet() const;
private:
    std::vector<std::string> atom;
    std::vector<Set> subsets;
};