#pragma once

#include "Interface.h"
class Xray {
 public:
  Xray(Interface* interface);
  Interface* getInterface();
  /**checks if the xray tube is on standby
     @return 1 if it is on standby,-9999 if its switched off, else 0
  */
  int isOnStandby();
  void setHVSwitch(int HVSwitch);	
  int getHVSwitch();
  void setVoltage(int Voltage);	
  void setCurrent(int Current);	
  void setVoltageAndCurrent(int Voltage, int Current);
  int getVoltage();
  int getVoltageActual();
  int getCurrent();
  int getCurrentActual();
  void getVoltageAndCurrent(int &voltage, int &current);
  void getVoltageAndCurrentActual(int &voltage, int &current);
  void setShutter(int Shutter, bool on);	
  int getShutter1();
  int getShutter2();
  int getShutter3();
  int getShutter4();
  void getShutters(int &s1,int &s2,int &s3,int &s4);
  void startWarmup(int warmupVoltage);
  int getWarmupTimeRemaining();
  bool isAccessPossible();
  int getErrorCode();
  std::string getErrorMessage();
  void clearErrorCode();
  void print();
  
 private:
  Interface* interface;
}; 

