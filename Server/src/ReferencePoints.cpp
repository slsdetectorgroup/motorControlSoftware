#include "ReferencePoints.h"
#include "Controller.h"
#include "Motor.h"
#include "ReferencePoint.h"
#include "commonDefs.h"

#include <iostream>
#include <math.h>

ReferencePoints::ReferencePoints(std::vector<Controller *> controller,
                                 std::vector<int> axis)
    : controller(controller), axis(axis) {}

int ReferencePoints::size() { return static_cast<int>(referencePoint.size()); }

void ReferencePoints::add(std::string name, std::vector<double> position) {
    referencePoint.push_back(
        new ReferencePoint(size(), name, position));
}

std::vector<double> ReferencePoints::getCurrentMotorPositions() {
    std::vector<double> positions(controller.size());
    for (size_t i = 0; i < controller.size(); ++i) {
        positions[i] = controller[i]->getMotor(axis[i])->getPosition();
    }
    return positions;
}

std::vector<double> ReferencePoints::getPositions(std::string name) {
    for (int i = 0; i < size(); ++i) {
        if (referencePoint[i]->getName() == name) {
            return referencePoint[i]->getPositions();
        }
    }
    throw RuntimeError("Invalid reference name " + name);
}

std::string ReferencePoints::getList() {
    std::string result;
    for (int i = 0; i < size(); ++i) {
        result += referencePoint[i]->getName();
        if (i < size() - 1) {
            result += ' ';
        }
    }
    return result;
}

std::string ReferencePoints::getCurrentReferenceName() {
    // get current positions
    std::vector<double> positions = getCurrentMotorPositions();

    // find matching reference name
    for (int i = 0; i < size(); ++i) {
        if (referencePoint[i]->isMatch(positions)) {
            return referencePoint[i]->getName();
        }
    }
    return "None";
}

void ReferencePoints::moveTo(std::string name) {
    for (int i = 0; i < size(); ++i) {
        if (referencePoint[i]->getName() == name) {
            std::vector<double> positions = referencePoint[i]->getPositions();
            for (unsigned int j = 0; j < controller.size(); ++j) {
                // move motor to that position ( as long as reference position
                // is not -1)
                if (!Motor::matches(positions[j], -1)) {
                    LOG(logINFO) << "Moving "
                                 << controller[j]->getMotor(axis[j])->getName()
                                 << " to " << positions[j];
                    controller[j]->moveAbs(positions[j], axis[j]);
                }
            }
            return;
        }
    }
    throw RuntimeError("Invalid reference name " + name);
}

void ReferencePoints::print() {
    std::cout << "\tControllers (axis) [";
    for (unsigned int i = 0; i < controller.size(); ++i) {
        std::cout << controller[i]->getName() << " (" << axis[i] << ")";
        if (i < controller.size() - 1) {
            std::cout << ", ";
        }
    }
    std::cout << ']' << std::endl;

    for (int i = 0; i < size(); ++i) {
        referencePoint[i]->print();
    }
    std::cout << std::endl;
}
