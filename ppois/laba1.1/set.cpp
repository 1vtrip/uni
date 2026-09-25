//
// Created by Ivan Triputko on 21.09.2026.
//

#include "set.h"
#include <string>
#include <stack>
#include <algorithm>

void Set::parseString(const std::string& str) {
    std::stack<Set> st;
    std::string atom = "";
    for (char el : str) {
        if (el == '{') {
            st.push(Set());
            atom = "";
        }
        else if (el == ',') {
            if (!atom.empty()) {
                st.top().atom.push_back(atom);
                atom = "";
            }
        }
        else if (el == '}') {
            if (!atom.empty()) {
                st.top().atom.push_back(atom);
                atom = "";
            }
            if (st.empty()) return;
            Set top = st.top();
            st.pop();
            if (st.empty()) *this = top;
            else st.top().subsets.push_back(top);
        } else if (el == ' ') continue;
        else atom += el;
    }
}

void Set::parseString(const char* str) {
    if (!str) return;
    int str_len = strlen(str);
    char* word = new char[str_len + 1];
    int start_pos = 0;
    std::stack<Set> st;
    for (int i = 0; str[i] != '\0'; ++i) {
        if (str[i] == '{') {
            st.push(Set());
            start_pos = i + 1;
        }
        else if (str[i] == ',') {
            int len = i - start_pos;
            if (len > 0 && !st.empty()) {
                strncpy(word, str + start_pos, len);
                word[len] = '\0';
                st.top().atom.push_back(word);
            }
            start_pos = i + 1;
        }
        else if (str[i] == '}') {
            int len = i - start_pos;
            if (len > 0 && !st.empty()) {
                strncpy(word, str + start_pos, len);
                word[len] = '\0';
                st.top().atom.push_back(word);
            }
            if (st.empty()) {
                delete[] word;
                return;
            }
            Set top = st.top();
            st.pop();
            if (st.empty()) {
                *this = top;
            } else {
                st.top().subsets.push_back(top);
            }
            start_pos = i + 1;
        }
    }
    delete[] word;
}

bool Set::isEmpty() const {
    return atom.empty() && subsets.empty();
}

size_t Set::getSize() const {
    return atom.size() + subsets.size();
}

Set& Set::addElement(char natom) {
    auto it = std::find(atom.begin(), atom.end(), natom);
    if (it == atom.end()) atom.push_back(natom);
    return *this;
}

Set& Set::addElement(const Set& nsubset) {
    auto it = std::find(subsets.begin(), subsets.end(), nsubset);
    if (it == subsets.end()) subsets.push_back(nsubset);
    return *this;
}

Set& Set::deleteElement(char ch) {
    auto it = std::find(atom.begin(), atom.end(), ch);
    if (it != atom.end()) {
        atom.erase(it);
    }
    return *this;
}

Set& Set::deleteElement(const Set& subset) {
    auto it = std::find(subsets.begin(), subsets.end(), subset);
    if (it != subsets.end()) {
        subsets.erase(it);
    }
    return *this;
}

Set Set::operator+(const Set& other) {
    Set temp = *this;
    for (char el : other.atom) {
        if (std::find(temp.atom.begin(), temp.atom.end(), el) == temp.atom.end()) {
            temp.atom.push_back(el);
        }
    }
    for (const Set& subset : other.subsets) {
        if (std::find(temp.subsets.begin(), temp.subsets.end(), subset) == temp.subsets.end()) {
            temp.subsets.push_back(subset);
        }
    }
    return temp;
}

Set Set::operator-(const Set& other) {
    Set temp = *this;
    for (char el : other.atom) {
        if (std::find(temp.atom.begin(), temp.atom.end(), el) != temp.atom.end()) {
            auto it = std::find(temp.atom.begin(), temp.atom.end(), el);
            temp.atom.erase(it);
        }
    }
    for (const Set& subset : other.subsets) {
        if (std::find(temp.subsets.begin(), temp.subsets.end(), subset) != temp.subsets.end()) {
            auto it = find(temp.subsets.begin(), temp.subsets.end(), subset);
            temp.subsets.erase(it);
        }
    }
    return temp;
}

Set& Set::operator+=(const Set& other) {
    for (char el : other.atom) {
        auto it = std::find(atom.begin(), atom.end(), el);
        if (it == atom.end()) {
            this->atom.push_back(el);
        }
    }
    for (const Set& subset : other.subsets) {
        auto it = std::find(this->subsets.begin(), this->subsets.end(), subset);
        if (it == this->subsets.end()) {
            this->subsets.push_back(subset);
        }
    }
    return *this;
}

Set& Set::operator-=(const Set& other) {
    for (char el : other.atom) {
        auto it = std::find(atom.begin(), atom.end(), el);
        if (it != atom.end()) {
            this->atom.erase(it);
        }
    }
    for (const Set& subset : other.subsets) {
        auto it = std::find(this->subsets.begin(), this->subsets.end(), subset);
        if (it != this->subsets.end()) {
            this->subsets.erase(it);
        }
    }
    return *this;
}

Set Set::operator*(const Set& other) {
    Set temp;
    for (char el : atom) {
        if (std::find(other.atom.begin(), other.atom.end(), el) != other.atom.end()) {
            temp.atom.push_back(el);
        }
    }
    for (const Set& subset : subsets) {
        if (std::find(other.subsets.begin(), other.subsets.end(), subset) != other.subsets.end()) {
            temp.subsets.push_back(subset);
        }
    }
    return temp;
}

Set& Set::operator*=(const Set& other) {
    *this = *this * other;
    return *this;
}

bool Set::operator==(const Set& other) const {
    return atom == other.atom && subsets == other.subsets;
}

bool Set::operator[](char natom) const {
    auto it = std::find(atom.begin(), atom.end(), natom);
    return it != atom.end();
}

bool Set::operator[](const Set& subset) const {
    auto it = std::find(subsets.begin(), subsets.end(), subset);
    return it != subsets.end();
}

Set Set::powerSet() const {
    Set temp;
    temp.addElement(Set());
    for (char el : atom) {
        Set singleAtom;
        singleAtom.addElement(el);
        temp.addElement(singleAtom);
    }
    for (const Set& subset : subsets) {
        temp.addElement(subset);
    }
    if (atom.size() >= 2) {
        Set pairAtom;
        for (char el : atom) {
            pairAtom.addElement(el);
        }
        temp.addElement(pairAtom);
    }
    for (char el : atom) {
        for (const Set& subset : subsets) {
            Set mixed;
            mixed.addElement(el);
            mixed.addElement(subset);
            temp.addElement(mixed);
        }
    }
    return temp;
}