#pragma once

#include <string>


enum InterfaceIndex{CONTROLLER, TUBE, PRESSURE, FILTER_WHEEL};


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
  char* send_command_to_fw(char* c, int rb);

private:
  void ValidateController();
  bool ControllerIsIdle(std::string result, bool validate = false);
  std::string ControllerSendCommand(std::string command, bool readBack = false);

  void ValidateTube();
  std::string TubeSendCommand(std::string command, bool readBack = false);

  void ValidatePressureGauge();
  std::string PressureGaugeSendCommand(std::string command);

  std::string serial;
  int serialPortNumber;
  int serialfd;
};
