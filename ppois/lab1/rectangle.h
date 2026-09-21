//
// Created by Ivan Triputko on 20.09.2026.
//

#pragma once

class Rectangle {
public:
    Rectangle(){};
    Rectangle(const int x, const int y, const int h, const int w) {
        this->coords[0] = x;
        this->coords[1] = y;
        this->h = h;
        this->w = w;
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
    void changeSize(const int nh, const int nw);
    Rectangle& operator++();
    Rectangle operator++(int value);
    Rectangle& operator--();
    Rectangle operator--(int value);
    static void printInfo(const Rectangle& rect);
    Rectangle& operator=(const Rectangle& other);
private:
    int coords[2]{0, 0};
    int h=0;
    int w=0;
};