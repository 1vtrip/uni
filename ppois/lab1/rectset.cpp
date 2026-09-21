//
// Created by Ivan Triputko on 20.09.2026.
//
#include "rectangle.h"
#include <iostream>
#include <algorithm>

using namespace std;

void Rectangle::getCoords(const Rectangle &rect) {
    cout << "(" << rect.coords[0] << "; " << rect.coords[1] << "); "
    << "(" << rect.coords[0] + rect.w << "; " << rect.coords[1] << "); ("
    << rect.coords[0] << "; " << rect.coords[1] + rect.h << "); "
    "(" << rect.coords[0] + rect.w << "; " << rect.coords[1] + rect.h << ").\n";
}

void Rectangle::moveToNewCoords(const int nx, const int ny) {
    this->coords[0] = nx;
    this->coords[1] = ny;
}

void Rectangle::changeSize(const int nw, const int nh) {
    this->h = nh;
    this->w = nw;
}

Rectangle& Rectangle::operator++() {
    ++this->h;
    ++this->w;
    return *this;
}

Rectangle Rectangle::operator++(int value) {
    Rectangle copy = *this;
    this->h++;
    this->w++;
    return copy;
}

Rectangle &Rectangle::operator--() {
    this->h--;
    this->w--;
    return *this;
}

Rectangle Rectangle::operator--(int value) {
    Rectangle temp = *this;
    this->h--;
    this->w--;
    return temp;
}

Rectangle Rectangle::operator+(const Rectangle& other) {
    int x1 = std::min(this->coords[0], other.coords[0]);
    int y1 = std::min(this->coords[1], other.coords[1]);
    int x2 = std::max(this->coords[0] + this->w,
                      other.coords[0] + other.w);
    int y2 = std::max(this->coords[1] + this->h,
                      other.coords[1] + other.h);

    return Rectangle(x1, y1, x2 - x1, y2 - y1);
}

Rectangle Rectangle::operator-(const Rectangle& other) {
    int x1 = std::max(this->coords[0], other.coords[0]);
    int y1 = std::max(this->coords[1], other.coords[1]);
    int x2 = std::min(this->coords[0] + this->w,
                      other.coords[0] + other.w);
    int y2 = std::min(this->coords[1] + this->h,
                      other.coords[1] + other.h);

    if (x2 <= x1 || y2 <= y1) {
        return Rectangle(0, 0, 0, 0);
    }
    return Rectangle(x1, y1, x2 - x1, y2 - y1);
}

Rectangle &Rectangle::operator+=(const Rectangle &other) {
    *this = *this + other;
    return *this;
}

Rectangle &Rectangle::operator-=(const Rectangle &other) {
    *this = *this - other;
    return *this;
}

void Rectangle::printInfo(const Rectangle &rect) {
    cout << rect.coords[0]
    << " " << rect.coords[1]
    << " " << rect.h
    << " " << rect.w << endl;
}

 Rectangle& Rectangle::operator=(const Rectangle &other) {
    if (this == &other) return *this;
    this->coords[0] = other.coords[0];
    this->coords[1] = other.coords[1];
    this->h = other.h;
    this->w = other.w;
    return *this;
}

bool Rectangle::operator!=(const Rectangle& other) {
    return !(*this == other);
}

bool Rectangle::operator==(const Rectangle& other) {
    return coords[0] == other.coords[0]
        && coords[1] == other.coords[1]
        && h == other.h
        && w == other.w;
}

bool Rectangle::operator!=(const Rectangle& other) {
    return !(*this == other);
}