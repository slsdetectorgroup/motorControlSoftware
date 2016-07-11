 /********************************************//**
 * @file MOTOR.h
 * @short Defines the motor objects
 * @author Dhanya
 ***********************************************/
#ifndef MOTOR_H
#define MOTOR_H


#include "INTERFACE.h"

#ifndef LASERBOX
#include "SLIT.h"
#endif

/**
  *@short Defines the motor object parameters and their methods.
  */
class MOTOR
{
 public:

  /**Constructor
	 @param Name Name of the motor
	 @param Axis The axis of the controller that the motor's connected to
	 @param Controller The controller connected to motor, eg. corvus_1
	 @param Speed The speed the motor is set to
	 @param LowerLimit The lower limit of the motor
	 @param UpperLimit The upper limit of the motor
	 @param Position The position of the motor
	 @param Interface Pointer to an interface object.<br>(The port that the  motor's connected to is accessed through this)
  */
  MOTOR(char* Name, int Axis, char* Controller, double Speed, double LowerLimit, double UpperLimit, double Position, INTERFACE* Interface);

 /**sets the name of motor.
	@param Name sets name of motor to this value
 */			
  void setName(char* Name);	

 /**gets the name of the motor.
   @returns the name of motor
 */ 
  char* getName();

 /**sets the Axis of motor.
	@param Axis sets Axis of motor to this value
 */		
  void setAxis(int Axis);	

 /**gets the Axis of the motor.
   @returns the Axis of motor
 */  
  int getAxis();

 /**sets the Controller of the motor.
	@param Controller sets Controller of the motor to this value
 */		
  void setController(char* Controller);	
 
 /**gets the Controller of the motor.
   @returns the Controller of the motor
 */
  char* getController();

 /**sets the Speed of motor.
	@param Speed sets Speed of motor to this value
 */		
  void setSpeed(double Speed);	

 /**gets the Speed of the motor.
   @returns the Speed of motor
 */
  double getSpeed();

 /**sets the Lower Limit of motor.
	@param LowerLimit sets  Lower Limit of motor to this value
 */		
  void setLowerLimit(double LowerLimit);	

 /**gets the Lower Limit of the motor.
   @returns the Lower Limit of motor
 */
  double getLowerLimit();

 /**sets the Upper Limit of motor.
	@param UpperLimit sets Upper Limit of motor to this value
 */		
  void setUpperLimit(double UpperLimit);	

 /**gets the Upper Limit of the motor.
   @returns the Upper Limit of motor
 */
  double getUpperLimit();

/**sets the Position of motor.
	@param Position sets Position of motor to this value
 */		
  void setPosition(double Position);	

 /**gets the Position of the motor.
   @returns the Position of motor
 */
  double getPosition();

 /**sets the Interface of motor.
	@param Interface sets Interface of motor to point to this value
 */		
  void setInterface(INTERFACE* Interface);

 /**gets the Interface of the motor.
   @returns the Interface of motor
 */
  INTERFACE* getInterface();

 /*for debugging - Prints out the parameters of a motor object: Name, Axis, Controller connected to, Speed, LowerLimit, UpperLimit, Interface.
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
  bool canMotorMove(double Position);

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

/**Name of motor
 */
  char Name[200];

/**Axis of motor
 */
  int Axis;

/**Controller connected to motor
 */
  char Controller[200];

/**Speed of motor
 */
  double Speed;

/**Lower limit of motor
 */
  double LowerLimit;

/**Upper limit of motor
 */
  double UpperLimit;

/**Position of motor
 */
  double Position;

/**Interface pointer of motor
 */
  INTERFACE* Interface;


}; 
#endif
