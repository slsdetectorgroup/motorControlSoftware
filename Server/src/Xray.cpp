#include "Xray.h"
#include "commonDefs.h"

#include <iostream>
#include <stdio.h>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <time.h>

#define TUBE_STANDBY_VALUE      (76)
#define TUBE_NUM_SHUTTERS       (4)
#define TUBE_MAX_VOLTAGE        (60)
#define TUBE_MIN_VOLTAGE        (2)
#define TUBE_MAX_CURRENT        (80)
#define TUBE_MIN_CURRENT        (2)

#define SHUTTER_1_OFST          (4)
#define SHUTTER_1_MSK           (0xF << SHUTTER_1_OFST)
#define SHUTTER_2_OFST          (0)
#define SHUTTER_2_MSK           (0xF << SHUTTER_2_OFST)
#define SHUTTER_3_OFST          (4)
#define SHUTTER_3_MSK           (0xF << SHUTTER_3_OFST)
#define SHUTTER_4_OFST          (0)
#define SHUTTER_4_MSK           (0xF << SHUTTER_4_OFST)

#define SHUTTER_ON_VALUE        (0xC)
#define SHUTTER_NOT_CONNECTED   (0x5)

#define TUBE_WARM_UP_MAX_SIZE   (61)

#define TUBE_HV_TRANSITION_VAL	(12)

#define WARMUP_FILE "warmupTimestamps.txt"

Xray::Xray(Interface* interface) 
    : interface(interface),  maxTubePower(0) {
    FILE_LOG(logINFO) << "Tube: [usbPort:" << interface->getSerial() << "]";
    try {
        readAllWarmupTimestamps();
    } catch (...) {
        FILE_LOG(logWARNING) << "Continuing anyway...";
    }
}

Xray::~Xray() {
    delete interface;
	FILE_LOG(logINFO) << "Deleting tube interface and class";
}

Interface* Xray::getInterface() {
    return interface;
}

int Xray::setMaxPower(int power) {
    maxTubePower = power;
}

int Xray::getMaxPower() {
    return maxTubePower;
}

int Xray::getInteger(std::string result) {
    result.erase(result.begin());
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
    result.erase(result.begin());
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
    std::string result = interface->TubeSend("sr:12 ", true);
    return getInteger(result);
}

std::string Xray::getErrorMessage() {
    return interface->TubeSend("er ", true);
}

void Xray::clearErrorCode() {
    interface->TubeSend("cl ", false);
}

bool Xray::isOnStandby() {
    std::string result = interface->TubeSend("sr:12 ", true);
    int value = getInteger(result);
    if (value == TUBE_STANDBY_VALUE) {
        return true;
    }
    return false;
}

void Xray::setHVSwitch(bool on) {
    if (isOnStandby()) {
        throw RuntimeError ("Cannot set high voltage. Tube is on Stand-by mode.");
    }
    std::ostringstream oss;
    oss << "hv:" << (on ? 1 : 0) << ' ';
    interface->TubeSend(oss.str());
}

bool Xray::getHVSwitch() {
    int hv = TUBE_HV_TRANSITION_VAL;
    while(hv == TUBE_HV_TRANSITION_VAL) {
        std::string result = interface->TubeSend("sr:1 ", true);
        hv = getInteger(result);
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
        oss << "Cannot set voltage/current. Exceeds max power: " << maxTubePower << " W. [" << power << " W, " << voltage << " kV, " << current << " mA]";
        throw RuntimeError (oss.str());        
    }
}

void Xray::validateVoltage(int voltage) {
    if (voltage < TUBE_MIN_VOLTAGE || voltage > TUBE_MAX_VOLTAGE) {
        std::ostringstream oss;
        oss << "Cannot set voltage " << voltage << " kV. Range [" << TUBE_MIN_VOLTAGE << " - " << TUBE_MAX_VOLTAGE << " kV]";
        throw RuntimeError (oss.str());
    }
}

void Xray::setVoltage(int value) {
    if (isOnStandby()) {
        throw RuntimeError ("Cannot set voltage. Tube is on Stand-by mode.");
    }
    validateVoltage(value);
    validatePower(value, getCurrent());
    std::ostringstream oss;
    oss << "sv:" << value << ' ';
    interface->TubeSend(oss.str());
}

