//
// Created by Ivan Triputko on 21.09.2026.
//

#include "set.h"
#include <string>
#include <stack>
#include <algorithm>

void Set::parseString(const std::string& str) {
    std::stack<Set> st;
    for (char el : str) {
        if (el == '{') {
            st.push(Set());
        } else if (el == ',' || el == ' ') {
            continue;
        } else if (el == '}') {
            if (st.empty()) return;
            Set top = st.top();
            st.pop();
            if (st.empty()) {
                *this = top;
            } else {
                st.top().subsets.push_back(top);
            }
        } else {
            if (st.empty()) return;
            st.top().atom.push_back(el);
        }
    }
}

bool Set::isEmpty() const {
    return atom.empty() && subsets.empty();
}

size_t Set::getSize() const {
    return atom.size() + subsets.size();
}

Set& Set::addElement(char natom) {
    atom.push_back(natom);
    return *this;
}

Set& Set::addElement(const Set& nsubset) {
    subsets.push_back(nsubset);
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

Set Set::operator+(const Set& other) const {
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

bool Set::operator==(const Set& other) const {
    return atom == other.atom && subsets == other.subsets;
}