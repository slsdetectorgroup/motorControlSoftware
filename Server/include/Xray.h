#pragma once

#include "Interface.h"

#include <vector>
#include <utility>

class Xray {
   public:
   Xray(Interface* interface);
   ~Xray();
   Interface* getInterface();
   int setMaxPower(int power);
   int getMaxPower();
   int getErrorCode();
   std::string getErrorMessage();
   void clearErrorCode();
   bool isOnStandby();
   void setHVSwitch(bool on);	
   bool getHVSwitch();
   void setVoltage(int value);	
   int getVoltage();
   void setCurrent(int value);	
   int getCurrent();
   void setVoltageAndCurrent(int v, int c);
   std::pair <int, int> getVoltageAndCurrent();
   int getActualVoltage();
   int getActualCurrent();
   std::pair <int, int> getActualVoltageAndCurrent();
   bool isShutterConnected(int index);
   void setShutter(int index, bool on);	
   bool getShutter(int index);
   void startWarmup(int voltage);
   int getWarmupTimeRemaining();
   bool isAccessPossible();
   void writeAllWarmupTimestamps();
   std::string getWarmupTimestamp(int voltage);

   // advanced
   void sendCommand(std::string command);
   std::string sendCommandAndReadBack(std::string command);
   void print();
  
   private:
   int getInteger(std::string result);
   std::pair<int, int> getTwoIntegers(std::string result);
   void validatePower(int voltage, int current);
   void validateVoltage(int voltage);
   void validateCurrent(int current);
   int getShutterStatus(int index);
   void readAllWarmupTimestamps();
   void setWarmupTimestamp(int voltage);

   Interface* interface;
   int maxTubePower;
   std::vector <std::string> warmupTimings;

}; 

