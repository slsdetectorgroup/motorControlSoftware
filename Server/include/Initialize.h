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
using namespace std;


class Initialize {
 public:
  Initialize();
  ~Initialize();

  /**executes commands from the command line through both server and local
	 @param args array of command line arguments (not including the object name eg ./localClient
	 @param mess message to be sent to client after executing the command
	 @returns 0 for successful command execution,-1 for fail
  */	
  int executeCommand(std::vector<std::string> args, char mess[]);

 private:

  int ReadWarmupTimestamps(string const fName);
  int WriteWarmupTimestamps(string const fName);
  void ReadConfigFile();
  void UpdateInterface(InterfaceIndex index);
  void TubeMode(vector<string> args);
  void PressureMode();
  void FwheelMode(vector<string> args);
  void ReferencePointMode(vector<string> args);
  void FluorescenceMode(vector<string> args);
  void ControllerMode(vector<string> args);
  void MotorMode(vector<string> args);

  vector<bool> usbSerialPortsUsed;
  Xray* xrayTube;
  int maxTubePower;
  vector<string> warmupTimings;
  Pgauge* pgauge;
  vector<Fwheel*> fwheel;
  vector<ReferencePoint*> referencePoint;
  vector<Fluorescence*> fluorescence;
  Slit* slit;
  vector<Controller*> controller;
  vector<Motor*> motor;
};

