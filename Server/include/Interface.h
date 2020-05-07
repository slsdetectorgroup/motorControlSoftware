#pragma once

#include <string>

enum InterfaceIndex { CONTROLLER, TUBE, PRESSURE, FILTER_WHEEL };

class Interface {
public:
  Interface(std::string serial, int serialPortNumber, InterfaceIndex index);
  ~Interface();
  std::string getSerial();
  int getSerialPortNumber();

  void ControllerInterface();
  void ControllerWaitForIdle();
  /* Calls ControllerSendCommand handles exceptions */
  std::string ControllerSend(std::string command, bool readBack = false);
  void TubeInterface();
  /* Calls TubeSendCommand handles exceptions */
  std::string TubeSend(std::string command, bool readBack = false);

  void PressureInterface();
  /* Calls PressureGaugeSendCommand handles exceptions */
  std::string PressureGaugeSend(std::string command);

  void FilterWheelInterface();
  /* Calls FilterWheelSendCommand handles exceptions */
  std::string FilterWheelSend(std::string command, bool readBack = false);

private:
  void ValidateController();
  bool ControllerIsIdle(std::string result);
  std::string ControllerSendCommand(std::string command, bool readBack = false);

  void ValidateTube();
  std::string TubeSendCommand(std::string command, bool readBack = false);

  void ValidatePressureGauge();
  std::string PressureGaugeSendCommand(std::string command);

  void ValidateFilterWheel();
  std::string FilterWheelSendCommand(std::string command, bool readBack);

  std::string serial;
  int serialPortNumber;
  int serialfd;
};
