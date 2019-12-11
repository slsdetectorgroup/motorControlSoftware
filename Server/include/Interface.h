#pragma once

#include <string>


enum InterfaceIndex{CONTROLLER, TUBE, PRESSURE, FILTER_WHEEL};


class Interface {
  public:
  Interface(std::string serial, InterfaceIndex index);
  ~Interface();
  std::string getSerial();

  void ControllerInterface();
  void ControllerWaitForIdle();
  /* Calls ControllerSendCommand
  handles exceptions, if validate, then throws after fixed number of attempts */
  std::string ControllerSend(std::string command, bool readBack = false, bool validate = false);



  void TubeInterface();
  void PressureInterface();
  void FilterWheelInterface();


  

 /**Sends the command to the XRayTube via interface
   @param c the command to be sent to the tube
   @param rb is passed as 1 if buffer needs to be read back from tube after sending command, else 0 is passed
   @param value is a pointer which points to the integer value of the buffer
   @param value2 is a pointer which points to the second integer value of the buffer. for cmd gn/ga
   @returns the binary value of the buffer read from the controller.
 */
  char* send_command_to_tube(char* c, int rb, int &value, int &value2);


  /**
   * Sends ayt to the pressure gauge controller to get TPG36 for model
   * @returns true if serial port is connected to the pressure gauge controller, else false
   */
  bool checkPressureGaugePort();

  /**
   * Read from pressure gauge controller. 5 attempts
   * @param c buffer to place data read
   * @param size size of buffer allocated
   * @returns number of bytes read
   */
  int read_from_pressure(char* c, const int size);

  /**
   * Sends the command to the pressure gauge controller, and read back if required
   * @param c the command to be sent, also value read placed in this buffer
   * @param size size of buffer allocated for c
   * @param rb true if readback required
   * @param enq true if enquiry needs to be sent after getting acknowledge (not needed only for com)
   */
  bool send_command_to_pressure(char* c, const int size, bool rb = true, bool enq = true);

  /**Sends the command to the filter wheel via interface
    @param c the command to be sent
    @param rb is passed as 1 if buffer needs to be read back from tube after sending command, else 0 is passed
    @returns buffer read from the controller.
  */
   char* send_command_to_fw(char* c, int rb);

private:
  void ValidateController();
  std::string ControllerSendCommand(std::string command, bool readBack = false, bool verbose = true);

  std::string serial;
  int serialfd;
};
