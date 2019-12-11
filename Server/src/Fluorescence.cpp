
#include "Fluorescence.h"
#include "TargetHolder.h"
#include "Controller.h"
#include "commonDefs.h"

#include <iostream>
#include <sstream>

#define CIRCULAR_WIDTH          (45)
#define CIRCULAR_OFFSET         (0)
#define LINEAR_WIDTH            (34)
#define LINEAR_OFFSET           (12)
#define LINEAR_LASER_POSITION   (128)

#define MAX_FLUOR_VALUES        (8)
#define FLUOR_LASERNAME			  	("laser")


Fluorescence::Fluorescence(int index, std::string name, Controller* controller)
    : index(index), name(name), circular(false), controller(controller), currentTargetHolder(0), currentTarget(-1) {
    FILE_LOG(logINFO) << name << " [" << index << "] added";
    if (name == "Fluorescence_wheel") {
        circular = true;
    }
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
    if (index >= (int)targetHolder.size()) {
        std::ostringstream oss;
        oss << "Current target holder must be less than " << targetHolder.size();
        throw RuntimeError(oss.str());
    }
    currentTargetHolder = index;
}

int Fluorescence::getCurrentTargetHolder() {
    return currentTargetHolder;
}

std::string Fluorescence::getCurrentTargetName() {
    if (currentTarget == -1) {
        return "None";
    }
    return targetHolder[currentTargetHolder]->getCurrentTargetName(currentTarget);
}

void Fluorescence::addTarget(std::string name, std::string energy) {
    // 1st time or previous target holder is full, create a new one
    if (targetHolder.size() == 0 || targetHolder[targetHolder.size() - 1]->isFull()) {
        targetHolder.push_back(new TargetHolder(targetHolder.size()));
    }
    targetHolder[targetHolder.size() - 1]->addTarget(name, energy);
}

std::string Fluorescence::getList() {
    std::string result;
    for (unsigned int i = 0; i < targetHolder.size(); ++i) {
        result += targetHolder[i]->getList();
        if (i < targetHolder.size() - 1) {
            result += ' ';
        }
    }    
    return result;
}

void Fluorescence::print() {
    std::cout << "\tFluorescence" << (circular ? "_wheel [" : " [") << index << ']' << std::endl;
    for (unsigned int i = 0; i < targetHolder.size(); ++i) {
      std::cout << "\t  Target Holder [" << i << ']' << std::endl;
        targetHolder[i]->print();
    }
    std::cout << std::endl;
}

