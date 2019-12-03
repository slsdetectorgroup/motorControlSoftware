#pragma once

#include <termios.h>  /* POSIX terminal control definitions */

 /**
  *@short Defines the USB objects for each connection to the controller.
 */
class Interface
{
 public:
  
 /**Constructor for USB for motors
	Initializes/opens the port according to the parameter.<br> 
	For eg. object usb0 would have '/dev/ttyUSB0' as serial
	@param serial specifies the port for the interface object.
	@param success becomes true if this serial port was successfully initialized
 */
  Interface(char* serial, bool* success);

#ifndef LASERBOX
 /**Constructor for serial for XRay Tube
	Initializes/opens the port according to the parameter.<br> 
	@param serial specifies the port for the interface object.
	@param success becomes true if this serial port was successfully initialized
	@param xray a useless variable to differentiate between the 2 constructors
 */
  Interface(char* serial,bool* success, bool xray);
#ifdef VACUUMBOX
  /**Constructor for serial for pressure gauge for vacuum box
 	Initializes/opens the port according to the parameter.<br>
 	@param serial specifies the port for the interface object.
 	@param pressure a useless variable to differentiate between the 2 constructors
 	@param success becomes true if this serial port was successfully initialized
  */
   Interface(char* serial,bool pressure, bool* success);
#endif
#else
  /**Constructor for serial for filter wheel
 	Initializes/opens the port according to the parameter.<br>
 	In this case the serial passed would always be '/dev/ttyS0'
 	@param fw is a useless variable to differentiate between the 2 constructors
 	@param serial specifies the port for the interface object.
 	@param success becomes true if this serial port was successfully initialized
  */
   Interface(bool fw,char* serial,bool* success);
#endif

 /**gets the port of the object.
   @returns the port
 */  
  char* getSerial();

 /**closes the port of the object.
 */
  void close_serialfd();

 /**For debugging - Prints out the parameters of an interface object: Serial & Serialfd.
 */
  void print();

 /**Sends the command to the controller via the port of this object.
   @param c the command to be sent to the contoller
   @param rb is passed as 1 if buffer needs to be read back from controller after sending command, else 0 is passed
   @returns buffer read from the controller.
 */
  char* send_command(char* c, int rb);
  

#ifndef LASERBOX
 /**Sends the command to the XRayTube via interface
   @param c the command to be sent to the tube
   @param rb is passed as 1 if buffer needs to be read back from tube after sending command, else 0 is passed
   @param value is a pointer which points to the integer value of the buffer
   @param value2 is a pointer which points to the second integer value of the buffer. for cmd gn/ga
   @returns the binary value of the buffer read from the controller.
 */
  char* send_command_to_tube(char* c, int rb, int &value, int &value2);

#ifdef VACUUMBOX

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
#endif
#else
  /**Sends the command to the filter wheel via interface
    @param c the command to be sent
    @param rb is passed as 1 if buffer needs to be read back from tube after sending command, else 0 is passed
    @returns buffer read from the controller.
  */
   char* send_command_to_fw(char* c, int rb);


    /**total number of interfaces for the filter wheels
      */
     static int NumFW_Interfaces;
#endif

     /**total number of interfaces for the motors
      */
     static int NumMotorcontroller_Interfaces;
 private:

 /**serial port of the object. For eg. '/dev/ttyUSB0'
 */
  char serial[200];

 /**serial port feed number returned when initializing/opening the port.
 */
  int serialfd;

 /**structure needed for serial port conf
 */
  struct termios new_serial_conf;

};
