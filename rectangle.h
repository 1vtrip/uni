//
// Created by Ivan Triputko on 20.09.2026.
//

#pragma once

class Rectangle {
public:
    Rectangle();
    Rectangle(int x, int y, int h, int w) {
        this->coords[0] = x;
        this->coords[1] = y;
        this->h = h;
        this->w = w;
    }
    ~Rectangle();
    void setCoords(int x, int y);
    static void printInfo(const Rectangle& req);
    Rectangle& changeSize(int nx, int ny);
private:
    int coords[2]{0, 0};
    int h=0;
    int w=0;
};