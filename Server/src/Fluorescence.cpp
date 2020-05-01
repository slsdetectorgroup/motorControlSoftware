
#include "Fluorescence.h"
#include "TargetHolder.h"
#include "Controller.h"
#include "Motor.h"
#include "commonDefs.h"

#include <iostream>
#include <sstream>
#include <cstring>
#include <math.h>

#define CIRCULAR_WIDTH          (45)
#define CIRCULAR_OFFSET         (0)

#define LINEAR_WIDTH            (34)
#define LINEAR_OFFSET           (12)

#define LINEAR_LASER_POSITION   (128)
#define FLUOR_LASER_NAME		("Laser")
#define FLUOR_LASER_VALUE		(-1)

#define FLUOR_NO_TARGET_NAME    ("None")
#define FLUOR_NO_TARGET_VALUE   (-2)


Fluorescence::Fluorescence(int index, std::string name, Controller* controller, int axis)
    : index(index), name(name), circular(false), controller(controller), axis(axis), width(0), offset(0), currentTargetHolder(0), currentTarget(-1) {
    FILE_LOG(logINFO) << name << " [" << index << "] added";
    if (name == "Fluorescence_wheel") {
        circular = true;
        width = CIRCULAR_WIDTH;
        offset = CIRCULAR_OFFSET;
    } else {
        width = LINEAR_WIDTH;
        offset = LINEAR_OFFSET;
    }
    controller->getMotor(axis)->setUpperLimit(offset + MAX_FLUOR_VALUES * width);
}

std::string Fluorescence::getName() {
    return name;
}

bool Fluorescence::isCircular() {
    return circular;
}

 Controller* Fluorescence::getController() {
    return controller;
 }

int Fluorescence::getNumTargetHolders() {
    return (int)targetHolder.size();
}

void Fluorescence::setCurrentTargetHolder(int index) {
    if (index < 0 || index >= (int)targetHolder.size()) {
        std::ostringstream oss;
        oss << "Current target holder must be less than " << targetHolder.size();
        throw RuntimeError(oss.str());
    }
    currentTargetHolder = index;
}

int Fluorescence::getCurrentTargetHolder() {
    return currentTargetHolder;
}

void Fluorescence::deleteLastTargetHolder() {
    if (name == "Fluorescence_wheel") {
        throw RuntimeError("Cannot delete target holders for this motor. It cannot be changed.");
    }
    if (targetHolder.size()  <= 1) {
        throw RuntimeError("Cannot delete target holder. Minimum 1 holder is required.");
    }
    targetHolder.pop_back();
    if (currentTargetHolder >= (int)targetHolder.size()) {
        currentTargetHolder = (int)targetHolder.size() - 1; 
    }
}

void Fluorescence::addHolder(std::vector <std::string> name, std::vector <std::string> energy) {
    targetHolder.push_back(new TargetHolder(targetHolder.size()));
    for (size_t i = 0; i < name.size(); ++i) {
        targetHolder[targetHolder.size() - 1]->addTarget(name[i], energy[i]);
    }
}

void Fluorescence::addTarget(std::string name, std::string energy) {
    // 1st time or previous target holder is full, create a new one
    if (targetHolder.size() == 0 || targetHolder[targetHolder.size() - 1]->isFull()) {
        targetHolder.push_back(new TargetHolder(targetHolder.size()));
    }
    targetHolder[targetHolder.size() - 1]->addTarget(name, energy);
}

std::string Fluorescence::getList(int index) {
    std::string result;
    if (index < 0 || index > (int)targetHolder.size()) {
        std::ostringstream oss;
        oss << "Holder index must be less than " << targetHolder.size();
        throw RuntimeError(oss.str());
    }
#ifdef XRAYBOX
    result += (std::string(FLUOR_LASER_NAME) + " - ");
#endif
    result += targetHolder[index]->getList();
    return result;
}

std::string Fluorescence::updateAndGetCurrentTargetName() {
    updateCurrentTarget();

    if (currentTarget >= 0) {
        return targetHolder[currentTargetHolder]->getCurrentTargetName(currentTarget);
    }
    if (currentTarget == FLUOR_LASER_VALUE) {
        return FLUOR_LASER_NAME;
    }
    return FLUOR_NO_TARGET_NAME;
}

void Fluorescence::updateCurrentTarget() {
    double position = controller->getMotor(axis)->getPosition();

#ifdef XRAYBOX
    // laser position
    if (!circular && Motor::matches(LINEAR_LASER_POSITION, position)) {
        currentTarget = FLUOR_LASER_VALUE;
        FILE_LOG(logINFO) << name << " at " << FLUOR_LASER_NAME;
        return;
    } 
#endif

    // find target
    int ipos = round((position - (double)offset) / (double)width);
    double calculatedPosition = offset + ipos * width;
    // target positions
    if (Motor::matches(calculatedPosition, position)) {
        currentTarget = ipos;
        FILE_LOG(logINFO) << name << " at " << targetHolder[currentTargetHolder]->getCurrentTargetName(currentTarget);
    } 
    // no target position
    else {
        currentTarget = FLUOR_NO_TARGET_VALUE;
        FILE_LOG(logINFO) << name << " at " << FLUOR_NO_TARGET_NAME;
    }
}

void Fluorescence::moveToTarget(std::string target) {
    int position = -1;
    try {
        // known target
        int ipos = targetHolder[currentTargetHolder]->getTargetIndex(target);
        position = offset + ipos * width;
    } catch (...) {
#ifdef XRAYBOX
        // laser position
        if (!strcasecmp(target.c_str(), FLUOR_LASER_NAME)) {
            position = LINEAR_LASER_POSITION;
        } 
#endif        
        // unknown target
        if (position == -1) {
            throw;
        }
    }
    controller->moveAbs(position, axis);
    FILE_LOG(logINFO) << name << " has been moved to " << target;
}

void Fluorescence::print() {
    std::cout       << "\tFluorescence" << (circular ? "_wheel [" : " [") << index << ']' << std::endl;
    for (unsigned int i = 0; i < targetHolder.size(); ++i) {
        std::cout   << "\t  Target Holder [" << i << ']' << std::endl;
        targetHolder[i]->print();
    }
    std::cout << std::endl;
}

