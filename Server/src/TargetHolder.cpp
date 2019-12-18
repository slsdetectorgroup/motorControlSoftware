
#include "TargetHolder.h"
#include "commonDefs.h"

#include <iostream>
#include <sstream>
#include <cstring>

#define MAX_FLUOR_VALUES        (8)


TargetHolder::TargetHolder(int index)
  : index(index), numTargets(0) {
    targetName.resize(MAX_FLUOR_VALUES);
    targetEnergy.resize(MAX_FLUOR_VALUES);
    for (int i = 0; i < MAX_FLUOR_VALUES; ++i) {
      targetName[i] = '-';
      targetEnergy[i] = '-';
    }
    FILE_LOG(logDEBUG) << "Target Holder [" << index << "] added";
}

int TargetHolder::getNumTargets() {
    return numTargets;
}

bool TargetHolder::isFull() {
    return (numTargets == MAX_FLUOR_VALUES);
}

std::string TargetHolder::getCurrentTargetName(int index) {
    return targetName[index];
}

void TargetHolder::addTarget(std::string name, std::string energy) {
    if (numTargets == MAX_FLUOR_VALUES) {
        std::ostringstream oss;
        oss << "Exceeded max fluorescence targets " << MAX_FLUOR_VALUES << ". Cannot add " << name;
        throw RuntimeError(oss.str());
    }
    targetName[numTargets] = name;
    targetEnergy[numTargets] = energy;
    FILE_LOG(logDEBUG) << "Target [" << index << "]: " << name;
    ++numTargets;
}

int TargetHolder::getTargetIndex(std::string name) {
    for (int i = 0; i < numTargets; ++i) {
        if (!strcasecmp(name.c_str(), targetName[i].c_str())) {
            return i;
        }
    }
    throw std::runtime_error("Unknown target " + name);
}


std::string TargetHolder::getList() {
    std::string result;
    for (int i = 0; i < numTargets; ++i) {
        result += (targetName[i] + ' ' + targetEnergy[i]);
        if (i < numTargets - 1) {
            result += ' ';
        }
    }    
    return result;
}

void TargetHolder::print() {
    for (int i = 0; i < numTargets; ++i) {
        std::cout << "\t    Target [" << i << "] : " << targetName[i] << ", " <<  targetEnergy[i] << std::endl;
    }
}
