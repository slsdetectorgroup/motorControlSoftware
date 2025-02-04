#include "Xray.h"
#include "commonDefs.h"
#include <cstring>
#include <iostream>
#include <stdio.h>
#include <unistd.h>

#define TUBE_STANDBY_VALUE (76)
#define TUBE_NUM_SHUTTERS  (4)
#define TUBE_MAX_VOLTAGE   (60)
#define TUBE_MIN_VOLTAGE   (2)
#define TUBE_MAX_CURRENT   (80)
#define TUBE_MIN_CURRENT   (2)

#define SHUTTER_1_OFST (4)
#define SHUTTER_1_MSK  (0xF << SHUTTER_1_OFST)
#define SHUTTER_2_OFST (0)
#define SHUTTER_2_MSK  (0xF << SHUTTER_2_OFST)
#define SHUTTER_3_OFST (4)
#define SHUTTER_3_MSK  (0xF << SHUTTER_3_OFST)
#define SHUTTER_4_OFST (0)
#define SHUTTER_4_MSK  (0xF << SHUTTER_4_OFST)

#define SHUTTER_TRANSITION_VAL (0x8)
#define SHUTTER_ON_VALUE       (0xC)
#define SHUTTER_NOT_CONNECTED  (0x5)

#define TUBE_WARM_UP_MAX_SIZE (61)

#define TUBE_HV_ON (64)
//#define TUBE_HV_TRANSITION_VAL	(12)

Xray::Xray(Interface *interface) : interface(interface), maxTubePower(0) {
    LOG(logINFO) << "Tube: [usbPort:" << interface->getSerial() << "]";
}

Xray::~Xray() {
    delete interface;
    LOG(logINFO) << "Deleting tube interface and class";
}

Interface *Xray::getInterface() { return interface; }

void Xray::setMaxPower(int power) { maxTubePower = power; }

int Xray::getMaxPower() { return maxTubePower; }