int Xray::getVoltage() {
    std::string result = interface->TubeSend("vn ", true);
    return (getInteger(result) / 1000);
}

void Xray::validateCurrent(int current) {
    if (current < TUBE_MIN_CURRENT || current > TUBE_MAX_CURRENT) {
        std::ostringstream oss;
        oss << "Cannot set current " << current << " mA. Range [" << TUBE_MIN_CURRENT << " - " << TUBE_MAX_CURRENT << " mA]";
        throw RuntimeError (oss.str());
    }
}

void Xray::setCurrent(int value)  {
    if (isOnStandby()) {
        throw RuntimeError ("Cannot set current. Tube is on Stand-by mode.");
    }
    validateCurrent(value);
    validatePower(getVoltage(), value);
    std::ostringstream oss;
    oss << "sc:" << value << ' ';
    interface->TubeSend(oss.str());
}

int Xray::getCurrent() {
    std::string result = interface->TubeSend("cn ", true);
    return (getInteger(result) / 1000);
}

void Xray::setVoltageAndCurrent(int v, int c)	{
    if (isOnStandby()) {
        throw RuntimeError ("Cannot set voltage and current. Tube is on Stand-by mode.");
    }
    validateVoltage(v);
    validateCurrent(c);
    validatePower(v, c);
    std::ostringstream oss;
    oss << "sn:" << v << "," << c << ' ';
    interface->TubeSend(oss.str());
}

std::pair <int, int> Xray::getVoltageAndCurrent() {
    std::string result = interface->TubeSend("gn ", true);
    std::pair <int, int> values = getTwoIntegers(result);
    values.first /= 1000;
    values.second /= 1000;
    return values;
}
 
int Xray::getActualVoltage() {
    std::string result = interface->TubeSend("va ", true);
    return getInteger(result);
}
 
int Xray::getActualCurrent() {
    std::string result = interface->TubeSend("ca ", true);
    return getInteger(result);
}

std::pair <int, int> Xray::getActualVoltageAndCurrent() {
    std::string result = interface->TubeSend("ga ", true);
    std::pair <int, int> values = getTwoIntegers(result);
    values.first /= 1000;
    values.second /= 1000;
    return values;
}

int Xray::getShutterStatus(int index) {
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
            offset = SHUTTER_3_OFST;
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
            throw RuntimeError ("Invalid shutter index " + index);
    }
    std::string result = interface->TubeSend(command, true);
    int value = getInteger(result);
    value = ((value & mask) >> offset);
    return value;
}

bool Xray::isShutterConnected(int index) {
    int status = getShutterStatus(index);
    if (status == SHUTTER_NOT_CONNECTED) {
        return false;
    }
    return true;
}

void Xray::setShutter(int index, bool on)	{
    if (isOnStandby()) {
        throw RuntimeError ("Cannot set shutter. Tube is on Stand-by mode.");
    }
    if (!isShutterConnected(index)) {
        std::ostringstream oss;
        oss << "Shutter " << index << " not connected";
        throw RuntimeError (oss.str());
    }
    std::ostringstream oss;
    oss << (on ? "os:" : "cs:") << index << ' ';
    interface->TubeSend(oss.str());
}

bool Xray::getShutter(int index) {
    int status = getShutterStatus(index);
    if (status == SHUTTER_ON_VALUE) {
        return true;
    }
    return false;
}

void Xray::startWarmup(int voltage) {
    if (isOnStandby()) {
        throw RuntimeError ("Cannot initiate warmup. Tube is on Stand-by mode.");
    }
    validateVoltage(voltage);

    for (int i = 1; i <= TUBE_NUM_SHUTTERS; ++i) {
        if (getShutter(i)) {
            setShutter(i, false);
        }
    }
    std::ostringstream oss;
    oss << "wu:4," << voltage << ' ';
    interface->TubeSend(oss.str());
    setHVSwitch(true);
    setWarmupTimestamp(voltage);
}


int Xray::getWarmupTimeRemaining() {
    std::string result = interface->TubeSend("wt ", true);
    return getInteger(result);
}


