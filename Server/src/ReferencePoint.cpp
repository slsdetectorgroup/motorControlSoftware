#include "ReferencePoint.h"
#include "commonDefs.h"

#include <iostream>

ReferencePoint::ReferencePoint(int index, std::string name, std::vector<double> position)
    : index(index), name(name), position(position) {
 	FILE_LOG(logINFO) << "Reference Point [" << index << "]: [name:" << name << ", position:(" << position[0] << ", " << position[1] << ", " << position[2] << "]";       
}

std::string ReferencePoint::getName() {
    return name;
}

double ReferencePoint::getXPosition() {
    return position[0];    
}

double ReferencePoint::getYPosition() {
    return position[1];
}

double ReferencePoint::getZPosition() {
    return position[2];
}
void ReferencePoint::print() {
    std::cout << "\t" << name << ": [" << position[0] << ", " <<  position[1] << ", " <<  position[2] << ']' << std::endl;
}
