 /********************************************//**
 * @file Motor.cpp
 * @short Defines the motor objects
 * @author Dhanya
 ***********************************************/
#include "Motor.h"

#include <iostream>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <stdio.h>
using namespace std;


int Motor::NumMotors(0);

#ifndef LASERBOX
double Motor::defaultSpeed(30);
#endif

Motor::Motor(char* name, int axis, char* controller, double speed, double lowerLimit, double upperLimit, Interface* interface):axis(axis),speed(speed),lowerLimit(lowerLimit),upperLimit(upperLimit),position(0.0)
{
  /* motor parameters are set according to arguments*/
  strcpy(this->name,name);
  strcpy(this->controller,controller);
  this->interface=interface;
  debugPosition();
}

void Motor::setName(char* name)
{
	strcpy(this->name,name);
}

char* Motor::getName()
{
	return name;
}

void Motor::setAxis(int axis)
{
	this->axis = axis;
}

int Motor::getAxis()
{
	return axis;
}

void Motor::setController(char* controller)
{
	strcpy(this->controller,controller);
}

char* Motor::getController()
{
	return controller;
}
	
void Motor::setSpeed(double speed)
{
	this->speed = speed;
}

double Motor::getSpeed()
{
	return speed;
}

void Motor::setLowerLimit(double lowerLimit)
{
	this->lowerLimit = lowerLimit;
}

double Motor::getLowerLimit()
{
	return lowerLimit;
}

void Motor::setUpperLimit(double upperLimit)
{
	this->upperLimit = upperLimit;
}

double Motor::getUpperLimit()
{
	return upperLimit;
}

void Motor::setPosition(double position)
{
  this->position = position;
}

double Motor::getPosition()
{
	return position;
}

void Motor::setInterface(Interface* interface)
{
  this->interface=interface;
}

Interface* Motor::getInterface()
{
  return interface;
}
//-------------------------------------------------------------------------------------------------------------------------------------------------

