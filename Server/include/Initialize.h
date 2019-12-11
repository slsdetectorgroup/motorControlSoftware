#pragma once

class Xray;
class Pgauge;
class Fwheel;
class ReferencePoint;
class Fluorescence;
class Slit;
class Controller;
class Motor;
#include "Interface.h"

#include <vector>
#include <string>
#include "time.h"


class Initialize {
 public:
  Initialize();
  ~Initialize();
  std::string executeCommand(std::vector<std::string> args);

 private:

  void RestrictedCommand(std::string name);
  void UpdateSlitLimits(std::string name);
  int GetMotorIndex(std::string name);
  int GetControllerIndex(std::string name);
  int ReadWarmupTimestamps(std::string const fName);
  int WriteWarmupTimestamps(std::string const fName);
  void ReadConfigFile();
  void UpdateInterface(InterfaceIndex index);
  void TubeMode(std::vector<std::string> args);
  void PressureMode();
  void FwheelMode(std::vector<std::string> args);
  void ReferencePointMode(std::vector<std::string> args);
  void FluorescenceMode(std::vector<std::string> args);
  void ControllerMode(std::vector<std::string> args);
  void MotorMode(std::vector<std::string> args);

  std::vector<bool> usbSerialPortsUsed;
  Xray* xrayTube;
  int maxTubePower;
  std::vector<std::string> warmupTimings;
  Pgauge* pgauge;
  std::vector<Fwheel*> fwheel;
  std::vector<ReferencePoint*> referencePoint;
  std::vector<Fluorescence*> fluorescence;
  Slit* slit;
  std::vector<Controller*> controller;
  std::vector<Motor*> motor;
};

