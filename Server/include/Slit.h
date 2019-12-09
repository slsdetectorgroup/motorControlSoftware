#pragma once

#include "Controller.h"

class Slit {
 public:

  Slit(Controller* controllerX1, Controller* controllerX2, int axisX1, int axisX2);
  void updateLimits();

  double getSlitWidth();
  double getX1Center();

  void print();
  

 private:
  Controller* controllerX1;
  Controller* controllerX2;
  int axisX1;
  int axisX2;
  double limit; 
}; 