bool Xray::isAccessPossible() {
    //if either of the shutters AND the HV is on
    if ((getShutter(1) || getShutter(2) || getShutter(3) || getShutter(4)) && getHVSwitch()) {
        return false;
    }
    return true;
}

void Xray::sendCommand(std::string command) {
    FILE_LOG(logINFO) << "Sending command: [" << command << "] to tube";
    interface->TubeSend(command);
}

std::string Xray::sendCommandAndReadBack(std::string command) {
    FILE_LOG(logINFO) << "Sending command to read back: [" << command << "] to tube";
    return interface->TubeSend(command, true);
}

void Xray::readAllWarmupTimestamps() {
    // initialize
    warmupTimings.resize(TUBE_WARM_UP_MAX_SIZE);
	for (unsigned int i = 0; i < warmupTimings.size(); ++i) {
		warmupTimings[i].assign("unknown");
	}   

    FILE_LOG(logINFO) << "Reading all warm up time stamps from file";
	std::ifstream inFile;
	inFile.open(WARMUP_FILE, std::ifstream::in);
	if (!inFile.is_open()) {
        std::ostringstream oss;
        oss << "Could not open warm up timestamps file " << WARMUP_FILE;
        throw RuntimeError (oss.str());
    }
    while(inFile.good()) {
        std::string line;
        getline(inFile, line);
        if (line.length() == 0) {
            continue;
        }
        std::istringstream iss(line);
        int voltage = -1;
        iss >> voltage;
        if (iss.fail()) {
            std::ostringstream oss;
            oss << "Cannot scan voltage " + line + " from file";
            throw RuntimeError (oss.str());
        }
        line.erase(0, 15);	   
        warmupTimings[voltage].assign(line);
    }
    inFile.close();
}

void Xray::writeAllWarmupTimestamps() {

	std::ofstream outFile;
	outFile.open(WARMUP_FILE);
	if (!outFile.is_open()) {
        std::ostringstream oss;
        oss << "Could not open warm up timestamps file " << WARMUP_FILE;
        throw RuntimeError (oss.str());
    }
	for (unsigned int i = 2; i < warmupTimings.size(); ++i) {
		outFile << std::setw(15) << std::left << i << warmupTimings[i] << std::endl;
    }
	outFile.close();
}

std::string Xray::getWarmupTimestamp(int voltage) {
    if (voltage < 2 || voltage >= TUBE_WARM_UP_MAX_SIZE) {
        std::ostringstream oss;
        oss << "Voltage " << voltage << " out of range. Range [2 - " << TUBE_WARM_UP_MAX_SIZE << ']';
        throw RuntimeError (oss.str());
    }
    return warmupTimings[voltage];
}

void Xray::setWarmupTimestamp(int voltage) {
    if (voltage < 2 || voltage >= TUBE_WARM_UP_MAX_SIZE) {
        std::ostringstream oss;
        oss << "Voltage " << voltage << " out of range. Range [2 - " << TUBE_WARM_UP_MAX_SIZE << ']';
        throw RuntimeError (oss.str());
    }

    // get timestamp
    time_t tdate;
    time(&tdate);
    std::string timestamp = ctime(&tdate);
    timestamp.erase(timestamp.begin() + 24, timestamp.end());
    for (int i = voltage; i >= 0; --i) {
        warmupTimings[i] = timestamp;
    }
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
	std::cout   << "Xray Tube" << std::endl;
	std::cout   << "=========" << std::endl;
    std::cout   << "\t  Usb port     : " << interface->getSerial() << std::endl
                << "\t  Status       : " << (standby ? "Stand-by" : "on") << std::endl
                << "\t  High Voltage : " << (hv ? "on" : "off") << std::endl
                << "\t  Voltage      : " << v << std::endl
                << "\t  Current      : " << c << std::endl
                << "\t  Shutter 1    : " << (s1 ? "on" : "off") << std::endl
                << "\t  Shutter 2    : " << (s2 ? "on" : "off") << std::endl
                << "\t  Shutter 3    : " << (s3 ? "on" : "off") << std::endl
                << "\t  Shutter 4    : " << (s4 ? "on" : "off") << std::endl << std::endl;
}
