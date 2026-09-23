//
// Created by Ivan Triputko on 20.09.2026.
//

#pragma once
#include <iostream>

class Rectangle {
public:
    Rectangle(){};
    Rectangle(const int x, const int y, const int w,  const int h) {
        this->coords[0] = x;
        this->coords[1] = y;
        this->w = w;
        this->h = h;
    }
    ~Rectangle(){};
    int getX() {
        return this->coords[0];
    }
    int getY() {
        return this->coords[1];
    }
    int getW() {
        return this->w;
    }
    int getH() {
        return this->h;
    }
    static void getCoords(const Rectangle& rect);
    void moveToNewCoords(const int nx, const int ny);
    void changeSize(const int nw, const int nh);
    Rectangle& operator++();
    Rectangle operator++(int value);
    Rectangle& operator--();
    Rectangle operator--(int value);
    Rectangle operator+(const Rectangle& other);
    Rectangle operator-(const Rectangle& other);
    Rectangle& operator+=(const Rectangle& other);
    Rectangle& operator-=(const Rectangle& other);
    static void printInfo(const Rectangle& rect);
    Rectangle& operator=(const Rectangle& other);
    bool operator==(const Rectangle& other);
    bool operator!=(const Rectangle& other);
    friend std::ostream& operator<<(std::ostream& os, const Rectangle& rect);
    friend std::istream& operator>>(std::istream& is, Rectangle& rect);
private:
    int coords[2]{0, 0};
    int w=0;
    int h=0;
};