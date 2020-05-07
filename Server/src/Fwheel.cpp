
#include "Fwheel.h"
#include "Interface.h"
#include "commonDefs.h"

#include <cstring>
#include <math.h>
#include <sstream>
#include <stdio.h>

#define NUM_FWHEEL_VALUES            (6)
#define FWHEEL_SERIAL_NUM_LINK_PART1 ("more /sys/class/tty/ttyUSB")
#define FWHEEL_SERIAL_NUM_LINK_PART2 ("/device/../../serial")

Fwheel::Fwheel(int index, std::string name, std::string serialNumber,
               std::vector<double> valueList)
    : index(index), name(name), serialNumber(serialNumber),
      valueList(valueList), interface(NULL) {
    currentValue = valueList[0];
    FILE_LOG(logINFO) << "Fwheel  [" << index << "]: [name:" << name
                      << ", serialNumber: " << serialNumber
                      << ", currentValue:" << currentValue << "]";
}

bool Fwheel::CheckFWSerialNumber(int usbport, std::string serialNumber) {
    std::ostringstream oss;
    oss << FWHEEL_SERIAL_NUM_LINK_PART1 << usbport
        << FWHEEL_SERIAL_NUM_LINK_PART2;
    std::string command = oss.str();

    FILE *sysFile = popen(command.c_str(), "r");
    char output[COMMAND_BUFFER_LENGTH];
    memset(output, 0, sizeof(output));
    fgets(output, sizeof(output), sysFile);
    pclose(sysFile);

    if (strstr(output, serialNumber.c_str()) != NULL) {
        FILE_LOG(logINFOGREEN) << "\tSuccess";
        return true;
    }
    FILE_LOG(logINFO) << "Expected to find [" << serialNumber << "] in ["
                      << output << ']';
    FILE_LOG(logWARNING) << "Fail";
    return false;
}

std::string Fwheel::getName() { return name; }

std::string Fwheel::getSerialNumber() { return serialNumber; }

std::vector<double> Fwheel::getValueList() { return valueList; }

void Fwheel::setInterface(Interface *interface) {
    this->interface = interface;
    FILE_LOG(logINFO) << "\tFwheel " << name
                      << ": [usbPort:" << interface->getSerial() << ']';
    setStartPosition();
}

Interface *Fwheel::getInterface() { return interface; }

void Fwheel::setStartPosition() { setValue(valueList[0]); }

double Fwheel::getValue() {
    std::string result = interface->FilterWheelSend("pos?\r", true);
    std::istringstream iss(result);
    int position = -1;
    iss >> position;
    if (iss.fail()) {
        std::ostringstream oss;
        oss << "Cannot scan integer from result for filter wheel " << name
            << ": " + result;
        throw RuntimeError(oss.str());
    }
    if (position < 1 || position > NUM_FWHEEL_VALUES) {
        std::ostringstream oss;
        oss << "Invalid position " << position << " got from filter wheel "
            << name << ": " + result;
        throw RuntimeError(oss.str());
    }
    currentValue = valueList[position - 1];
    return currentValue;
}

void Fwheel::setValue(double currentValue) {
    std::ostringstream oss;
    for (unsigned int i = 0; i < valueList.size(); ++i) {
        if (fabs(valueList[i] - currentValue) < FWHEEL_TOLERANCE) {
            oss << "pos=" << i + 1 << '\r';
            // repeat 5 times max if it reads weird
            int loop = 0;
            for (;;) {
                try {
                    interface->FilterWheelSend(oss.str());
                    if (currentValue != getValue()) {
                        oss << "Filter wheel " << name
                            << " could not be moved to " << currentValue
                            << ". It is at " << this->currentValue;
                        throw RuntimeError(oss.str());
                    }
                    return;
                } catch (const RuntimeError &e) {
                    ++loop;
                    if (loop == 5) {
                        throw;
                    }
                }
            }
        }
    }
    oss << "Invalid absorption value " << currentValue;
    throw RuntimeError(oss.str());
}

void Fwheel::print() {
    std::cout << "\tFilter Wheel [" << index << ']' << std::endl;
    std::cout << "\t\t[Name:" << name << ", SerialNumber:" << serialNumber
              << ", UsbPort:" << interface->getSerial() << ']' << std::endl;
    std::cout << "\t\t[Values: ";
    for (unsigned int i = 0; i < valueList.size(); ++i) {
        std::cout << valueList[i];
        if (i < valueList.size() - 1)
            std::cout << ", ";
    }
    std::cout << ']' << std::endl << std::endl;
}
