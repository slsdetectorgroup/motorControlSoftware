#pragma once

class Interface;

#include <string>
#include <vector>

class Fwheel {
public:
  Fwheel(int index, std::string name, std::string serialNumber,
         std::vector<double> valueList);
  /** checks if serial number matches with the one for usp port properties */
  static bool CheckFWSerialNumber(int usbport, std::string serialNumber);
  std::string getName();
  std::string getSerialNumber();
  std::vector<double> getValueList();
  void setInterface(Interface *interface);
  Interface *getInterface();
  double getValue();
  void setValue(double currentValue);
  void print();

private:
  void setStartPosition();
  int index;
  std::string name;
  std::string serialNumber;
  std::vector<double> valueList;
  double currentValue;
  Interface *interface;
};
