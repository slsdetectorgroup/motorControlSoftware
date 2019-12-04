#pragma once


#include "Interface.h"

/**
  *@short Defines the motor object parameters and their methods.
  */
class Motor
{
 public:

  /**Constructor
	 @param name name of the motor
	 @param axis The axis of the controller that the motor's connected to
	 @param controller The controller connected to motor, eg. corvus_1
	 @param speed The speed the motor is set to
	 @param lowerLimit The lower limit of the motor
	 @param upperLimit The upper limit of the motor
	 @param interface Pointer to an interface object.<br>(The port that the  motor's connected to is accessed through this)
  */
  Motor(char* name, int axis, char* controller, double speed, double lowerLimit, double upperLimit, Interface* interface);

 /**sets the name of motor.
	@param name sets name of motor to this value
 */			
  void setName(char* name);	

 /**gets the name of the motor.
   @returns the name of motor
 */ 
  char* getName();

 /**sets the axis of motor.
	@param axis sets axis of motor to this value
 */		
  void setAxis(int axis);	

 /**gets the axis of the motor.
   @returns the axis of motor
 */  
  int getAxis();

 /**sets the controller of the motor.
	@param controller sets controller of the motor to this value
 */		
  void setController(char* controller);	
 
 /**gets the controller of the motor.
   @returns the controller of the motor
 */
  char* getController();

 /**sets the speed of motor.
	@param speed sets speed of motor to this value
 */		
  void setSpeed(double speed);	

 /**gets the speed of the motor.
   @returns the speed of motor
 */
  double getSpeed();

 /**sets the Lower Limit of motor.
	@param lowerLimit sets  Lower Limit of motor to this value
 */		
  void setLowerLimit(double lowerLimit);	

 /**gets the Lower Limit of the motor.
   @returns the Lower Limit of motor
 */
  double getLowerLimit();

 /**sets the Upper Limit of motor.
	@param upperLimit sets Upper Limit of motor to this value
 */		
  void setUpperLimit(double upperLimit);	

 /**gets the Upper Limit of the motor.
   @returns the Upper Limit of motor
 */
  double getUpperLimit();

/**sets the Position of motor.
       @param Position sets Position of motor to this value
 */            
  void setPosition(double position);  

 /**gets the position of the motor.
   @returns the position of motor
 */
  double getPosition();

 /**sets the Interface of motor.
	@param interface sets Interface of motor to point to this value
 */		
  void setInterface(Interface* interface);

 /**gets the Interface of the motor.
   @returns the Interface of motor
 */
  Interface* getInterface();

 /*for debugging - Prints out the parameters of a motor object: name, axis, controller connected to, speed, lowerLimit, upperLimit, Interface.
 */		
  void print();

 /*for debugging - gets the accurate position of motor by sending the 'pos ' cmd to controller.
   @returns accurate position returned by controller
 */
  double debugPosition();

 /*checks if the motor movement is within its limits
   @param the absolute position the motor is about to move to
   @returns true if it can move
 */
  bool canMotorMove(double position);

 /**moves the motor to a relative position.
	@param position relative position
	@param otherAxis axis of second motor for simultaneous movement. 0 if not required.
	@param otherPosition relative position of second motor for simultaneous movement. ignored if otherAxis is 0.
 */
  void moveRel(double position, int otherAxis, double otherPosition);

 /**moves the motor to an absolute position.
	@param position absolute position
	@param otherAxis axis of second motor for simultaneous movement. 0 if not required.
	@param otherPosition absolute position of second motor for simultaneous movement. ignored if otherAxis is 0.
	@param otherOrigPosition the original position of second motor for simultaneous movt. Required to cal relative movt.
 */
  void moveAbs(double position, int otherAxis, double otherPosition, double otherOrigPosition);

 /**moves all the motors of the controller to an absolute position.
	@param position1 the absolute position of axis 1 of controller
	@param position2 the absolute position of axis 2 of controller 
	@param position3 the absolute position of axis 3 of controller
 */
  void moveAllAbs(double position1,double position2, double position3);

 /**moves the motor to its lower limit switch and sets that value to zero
 */
  void calibrate();


  /**total number of motor object
   */		
  static int NumMotors;

#ifndef LASERBOX
  /**the cal speed of huber
   */
  const static double huberCalSpeed = 0.23;

  /**the default speed of the controller huber is connected to
   */
  static double defaultSpeed;
#endif
		
 private:

/**name of motor
 */
  char name[200];

/**axis of motor
 */
  int axis;

/**controller connected to motor
 */
  char controller[200];

/**speed of motor
 */
  double speed;

/**Lower limit of motor
 */
  double lowerLimit;

/**Upper limit of motor
 */
  double upperLimit;

/**position of motor
 */
  double position;

/**Interface pointer of motor
 */
  Interface* interface;


}; 

