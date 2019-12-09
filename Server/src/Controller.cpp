#include "Controller.h"
#include "Motor.h"
#include "Interface.h"
#include "commonDefs.h"

#include <iostream>
#include <sstream>
#include <cstring>
#include <iterator>

Controller::Controller(const int index, const std::string name, const std::string serialNumber)
: index(index), name(name), serialNumber(serialNumber), interface(NULL) {
    motor.resize(MAX_NUM_MOTORS_PER_CONTROLLER);
    for (unsigned int i = 0; i < motor.size(); ++i) {
        motor[i] = NULL;
    }
    FILE_LOG(logINFO) << "Contoller  [" << index << "]: [name:" << name << ", serialNumber:" << serialNumber << "]";
}

bool Controller::CheckControllerSerialNumber(int usbport, Interface* interface, std::string serialNumber) {
    std::string result = interface->controllerSendCommand("getserialno ", true);
    std::istringstream iss(result);
    if (iss.good()) {
        std::string snum;
	    iss >> snum;
        if (snum == serialNumber) {
            return true;
        }
   	}
	return false;
}

std::string Controller::getName() const {
    return name;
}

std::string Controller::getSerialNumber() const {
    return serialNumber;
}

void Controller::setInterface(Interface* interface) {
    this->interface = interface;
	FILE_LOG(logINFO) << "\tController " << name << ": [usbPort:" << interface->getSerial() << ']';
}

Interface* Controller::getInterface() const {
    return interface;
}

void Controller::setMotor(const int axis, Motor* motor) {
    // invalid axis
    if (axis < 0 || axis >= MAX_NUM_MOTORS_PER_CONTROLLER) {
        std::ostringstream oss; 
        oss << "Invalid axis index " << axis << " to set motor " << motor->getName() << " for controller " + name;
        throw RuntimeError(oss.str());
    }
    // motor has been set before
    if (this->motor[axis] != NULL) {
        std::ostringstream oss; 
        oss << "Duplicate axis index " << axis << " to set motor " << motor->getName() << " for controller " + name;
        throw RuntimeError(oss.str());   
    }
    this->motor[axis] = motor;
}

Motor* Controller::getMotor(int axis) const {
    return motor[axis];
}

void Controller::updateAxisEnabled() {
    for (int i = 0; i < motor.size(); ++i) {
        int axis = i + 1;
        int enable = motor[i] == NULL ? 0 : 1;
        std::ostringstream oss;
        oss << enable << ' ' << axis << " setaxis ";
        interface->controllerSendCommand(oss.str());
    }
}

void Controller::debugPositions() {
    std::string result = interface->controllerSendCommand("pos ", true);
    std::istringstream iss(result);
    std::vector<double> pos = std::vector<double>(std::istream_iterator<double>(iss), std::istream_iterator<double>());
    for (unsigned int i = 0; i < pos.size(); ++i) {
        if (motor[i] != NULL) {
            motor[i]->setPosition(pos[i]);
        }
    }
}

void Controller::moveRel(double position, int axis) {
    double positions[3] = {0.0, 0.0, 0.0};
    positions[axis] = position;

    std::ostringstream oss;
    oss << positions[0] << ' ' << positions[1] << ' ' << positions[2] << " r ";
    interface->controllerSendCommand(oss.str());  
    debugPositions(); 
}


void Controller::moveAbs(double position, int axis) {
    double positions[3] = {0.0, 0.0, 0.0};
    for (int i = 0; i < 3; ++i) {
        if (motor[i] !=  NULL) {
            positions[i] = motor[i]->getPosition();
        }
    }
    positions[axis] = position;

    std::ostringstream oss;
    oss << positions[0] << ' ' << positions[1] << ' ' << positions[2] << " m ";
    interface->controllerSendCommand(oss.str());   
    debugPositions();
}


void Controller::calibrate(int axis) {
    std::ostringstream oss;
    oss << axis + 1 << " ncal ";
    interface->controllerSendCommand(oss.str());   
    debugPositions();
}

void Controller::sendCommand(std::string command) {
    interface->controllerSendCommand(command);
}

std::string Controller::sendCommandAndReadBack(std::string command) {
    return interface->controllerSendCommand(command, true);
}

void Controller::print() {
  std::cout << "\tController [" << index << "]\n\t "
  			<< "Name         : " << name << "\n\t "
  			<< "SerialNumber : " << serialNumber << "\n\t "
			<< "UsbPort      : " << interface->getSerial() << "\n\t "
 	 		<< "Motors 0     : " << ((motor[0] == NULL) ? "None" : motor[0]->getName()) << "\n\t "
 	 		<< "Motors 1     : " << ((motor[1] == NULL) ? "None" : motor[1]->getName()) << "\n\t "
 	 		<< "Motors 2     : " << ((motor[2] == NULL) ? "None" : motor[2]->getName()) << "\n\n";
}