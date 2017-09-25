 /********************************************//**
 * @file PRESSURE.h
 * @short defines the pressure object
 * @author Dhanya
 ***********************************************/
#ifndef PRESSURE_H
#define PRESSURE_H

#ifdef VACUUMBOX

#include "INTERFACE.h"

/**
 *@short Defines the pressure object
 */
class PRESSURE
{
 public:
  /**Constructor
     @param Interface is the pointer to the pressure serial port
  */
  PRESSURE(INTERFACE* Interface);

 private:

  //none of these parameters are being read!

  /**Interface of pressure object
   */
  INTERFACE* Interface;

}; 
#endif
#endif

