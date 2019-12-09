
#include "Fwheel.h"
#include "Interface.h"
#include "commonDefs.h"

#include <sstream>
#include <cstring>
#include <stdio.h>

using namespace std;

#define NUM_FWHEEL_VALUES	(6)


Fwheel::Fwheel(int index, std::string name, std::string serialNumber, vector<double> values)
	:index(index), name(name), serialNumber(serialNumber), values(values), interface(NULL) {
  currentValue = values[0];
  FILE_LOG(logINFO) << "Fwheel  [" << index << "]: [name:" << name << ", serialNumber: " << serialNumber <<  ", currentValue:" << currentValue << "]";
}

bool Fwheel::CheckFWSerialNumber(int usbport, std::string serialNumber) {
	std::ostringstream oss;
	oss << "more /sys/class/tty/ttyUSB" << usbport << "/device/../../serial";
	std::string command = oss.str();

	FILE* sysFile = popen(command.c_str(), "r");
	char output[COMMAND_BUFFER_LENGTH];
	memset(output, 0, sizeof(output));
	fgets(output, sizeof(output), sysFile);
	pclose(sysFile);

	if (strstr(output, serialNumber.c_str()) != NULL) {
		return true;
	}
	return false;
}

std::string Fwheel::getName() {
	return name;
}

std::string Fwheel::getSerialNumber() {
	return serialNumber;
}

void Fwheel::setInterface(Interface* interface) {
	this->interface = interface;
	FILE_LOG(logINFO) << "\tFwheel " << name << ": [usbPort:" << interface->getSerial() << ']';
}

Interface* Fwheel::getInterface() {
  return interface;
}


void Fwheel::setStartPosition() {
	//start position
	interface->send_command_to_fw((char*)"pos=1",0);
	currentValue=values[0];
}


double Fwheel::getValue() {
	return currentValue;
}

int Fwheel::setValue(double currentValue) {
	char command[COMMAND_BUFFER_LENGTH];

	std::string prefix = "pos=";
	for(int i = 0; i < NUM_FWHEEL_VALUES; ++i) 	{
		if (values[i] == currentValue) {
			memset(command, 0, sizeof(command));
			sprintf(command, "%s%d", prefix.c_str(), i + 1); //+1 because positions for filter wheel in display are from 1 to 6
			interface->send_command_to_fw(command, 0);
			this->currentValue = currentValue;
			return 1;
		}
	}

	//did not find the currentValue in the list
	return 0;
}


void Fwheel::print() {
	std::cout << "\tFilter Wheel" << index << std::endl;
	std::cout << "\t\t[Name:" << name << ", SerialNumber:" << serialNumber << ", UsbPort:" << interface->getSerial() << ']' << std::endl;
	std::cout << "\t\t[Values: ";
	for (unsigned int i = 0; i < values.size(); ++i) {
		std::cout << values[i] << ", ";
	}  
	std::cout << ']' << std::endl << std::endl;
}


