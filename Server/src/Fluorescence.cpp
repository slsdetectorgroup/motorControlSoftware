
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


Fluorescence::Fluorescence(int index, bool circular, Controller* controller)
  : index(index), circular(circular), controller(controller), currentTargetHolder(0) {
  if (circular) {
    FILE_LOG(logINFO) << "Fluorescence wheel [" << index << "] added";
  } else {
    FILE_LOG(logINFO) << "Fluorescence [" << index << "] added";
  }
}

bool Fluorescence::isCircular() {
  return circular;
}

 Controller* Fluorescence::getController() {
   return controller;
 }

int Fluorescence::getNumTargetLists() {
  return (int)targetHolder.size();
}

void Fluorescence::addTarget(std::string name, std::string energy) {
  // 1st time or previous target holder is full, create a new one
  if (targetHolder.size() == 0 || targetHolder[targetHolder.size() - 1]->isFull()) {
    targetHolder.push_back(new TargetHolder(targetHolder.size()));
  }
  targetHolder[targetHolder.size() - 1]->addTarget(name, energy);
}

std::string Fluorescence::getTargetName(int listIndex, int targetIndex) {
  return targetHolder[listIndex]->getTargetName(targetIndex);
}

std::string Fluorescence::getTargetEnergy(int listIndex, int targetIndex) {
  return targetHolder[listIndex]->getTargetEnergy(targetIndex);
}

void Fluorescence::print() {
  std::cout << "\tFluorescence" << (circular ? "_wheel [" : " [") << index << ']' << std::endl;
  for (unsigned int i = 0; i < targetHolder.size(); ++i) {
    std::cout << "\t Target Holder [" << i << ']' << std::endl;
      targetHolder[i]->print();
  }
  std::cout << std::endl;
}




double Fluorescence::getLimit() {
  return limit;
}

void Fluorescence::setPos(double newPosition) {
  /// xxxx
#ifdef VERY_VERBOSE
  print();
#endif
}

int Fluorescence::canMove(double newPosition) {
  if(newPosition > limit)  
	  return 1;
  else if(newPosition < 0)
	  return -1;
  //return 0 if it can move
  else
	  return 0;
}


