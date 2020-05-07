#include "ReferencePoint.h"
#include "Motor.h"
#include "commonDefs.h"

#include <iostream>

ReferencePoint::ReferencePoint(int index, std::string name,
                               std::vector<double> position)
    : index(index), name(name), position(position) {}

std::string ReferencePoint::getName() { return name; }

std::vector<double> ReferencePoint::getPositions() { return position; }

bool ReferencePoint::isMatch(std::vector<double> pos) {
    for (unsigned int i = 0; i < pos.size(); ++i) {
        // if reference position is -1, valid as it is a dont care
        if (Motor::matches(position[i], -1)) {
            continue;
        }
        if (!Motor::matches(pos[i], position[i])) {
            return false;
        }
    }
    return true;
}

void ReferencePoint::print() {
    std::cout << "\t  [" << index << "] : " << name << " [";
    for (unsigned int i = 0; i < position.size(); ++i) {
        std::cout << position[i];
        if (i < position.size() - 1) {
            std::cout << ", ";
        }
    }
    std::cout << ']' << std::endl;
}
