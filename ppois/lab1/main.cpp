//
// Created by Ivan Triputko on 20.09.2026.
//

#include "rectangle.h"

int main() {
    Rectangle nrec(0, 0, 1 ,0);
    Rectangle::printInfo(nrec);
    nrec.moveToNewCoords(1, 4);
    nrec.changeSize(2, 4);
    Rectangle::printInfo(nrec);
    return 0;
}