int Xray::getInteger(std::string result) {
    size_t star = result.find("*");
    while (star != std::string::npos) {
        result.erase(star, 1);
        star = result.find("*");
    }
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

std::pair<int, int> Xray::getTwoIntegers(std::string result) {
    size_t star = result.find("*");
    while (star != std::string::npos) {
        result.erase(star, 1);
        star = result.find("*");
    }
    std::pair<int, int> values;
    // first value
    std::string p1 = result.substr(0, result.find(':'));
    {
        std::istringstream iss(p1);
        int value = 1;
        iss >> value;
        if (iss.fail()) {
            std::ostringstream oss;
            oss << "Cannot scan 1st integer from result: " + p1;
            throw std::runtime_error(oss.str());
        }
        values.first = value;
    }
    std::string p2 = result.substr(result.find(':') + 1);
    {
        std::istringstream iss(p2);
        int value = 1;
        iss >> value;
        if (iss.fail()) {
            std::ostringstream oss;
            oss << "Cannot scan 2nd integer from result: " + p2;
            throw std::runtime_error(oss.str());
        }
        values.second = value;
    }
    return values;
}

int Xray::getErrorCode() {
#ifdef VIRTUAL
    return 0;
#endif
    while (true) {
        try {
            std::string result = interface->TubeSend("sr:12 ", true);
            return getInteger(result);
        } catch (std::exception &e) {
            // not a conversion problem
            if (strstr(e.what(), "Cannot scan ") == NULL) {
                throw;
            }
        }
    }
}

std::string Xray::getErrorMessage() { 
#ifdef VIRTUAL
    return std::string();
#else
    return interface->TubeSend("er ", true); 
#endif
}

void Xray::clearErrorCode() {
#ifdef VIRTUAL
    return;
#endif
    interface->TubeSend("cl ", false);
    usleep(1 * 1000 * 1000);
}

bool Xray::isOnStandby() {
#ifdef VIRTUAL
    return false;
#endif
    while (true) {
        try {
            std::string result = interface->TubeSend("sr:12 ", true);
            int value = getInteger(result);
            if (value == TUBE_STANDBY_VALUE) {
                return true;
            }
            return false;
        } catch (std::exception &e) {
            // not a conversion problem
            if (strstr(e.what(), "Cannot scan ") == NULL) {
                throw;
            }
        }
    }
}

void Xray::setHVSwitch(bool on) {

    std::ostringstream oss;
    if (isOnStandby()) {
        oss << "Cannot set high voltage. Tube is on "
            << TUBE_STANDBY_ERROR_PHRASE << " mode";
        throw RuntimeError(oss.str());
    }
#ifdef VIRTUAL
    virtualHV = on;
#else
    oss << "hv:" << (on ? 1 : 0) << ' ';
    interface->TubeSend(oss.str());
#endif
}

bool Xray::getHVSwitch() {
#ifdef VIRTUAL
    return virtualHV;
#endif
    int hv = 1;
    while (hv != TUBE_HV_ON && hv != 0) {
        while (true) {
            try {
                std::string result = interface->TubeSend("sr:1 ", true);
                hv = getInteger(result);
                break;
            } catch (std::exception &e) {
                // not a conversion problem
                if (strstr(e.what(), "Cannot scan ") == NULL) {
                    throw;
                }
            }
        }
    }
    if (hv == 0) {
        return false;
    }
    return true;
}

void Xray::validatePower(int voltage, int current) {
    int power = voltage * current;
    if (power > maxTubePower) {
        std::ostringstream oss;
        oss << "Cannot set voltage/current. Exceeds max power: " << maxTubePower
            << " W. [" << power << " W, " << voltage << " kV, " << current
            << " mA]";
        throw RuntimeError(oss.str());
    }
}

void Xray::validateVoltage(int voltage) {
    if (voltage < TUBE_MIN_VOLTAGE || voltage > TUBE_MAX_VOLTAGE) {
        std::ostringstream oss;
        oss << "Cannot set voltage " << voltage << " kV. Range ["
            << TUBE_MIN_VOLTAGE << " - " << TUBE_MAX_VOLTAGE << " kV]";
        throw RuntimeError(oss.str());
    }
}

void Xray::setVoltage(int value) {
    std::ostringstream oss;
    if (isOnStandby()) {
        oss << "Cannot set voltage. Tube is on " << TUBE_STANDBY_ERROR_PHRASE
            << " mode";
        throw RuntimeError(oss.str());
    }
    validateVoltage(value);
    validatePower(value, getCurrent());
#ifdef VIRTUAL
    virtualVoltage = value;
#else
    oss << "sv:" << value << ' ';
    interface->TubeSend(oss.str());
#endif
}

int Xray::getVoltage() {
#ifdef VIRTUAL
    return virtualVoltage;
#endif
    while (true) {
        try {
            std::string result = interface->TubeSend("vn ", true);
            return (getInteger(result) / 1000);
        } catch (std::exception &e) {
            // not a conversion problem
            if (strstr(e.what(), "Cannot scan ") == NULL) {
                throw;
            }
        }
    }
}

void Xray::validateCurrent(int current) {
    if (current < TUBE_MIN_CURRENT || current > TUBE_MAX_CURRENT) {
        std::ostringstream oss;
        oss << "Cannot set current " << current << " mA. Range ["
            << TUBE_MIN_CURRENT << " - " << TUBE_MAX_CURRENT << " mA]";
        throw RuntimeError(oss.str());
    }
}

void Xray::setCurrent(int value) {
    std::ostringstream oss;
    if (isOnStandby()) {
        oss << "Cannot set current. Tube is on " << TUBE_STANDBY_ERROR_PHRASE
            << " mode";
        throw RuntimeError(oss.str());
    }
    validateCurrent(value);
    validatePower(getVoltage(), value);
#ifdef VIRTUAL
    virtualCurrent = value;
#else
    oss << "sc:" << value << ' ';
    interface->TubeSend(oss.str());
#endif
}

int Xray::getCurrent() {
#ifdef VIRTUAL
    return virtualCurrent;
#endif
    while (true) {
        try {
            std::string result = interface->TubeSend("cn ", true);
            return (getInteger(result) / 1000);
        } catch (std::exception &e) {
            // not a conversion problem
            if (strstr(e.what(), "Cannot scan ") == NULL) {
                throw;
            }
        }
    }
}

void Xray::setVoltageAndCurrent(int v, int c) {
    std::ostringstream oss;
    if (isOnStandby()) {
        oss << "Cannot set voltage and current. Tube is on "
            << TUBE_STANDBY_ERROR_PHRASE << " mode";
        throw RuntimeError(oss.str());
    }
    validateVoltage(v);
    validateCurrent(c);
    validatePower(v, c);
#ifdef VIRTUAL
    virtualVoltage = v;
    virtualCurrent = c;
#else
    oss << "sn:" << v << "," << c << ' ';
    interface->TubeSend(oss.str());
#endif
}

std::pair<int, int> Xray::getVoltageAndCurrent() {
#ifdef VIRTUAL
    return std::make_pair(virtualVoltage, virtualCurrent);
#endif
    while (true) {
        try {
            std::string result = interface->TubeSend("gn ", true);
            std::pair<int, int> values = getTwoIntegers(result);
            values.first /= 1000;
            values.second /= 1000;
            return values;
        } catch (std::exception &e) {
            // not a conversion problem
            if (strstr(e.what(), "Cannot scan ") == NULL) {
                throw;
            }
        }
    }
}

int Xray::getActualVoltage() {
#ifdef VIRTUAL
    return virtualVoltage;
#endif
    while (true) {
        try {
            std::string result = interface->TubeSend("va ", true);
            return (getInteger(result) / 1000);
        } catch (std::exception &e) {
            // not a conversion problem
            if (strstr(e.what(), "Cannot scan ") == NULL) {
                throw;
            }
        }
    }
}

int Xray::getActualCurrent() {
#ifdef VIRTUAL
    return virtualCurrent;
#endif
    while (true) {
        try {
            std::string result = interface->TubeSend("ca ", true);
            return (getInteger(result) / 1000);
        } catch (std::exception &e) {
            // not a conversion problem
            if (strstr(e.what(), "Cannot scan ") == NULL) {
                throw;
            }
        }
    }
}

std::pair<int, int> Xray::getActualVoltageAndCurrent() {
#ifdef VIRTUAL
    return std::make_pair(virtualVoltage, virtualCurrent);
#endif
    while (true) {
        try {
            std::string result = interface->TubeSend("ga ", true);
            std::pair<int, int> values = getTwoIntegers(result);
            values.first /= 1000;
            values.second /= 1000;
            return values;
        } catch (std::exception &e) {
            // not a conversion problem
            if (strstr(e.what(), "Cannot scan ") == NULL) {
                throw;
            }
        }
    }
}

int Xray::getShutterStatus(int index) {
#ifdef VIRTUAL
    return (virtualShutter[index - 1] ? 1 : 0);
#endif
    std::string command;
    int mask = 0;
    int offset = 0;
    switch (index) {
    case 1:
        command = "sr:3 ";
        mask = SHUTTER_1_MSK;
        offset = SHUTTER_1_OFST;
        break;
    case 2:
        command = "sr:3 ";
        mask = SHUTTER_2_MSK;
        offset = SHUTTER_2_OFST;
        break;
    case 3:
        command = "sr:4 ";
        mask = SHUTTER_3_MSK;
        offset = SHUTTER_3_OFST;
        break;
    case 4:
        command = "sr:4 ";
        mask = SHUTTER_4_MSK;
        offset = SHUTTER_4_OFST;
        break;
    default:
        std::ostringstream oss;
        oss << "Invalid shutter index " << index;
        throw RuntimeError(oss.str());
    }

    while (true) {
        try {
            std::string result = interface->TubeSend(command, true);
            int value = getInteger(result);
            value = ((value & mask) >> offset);
            return value;
        } catch (std::exception &e) {
            // not a conversion problem
            if (strstr(e.what(), "Cannot scan ") == NULL) {
                throw;
            }
        }
    }
}

bool Xray::isShutterConnected(int index) {
#ifdef VIRTUAL
    return true;
#endif
    int status = getShutterStatus(index);
    if (status == SHUTTER_NOT_CONNECTED) {
        return false;
    }
    return true;
}

void Xray::setShutter(int index, bool on) {
    std::ostringstream oss;
    if (isOnStandby()) {
        oss << "Cannot set shutter. Tube is on " << TUBE_STANDBY_ERROR_PHRASE
            << " mode";
        throw RuntimeError(oss.str());
    }
    if (!isShutterConnected(index)) {
        oss << "Shutter " << index << " not connected";
        throw RuntimeError(oss.str());
    }
#ifdef VIRTUAL
    virtualShutter[index - 1] = on;
#else
    oss << (on ? "os:" : "cs:") << index << ' ';
    interface->TubeSend(oss.str());
#endif
}

bool Xray::getShutter(int index) {
#ifdef VIRTUAL
    return virtualShutter[index - 1];
#endif
    int status = SHUTTER_TRANSITION_VAL;
    while (status == SHUTTER_TRANSITION_VAL) {
        status = getShutterStatus(index);
    }
    LOG(logINFO) << "Shutter " << index << ": " << status;
    if (status == SHUTTER_ON_VALUE) {
        return true;
    }
    return false;
}

void Xray::startWarmup(int voltage) {
    std::ostringstream oss;
    if (isOnStandby()) {
        oss << "Cannot initiate warmup. Tube is on "
            << TUBE_STANDBY_ERROR_PHRASE << " mode";
        throw RuntimeError(oss.str());
    }
    validateVoltage(voltage);
#ifndef VIRTUAL
    oss << "wu:4," << voltage << ' ';
    interface->TubeSend(oss.str());
#endif
    setHVSwitch(true);
}

int Xray::getWarmupTimeRemaining() {
#ifdef VIRTUAL
    return 0;
#endif
    while (true) {
        try {
            std::string result = interface->TubeSend("wt ", true);
            return getInteger(result);
        } catch (std::exception &e) {
            // not a conversion problem
            if (strstr(e.what(), "Cannot scan ") == NULL) {
                throw;
            }
        }
    }
}

bool Xray::isAccessPossible() {
    // if either of the shutters AND the HV is on
    if ((getShutter(1) || getShutter(2) || getShutter(3) || getShutter(4)) &&
        getHVSwitch()) {
        return false;
    }
    return true;
}

void Xray::sendCommand(std::string command) {
#ifdef VIRTUAL
    LOG(logWARNING) << "Cannot send command: [" << command << "] to tube";
#else
    LOG(logINFO) << "Sending command: [" << command << "] to tube";
    interface->TubeSend(command);
#endif
}

std::string Xray::sendCommandAndReadBack(std::string command) {
#ifdef VIRTUAL
    LOG(logWARNING) << "Cannot send command to read back: [" << command << "] to tube";
    return std::string();
#else
    LOG(logINFO) << "Sending command to read back: [" << command << "] to tube";
    return interface->TubeSend(command, true);
#endif
}

void Xray::print() {
    bool standby = isOnStandby();
    bool hv = getHVSwitch();
    int v = getVoltage();
    int c = getCurrent();
    bool s1 = getShutter(1);
    bool s2 = getShutter(2);
    bool s3 = getShutter(3);
    bool s4 = getShutter(4);
    std::cout << "Xray Tube" << std::endl;
    std::cout << "=========" << std::endl;
    std::cout << "\t  Usb port     : " << interface->getSerial() << std::endl
              << "\t  Status       : " << (standby ? "Stand-by" : "on")
              << std::endl
              << "\t  High Voltage : " << (hv ? "on" : "off") << std::endl
              << "\t  Voltage      : " << v << std::endl
              << "\t  Current      : " << c << std::endl
              << "\t  Shutter 1    : " << (s1 ? "on" : "off") << std::endl
              << "\t  Shutter 2    : " << (s2 ? "on" : "off") << std::endl
              << "\t  Shutter 3    : " << (s3 ? "on" : "off") << std::endl
              << "\t  Shutter 4    : " << (s4 ? "on" : "off") << std::endl
              << std::endl;
}
