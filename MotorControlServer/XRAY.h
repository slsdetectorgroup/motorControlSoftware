 /********************************************//**
 * @file XRAY.h
 * @short defines the xray tube object
 * @author Dhanya
 ***********************************************/
#ifndef XRAY_H
#define XRAY_H

#ifndef LASERBOX

#include "INTERFACE.h"

/**
 *@short Defines the xray tube object
 */
class XRAY
{
 public:
  /**Constructor
     @param Interface is the pointer to the xray tube serial port
  */
  XRAY(INTERFACE* Interface);

  /**checks if the xray tube is on standby
     @return 1 if it is on standby,-9999 if its switched off, else 0
  */
  int isOnStandby();

  /**switches HV on/off
     @param HVSwitch is 1 to set HV on, else 0
  */		
  void setHVSwitch(int HVSwitch);	
 
  /**gets if the HV is on/off
     @return if the HV is on/off
  */
  int getHVSwitch();

  /**sets the Voltage
     @param Voltage sets the Voltage to this value
  */		
  void setVoltage(int Voltage);	
 
  /**sets the Current
     @param Current sets the Current to this value
  */		
  void setCurrent(int Current);	

  /**sets the voltage and current
     @param Voltage sets the Voltage to this value
     @param Current sets the Current to this value
  */ 
  void setVoltageAndCurrent(int Voltage, int Current);

  /**gets set Voltage value
     @return the set Voltage value
  */
  int getVoltage();

  /**gets actual Voltage value
     @return the actual Voltage value
  */
  int getVoltageActual();

  /**gets set Current value
     @return the set Current value
  */
  int getCurrent();

  /**gets actual Current value
     @return the actual Current value
  */
  int getCurrentActual();

  /**gets the set voltage and current value
     @param voltage gets the voltage value to this parameter
     @param current gets the current value to this parameter
  */
  void getVoltageAndCurrent(int &voltage, int &current);

  /**gets the actual voltage and current value
     @param voltage gets the actual voltage value to this parameter
     @param current gets the actual current value to this parameter
  */
  void getVoltageAndCurrentActual(int &voltage, int &current);

  /**opens/closes the  any shutter
     @param Shutter the shutter to be switched on/off
     @param on is true to be switched on, else false
  */		
  void setShutter(int Shutter, bool on);	
 
  /**gets if the Shutter1 is open/close
     returns 1/0 if the Shutter1 is open/close
  */
  int getShutter1();
 
  /**gets if the Shutter2 is open/close
     returns 1/0 if the Shutter2 is open/close
  */
  int getShutter2();

  /**gets if the Shutter3 is open/close
     returns 1/0 if the Shutter3 is open/close
  */
  int getShutter3();

  /**gets if the Shutter4 is open/close
     returns 1/0 if the Shutter4 is open/close
  */
  int getShutter4();

  /**gets is all the shutters are open/close
     @param s1 if the Shutter1 is open/close
     @param s2 if the Shutter2 is open/close
     @param s3 if the Shutter3 is open/close
     @param s4 if the Shutter4 is open/close
  */
  void getShutters(int &s1,int &s2,int &s3,int &s4);

  /**for debugging: prints all its parameters
   */
  void print();

  /**starts warmup of the xray tube
     @param warmupVoltage xray tube warms up to this value
  */
  void startWarmup(int warmupVoltage);

  /**returns the warm up time remaining
     returns the warm up time remaining
  */
  int getWarmupTimeRemaining();

  /**checks if its safe to open the door
     returns true if its safe to open door
  */
  bool isAccessPossible();

  /**reads the error code for sr:12
     returns the error code 
  */
  int getErrorCode();

  /**reads the error message
     returns the error message 
  */
  char* getErrorMessage();


  /**acknowledges the error and clears the error code
  */
  void clearErrorCode();

 private:

  //none of these parameters are being read!

  /**Interface of xray tube
   */
  INTERFACE* Interface;

  /**temporary command to send to Interface
   */
  char message[200];

  /**temporary ptr to rxd value
   */
  int value;

}; 
#endif
#endif

