#pragma once 

class Interface;

#include <vector>
#include <string>

class Fwheel{
public:
	Fwheel(int index, std::string name, std::string serialNumber, std::vector<double> values);
	/** checks if serial number matches with the one for usp port properties */
	static bool  CheckFWSerialNumber(int usbport, std::string serialNumber);
	std::string getName();
	std::string getSerialNumber();
	void setInterface(Interface* interface);
	Interface* getInterface();
	void setStartPosition();
	double getValue();
	int setValue(double currentValue);
	void print();

private:
	int index;
	std::string name;
	std::string serialNumber;
	std::vector<double> values;
	double currentValue;
	Interface* interface;
};

