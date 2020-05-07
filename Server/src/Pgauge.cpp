#include "Pgauge.h"
#include "Interface.h"
#include "commonDefs.h"

#include <cstring>
#include <sstream>

#define NUM_PRESSURE_GAUGES_PER_CONTROLLER (2)

Pgauge::Pgauge(Interface *interface) {
  this->interface = interface;
  FILE_LOG(logINFO) << "Pressure Gauge: [usbPort:" << interface->getSerial()
                    << "]";
}

Pgauge::~Pgauge() {
  delete interface;
  FILE_LOG(logINFO) << "Deleting pressure gauge interface and class";
}

Interface *Pgauge::getInterface() { return interface; }

std::string Pgauge::getStatus(int value) {
  switch (value) {
  case 0:
    return "Measurement data okay";
  case 1:
    return "Underrange";
  case 2:
    return "Overrange";
  case 3:
    return "Sensor error";
  case 4:
    return "Sensor off";
  case 5:
    return "No Sensor";
  default:
    return "Identification error";
  }
}

int Pgauge::getInteger(std::string result) {
  std::istringstream iss(result);
  int value = -1;
  iss >> value;
  if (iss.fail()) {
    std::ostringstream oss;
    oss << "Cannot scan integer from result: " + result;
    throw std::runtime_error(oss.str());
  }
  return value;
}

std::vector<PressureGauge> Pgauge::getPressure() {
  while (true) {
    try {
      std::vector<PressureGauge> gauges(NUM_PRESSURE_GAUGES_PER_CONTROLLER);
      std::string result = interface->PressureGaugeSend("PRX\r\n");
      std::stringstream iss(result);
      std::string value;
      // status 1
      if (getline(iss, value, ',')) {
        gauges[0].status = getStatus(getInteger(value));
      }
      // pressure 1
      if (getline(iss, value, ',')) {
        gauges[0].pressure = value;
      }
      // status 2
      if (getline(iss, value, ',')) {
        gauges[1].status = getStatus(getInteger(value));
      }
      // pressure 2
      if (getline(iss, value, ',')) {
        gauges[1].pressure = value.substr(0, value.length() - 2);
      }
      return gauges;
    } catch (std::exception &e) {
      // not a conversion problem
      if (strstr(e.what(), "Cannot scan integer") == NULL) {
        throw;
      }
    }
  }
}

void Pgauge::print() {
  std::vector<PressureGauge> gauges = getPressure();
  std::cout << "Pressure Gauge" << std::endl;
  std::cout << "==============" << std::endl;
  std::cout << "\tUsb port : " << interface->getSerial() << std::endl;
  for (unsigned int i = 0; i < gauges.size(); ++i) {
    std::cout << "\t  [" << i << "]\n"
              << "\t    Status   : " << gauges[i].status << std::endl
              << "\t    Pressure : " << gauges[i].pressure << std::endl;
  }
  std::cout << std::endl;
}
