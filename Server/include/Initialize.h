#pragma once

class Xray;
class Pgauge;
class Fwheel;
class ReferencePoints;
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
  void OnlyTubeCommand();
  void OnlyFluorescenceCommand(std::string name);
  void RestrictedForSlitAndFluorescence(std::string name);
  void UpdateSlitLimits(std::string name);
  int GetFwheelIndex(std::string name);  
  int GetFluorescenceIndex(std::string name);
  int GetMotorIndex(std::string name);
  int GetControllerIndex(std::string name);
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
  Pgauge* pgauge;
  std::vector<Fwheel*> fwheel;
  ReferencePoints* referencePoints;
  std::vector<Fluorescence*> fluorescence;
  Slit* slit;
  std::vector<Controller*> controller;
  std::vector<Motor*> motor;
  int maxTubePower;
};

