#pragma once


#include "Interface.h"

class Motor {
 public:

  Motor(int index, std::string name, int axis, int controller, double lowerLimit, double upperLimit);
  std::string getName();
  int getAxis();
  int getController();
  void setLowerLimit(double lowerLimit);	
  double getLowerLimit();
  void setUpperLimit(double upperLimit);	
  double getUpperLimit();
  void setPosition(double position);  
  double getPosition();
  bool canMotorMove(double position);
  void print();

 private:
 int index;
  std::string name;
  int axis;
  int controller;
  double lowerLimit;
  double upperLimit;
  double position;

}; 

