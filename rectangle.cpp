//
// Created by Ivan Triputko on 20.09.2026.
//
#include "rectangle.h"
#include <iostream>

using namespace std;

void Rectangle::setCoords(int x, int y) {
    this->coords[0] = x;
    this->coords[1] = y;
}

void Rectangle::printInfo(const Rectangle &req) {
    cout << req.coords[0]
    << " " << req.coords[1]
    << " " << req.h
    << " " << req.w << endl;
}