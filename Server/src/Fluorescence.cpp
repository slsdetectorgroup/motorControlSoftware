
#include "Fluorescence.h"
#include "Controller.h"
#include "Motor.h"
#include "TargetHolder.h"
#include "commonDefs.h"

#include <cstring>
#include <iostream>
#include <math.h>
#include <sstream>

#define CIRCULAR_WIDTH  (45)
#define CIRCULAR_OFFSET (0)

#define LINEAR_WIDTH  (34)
#define LINEAR_OFFSET (6)

#define LINEAR_LASER_POSITION (128)
#define FLUOR_LASER_NAME      ("Laser")
#define FLUOR_LASER_VALUE     (-1)

#define FLUOR_NO_TARGET_NAME  ("None")
#define FLUOR_NO_TARGET_VALUE (-2)

Fluorescence::Fluorescence(int index, std::string name, Controller *controller,
                           int axis)
    : index(index), name(name), circular(false), controller(controller),
      axis(axis), width(0), offset(0), currentTargetHolder(0),
      currentTarget(-1) {
    LOG(logINFO) << name << " [" << index << "] added";
    if (name == "Fluorescence_wheel") {
        circular = true;
        width = CIRCULAR_WIDTH;
        offset = CIRCULAR_OFFSET;
    } else {
        width = LINEAR_WIDTH;
        offset = LINEAR_OFFSET;
    }
    controller->getMotor(axis)->setUpperLimit(offset +
                                              MAX_FLUOR_VALUES * width);
}

std::string Fluorescence::getName() { return name; }

bool Fluorescence::isCircular() { return circular; }

Controller *Fluorescence::getController() { return controller; }

int Fluorescence::getNumTargetHolders() {
    return static_cast<int>(targetHolder.size());
}

void Fluorescence::setCurrentTargetHolder(int index) {
    if (index < 0 || index >= getNumTargetHolders()) {
        std::ostringstream oss;
        oss << "Current target holder " << index << " must be less than "
            << targetHolder.size();
        throw RuntimeError(oss.str());
    }
    currentTargetHolder = index;
}

int Fluorescence::getCurrentTargetHolder() { return currentTargetHolder; }

void Fluorescence::deleteLastTargetHolder() {
    if (name == "Fluorescence_wheel") {
        throw RuntimeError("Cannot delete target holders for this motor. It "
                           "cannot be changed.");
    }
    if (targetHolder.size() <= 1) {
        throw RuntimeError(
            "Cannot delete target holder. Minimum 1 holder is required.");
    }
    targetHolder.pop_back();
    int targetHolderSize = getNumTargetHolders();
    if (currentTargetHolder >= targetHolderSize) {
        currentTargetHolder = targetHolderSize - 1;
    }
}

void Fluorescence::addHolder(std::vector<std::string> name,
                             std::vector<std::string> energy) {
    int targetHolderSize = getNumTargetHolders();
    targetHolder.push_back(new TargetHolder(targetHolderSize));
    for (size_t i = 0; i < name.size(); ++i) {
        targetHolderSize = getNumTargetHolders();
        targetHolder[targetHolderSize - 1]->addTarget(name[i], energy[i]);
    }
}

void Fluorescence::addTarget(std::string name, std::string energy) {
    // 1st time or previous target holder is full, create a new one
    int targetHolderSize = getNumTargetHolders();
    if (targetHolderSize == 0 || targetHolder[targetHolderSize - 1]->isFull()) {
        targetHolder.push_back(new TargetHolder(targetHolderSize));
    }
    targetHolderSize = getNumTargetHolders();
    targetHolder[targetHolderSize - 1]->addTarget(name, energy);
}

void Fluorescence::changeTarget(int holderIndex, int targetIndex,
                                std::string name, std::string energy) {
    if (holderIndex < 0 || holderIndex >= getNumTargetHolders()) {
        std::ostringstream oss;
        oss << "Holder index must be less than " << targetHolder.size();
        throw RuntimeError(oss.str());
    }
    targetHolder[holderIndex]->changeTarget(targetIndex, name, energy);
}

std::string Fluorescence::getList(int index) {
    std::string result;
    if (index < 0 || index >= getNumTargetHolders()) {
        std::ostringstream oss;
        oss << "Holder index " << index << " must be less than "
            << targetHolder.size();
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
        return targetHolder[currentTargetHolder]->getCurrentTargetName(
            currentTarget);
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
        LOG(logINFO) << name << " at " << FLUOR_LASER_NAME;
        return;
    }
#endif

    // find target
    double dOffset = static_cast<double>(offset);
    double dWidth = static_cast<double>(width);
    int ipos = static_cast<int>(round((position - dOffset) / dWidth));
    double calculatedPosition = offset + ipos * width;
    // target positions
    if (Motor::matches(calculatedPosition, position)) {
        currentTarget = ipos;
        LOG(logINFO) << name << " at "
                     << targetHolder[currentTargetHolder]->getCurrentTargetName(
                            currentTarget);
    }
    // no target position
    else {
        currentTarget = FLUOR_NO_TARGET_VALUE;
        LOG(logINFO) << name << " at " << FLUOR_NO_TARGET_NAME;
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
    LOG(logINFO) << name << " has been moved to " << target;
}

void Fluorescence::print() {
    std::cout << "\tFluorescence" << (circular ? "_wheel [" : " [") << index
              << ']' << std::endl;
    for (int i = 0; i < getNumTargetHolders(); ++i) {
        std::cout << "\t  Target Holder [" << i << ']' << std::endl;
        targetHolder[i]->print();
    }
    std::cout << std::endl;
}
