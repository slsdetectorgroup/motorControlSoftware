#include "Motor.h"
#include "commonDefs.h"

#include <iostream>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <stdio.h>


Motor::Motor(int index, std::string name, int axis, int controller, double lowerLimit, double upperLimit)
  : index(index),name(name),axis(axis),controller(controller), lowerLimit(lowerLimit),upperLimit(upperLimit),position(0.0) {
	FILE_LOG(logINFO) << "Motor [" << index << "]: [name:" << name << ", axis:" << axis <<  ", iController:" << controller << "]";
  }

std::string Motor::getName() {
	return name;
}

int Motor::getAxis() {
	return axis;
}

int Motor::getController() {
	return controller;
}

void Motor::setLowerLimit(double lowerLimit) {
	if (lowerLimit > upperLimit) {
		std::ostringstream oss;
		oss << "Could not set lower limit " << lowerLimit << ". Must be less than upper limit " << upperLimit;
		throw RuntimeError(oss.str());
	}
	this->lowerLimit = lowerLimit;
}

double Motor::getLowerLimit() {
	return lowerLimit;
}

void Motor::setUpperLimit(double upperLimit) {
	if (upperLimit < lowerLimit) {
		std::ostringstream oss;
		oss << "Could not set upper limit " << upperLimit << ". Must be greater than lower limit " << lowerLimit;
		throw RuntimeError(oss.str());
	}
	this->upperLimit = upperLimit;
}

double Motor::getUpperLimit() {
	return upperLimit;
}

void Motor::setPosition(double position) {
	this->position = position;
}

double Motor::getPosition() {
	return position;
}

void Motor::print() {
  std::cout << "\tMotor [" << index << "]\n\t  "
  			<< "Name        : " << name << "\n\t  "
  			<< "Axis        : " << axis << "\n\t  "
			<< "iController : " << controller << "\n\t  "
 	 		<< "LowerLimit  : " << lowerLimit << "\n\t  "
			<< "Upper Limit : " << upperLimit << "\n\t  "
			<< "Position    : " << position << "\n\n";
}

bool Motor::canMotorMove(double position) {
  if (upperLimit == -1 && lowerLimit == -1)
	  return true;
  if (position <= upperLimit && position >= lowerLimit)
	  return true;
  return false;
}


