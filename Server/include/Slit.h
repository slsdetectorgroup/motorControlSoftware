#pragma once

#include "Controller.h"

class Slit {
 public:

  Slit(Controller* controllerX1, Controller* controllerX2, int axisX1, int axisX2);
  void updateLimits();
  std::string canBothMove(double position1, double position2);
  double getSlitWidth();
  void setSlitWidth(double width);
  double getCenter();
  void setCenter(double center);
  void print();
  

 private:
  Controller* controllerX1;
  Controller* controllerX2;
  int axisX1;
  int axisX2;
  double limit; 
}; 