void Motor::print()
{
  /* prints all the characteristics of the motor object */
  cout<<"Name of Motor\t:\t"<<name;
  cout<<"\nAxis\t\t:\t"<<axis;
  cout<<"\nController\t:\t"<<controller;
  cout<<"\nPosition\t:\t"<<position;
  cout<<"\nSpeed\t\t:\t"<<speed;
  cout<<"\nLowerLimit\t:\t"<<lowerLimit;
  cout<<"\nUpperLimit\t:\t"<<upperLimit;
  cout<<"\nInterface\t:\t"<<interface->getSerial();
  cout<<"\n";
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

double Motor::debugPosition()
{
  char buffer[255]="";
  int i=0;

  /* creating an array to save the positions of all motors of controller */
  double args[3];                                         

  /* sends command 'pos ' to controller */
  strcpy(buffer,interface->send_command((char*)"pos ",1));
  istringstream sstr(buffer);

  /* separates pos values from all the axis of controller returned and white spaces */
  while(sstr.good())                                      
  	{
	  sstr>>buffer;
	  args[i]=atof(buffer);
	  i++;
   	}   
  
  /* returns only the pos of the motor*/
  position = args[axis-1];
  return position;
}


//-------------------------------------------------------------------------------------------------------------------------------------------------


bool Motor::canMotorMove(double position)
{
  // if by any chance the upper and lower limit is -1.. it doesnt really check
  if((upperLimit==-1)&& (lowerLimit==-1))
	return true;
  if((position<=upperLimit) && (position>=lowerLimit))
	return true;
  
  return false;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------


void Motor::moveRel(double position, int otherAxis, double otherPosition)
{
  /* create the command using string concatenation */
  char command[200]="";

  /* list of positions for all 3 axis in controller */
  char args[3][100]={"0.000 ","0.000 ","0.000 "};
 
  /* converting the position to a char*, adding space after it, copied to args[axis-1]th element */
  char pos[200]="";                         
  sprintf(pos,"%f",position);
  strcat(pos," ");
  strcpy(args[axis-1],pos);

  /* for simultaneous movement, use otherPosition the same way*/
  if(otherAxis>0)
	{
	  sprintf(pos,"%f",otherPosition);
	  strcat(pos," ");
	  strcpy(args[otherAxis-1],pos);
	}

  /* concatenating all the positions  and the "r " to make the command */
  for(int i=0;i<3;i++)                     
	strcat(command,args[i]);
  strcat(command,"r ");

#ifndef LASERBOX
  /* if motor is not huber   */
    if(strcasecmp(name,"Huber"))
#endif
	  interface->send_command(command,0);   
#ifndef LASERBOX
  else
	{
	  //change speed to hubers
	  char changeVel[200];
	  sprintf(changeVel,"%f setvel ",speed);
	  cout<<"motor class:"<<changeVel<<endl;
	  interface->send_command(changeVel,0);

	  interface->send_command(command,0); 

	  //change speed back
	  sprintf(changeVel,"%f setvel ",defaultSpeed);
	  cout<<"motor class:"<<changeVel<<endl; 
	  interface->send_command(changeVel,0);

	}  
#endif
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void Motor::moveAbs(double position, int otherAxis, double otherPosition, double otherOrigPosition)
{
#ifdef VERBOSE_MOTOR
  cout<<"move motor of axis"<<axis<<"from "<<position<<" to "<<position<<endl<<endl;
#endif
  /* create the command using string concatenation */
  char command[200]="";

  /* list of positions for all 3 axis in controlleor */
  char args[3][100]={"0.000 ","0.000 ","0.000 "};
  
  /*converting the absolute position to a char*, 
	 adding space after it, copied to args[axis-1]th element 
  */                
  char pos[200]="";       
  sprintf(pos,"%f",position-position);
  strcat(pos," ");
  strcpy(args[axis-1],pos);

  /* for simultaneous movement  */
  if(otherAxis>0)
	{
#ifdef VERBOSE_MOTOR
	   cout<<"move motor of axis"<<otherAxis<<"from "<<otherOrigPosition<<" to "<<otherPosition<<endl<<endl;
#endif
	   /* repeat with other axis */  
	  sprintf(pos,"%f",otherPosition-otherOrigPosition);
	  strcat(pos," ");
	  strcpy(args[otherAxis-1],pos);
	}

  /* concatenating all the positions  and the "m " to make the command */
  for(int i=0;i<3;i++)                                      
	strcat(command,args[i]);
  strcat(command,"r ");

#ifndef LASERBOX
  /* if motor is not huber   */
  if(strcasecmp(name,"Huber"))
#endif
	interface->send_command(command,0);
#ifndef LASERBOX
  else
	{
	  //change speed to hubers
	  char changeVel[200];
	  sprintf(changeVel,"%f setvel ",speed);
	  cout<<"in motor class:"<<changeVel<<endl;
	  interface->send_command(changeVel,0);

	  interface->send_command(command,0); 

	  //change speed back
	  sprintf(changeVel,"%f setvel ",defaultSpeed);
	  cout<<"in motor class:"<<changeVel<<endl; 
	  interface->send_command(changeVel,0);
	}  
#endif

  //confirming the move is over
  //while(debugPosition()!=position)
  double temp = debugPosition()-position;
  if(temp>0)
    {
      while(temp>0.0001)
	{
	  cout<<"Target position:"<<position<<"; At position "<<debugPosition()<<" right now. Moving..\n";
	  temp = debugPosition()-position;
	}
    }
  else
    {
      while(-temp>0.0001)
	{
	  cout<<"Target position:"<<position<<"; At position "<<debugPosition()<<" right now. Moving..\n";
	  temp = debugPosition()-position;
	}
    }
 
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void Motor::moveAllAbs(double position1,double position2, double position3)
{
#ifdef VERBOSE_MOTOR
  cout<<"move all the motors of controller "<<controller<<" to positions"<<position1<<", "<<position2<<" and "<<position3<<endl;
#endif

  /* create the command using string concatenation */
  char command[200]="";

  /*making the command to send to the controller
  */                
  char pos1[200]="", pos2[200]="", pos3[200]="";   
  sprintf(pos1,"%f",position1);
  sprintf(pos2,"%f",position2);
  sprintf(pos3,"%f",position3);

  sprintf(command,"%s %s %s m ",pos1,pos2,pos3);

   /* sending the command to move with readback/rb=0 */
  interface->send_command(command,0);  
  
  //confirming the move is over
  char buffer[255]="";
  int i=0;
  double args[3];                                         
 
  //need to keep checking until all the positions are the same
  while((args[0]!=position1)||(args[1]!=position2)||(args[2]!=position3))
	{
	  strcpy(buffer,interface->send_command((char*)"pos ",1));
	  istringstream sstr(buffer);

	  // separates pos values from all the axis of controller returned and white spaces 
	  while(sstr.good())                                      
		{
		  sstr>>buffer;
		  args[i]=atof(buffer);
		  i++;
		}   
	}

}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void Motor::calibrate()
{
  char command[200]="";
  sprintf(command,"%d",axis);
  strcat(command," ncal ");
  interface->send_command(command,0);

  //keep checking till the motor has moved to its position 0
  while(debugPosition()!=0) ;

}

//-------------------------------------------------------------------------------------------------------------------------------------------------
