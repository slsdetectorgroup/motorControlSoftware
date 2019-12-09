#include "Slit.h"
#include "Motor.h"
#include "commonDefs.h"

#include <iostream>

#define SLITS_LIMIT (105000)


Slit::Slit(Controller* controllerX1, Controller* controllerX2, int axisX1, int axisX2) 
  : controllerX1(controllerX1), controllerX2(controllerX2), axisX1(axisX1), axisX2(axisX2),limit(SLITS_LIMIT) {
  controllerX1->getMotor(axisX1)->setLowerLimit(0);
  controllerX2->getMotor(axisX2)->setLowerLimit(0);
 	FILE_LOG(logINFO) << "Slit: [controllerX1:" << controllerX1->getName() << ", controllerX2:(" << controllerX2->getName() << ", axisX1:" << axisX1 << ", axisX2:" << axisX2 << "]";       
}

void Slit::updateLimits() {
  int x1pos = controllerX1->getMotor(axisX1)->getPosition();  
  int x2pos = controllerX2->getMotor(axisX2)->getPosition();
  controllerX1->getMotor(axisX1)->setUpperLimit(limit - x2pos);
  controllerX2->getMotor(axisX2)->setUpperLimit(limit - x1pos);
}

double Slit::getSlitWidth() {
  int x1pos = controllerX1->getMotor(axisX1)->getPosition();  
  int x2pos = controllerX2->getMotor(axisX2)->getPosition();
  return limit - x2pos - x1pos;
}

double Slit::getX1Center() {
  int x1pos = controllerX1->getMotor(axisX1)->getPosition();  
  int x2pos = controllerX2->getMotor(axisX2)->getPosition();
  return (limit - x2pos + x1pos) / 2;
}


void Slit::print() {
  std::cout << "\tLimit: " << limit << ", Controllers: [" << controllerX1->getName() << ", " << controllerX2->getName() << ']' << std::endl;
}

