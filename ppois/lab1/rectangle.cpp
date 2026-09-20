//
// Created by Ivan Triputko on 20.09.2026.
//
#include "rectangle.h"
#include <iostream>

using namespace std;

void Rectangle::moveToNewCoords(const int nx, const int ny) {
    this->coords[0] = nx;
    this->coords[1] = ny;
}


void Rectangle::printInfo(const Rectangle &req) {
    cout << req.coords[0]
    << " " << req.coords[1]
    << " " << req.h
    << " " << req.w << endl;
}

void Rectangle::changeSize(const int nh, const int nw) {
    this->h = nh;
    this->w = nw;
}

 Rectangle& Rectangle::operator=(const Rectangle &other) {
    this->coords[0] = other.coords[0];
    this->coords[1] = other.coords[1];
    this->h = other.h;
    this->w = other.w;
    return *this;
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
