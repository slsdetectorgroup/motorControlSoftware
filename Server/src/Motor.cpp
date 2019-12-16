#include "Motor.h"
#include "commonDefs.h"

#include <sstream>
#include <math.h>

#define MOTOR_TOLERANCE	(0.0001)

Motor::Motor(int index, std::string name, int axis, int controller, double lowerLimit, double upperLimit)
  : index(index),name(name),axis(axis),controller(controller), lowerLimit(lowerLimit),upperLimit(upperLimit),position(0.0) {
	// validations
	if (lowerLimit > upperLimit) {
		std::ostringstream oss; 
		oss << "Invalid limits to set motor " << name;
		throw RuntimeError(oss.str());			
	}
	FILE_LOG(logINFO) << "Motor [" << index << "]: [name:" << name << ", axis:" << axis <<  ", iController:" << controller << "]";
}

bool Motor::matches(double position1, double position2) {
	if (fabs(position1 - position2) < MOTOR_TOLERANCE) {
		return true;
	}
	return false;
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

bool Motor::canMotorMove(double position) {
  if (upperLimit == -1 && lowerLimit == -1)
	  return true;
  if (position >= lowerLimit && position <= upperLimit)
	  return true;
  return false;
}

void Motor::print() {
  std::cout << "\tMotor [" << index << "]" << std::endl
  			<< "\t  Name        : " << name << std::endl
  			<< "\t  Axis        : " << axis << std::endl
			<< "\t  iController : " << controller << std::endl
 	 		<< "\t  LowerLimit  : " << lowerLimit << std::endl
			<< "\t  Upper Limit : " << upperLimit << std::endl
			<< "\t  Position    : " << position << std::endl << std::endl;
}

