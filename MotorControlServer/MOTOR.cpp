 /********************************************//**
 * @file MOTOR.cpp
 * @short Defines the motor objects
 * @author Dhanya
 ***********************************************/
#include "MOTOR.h"

#include <iostream>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <stdio.h>
using namespace std;


int MOTOR::NumMotors(0);

#ifndef LASERBOX
double MOTOR::defaultSpeed(30);
#endif

MOTOR::MOTOR(char* Name, int Axis, char* Controller, double Speed, double LowerLimit, double UpperLimit, double Position, INTERFACE* Interface):Axis(Axis),Speed(Speed),LowerLimit(LowerLimit),UpperLimit(UpperLimit),Position(Position)
{
  /* motor parameters are set according to arguments*/
  strcpy(this->Name,Name);
  strcpy(this->Controller,Controller);
  this->Interface=Interface;
}

void MOTOR::setName(char* Name)
{
	strcpy(this->Name,Name);
}

char* MOTOR::getName()
{
	return Name;
}

void MOTOR::setAxis(int Axis)
{
	this->Axis = Axis;
}

int MOTOR::getAxis()
{
	return Axis;
}

void MOTOR::setController(char* Controller)
{
	strcpy(this->Controller,Controller);
}

char* MOTOR::getController()
{
	return Controller;
}
	
void MOTOR::setSpeed(double Speed)
{
	this->Speed = Speed;
}

double MOTOR::getSpeed()
{
	return Speed;
}

void MOTOR::setLowerLimit(double LowerLimit)
{
	this->LowerLimit = LowerLimit;
}

double MOTOR::getLowerLimit()
{
	return LowerLimit;
}

void MOTOR::setUpperLimit(double UpperLimit)
{
	this->UpperLimit = UpperLimit;
}

double MOTOR::getUpperLimit()
{
	return UpperLimit;
}

void MOTOR::setPosition(double Position)
{
	this->Position = Position;
}

double MOTOR::getPosition()
{
	return Position;
}

void MOTOR::setInterface(INTERFACE* Interface)
{
  this->Interface=Interface;
}

INTERFACE* MOTOR::getInterface()
{
  return Interface;
}
//-------------------------------------------------------------------------------------------------------------------------------------------------

void MOTOR::print()
{
  /* prints all the characteristics of the motor object */
  cout<<"\n\nName of Motor\t:\t"<<Name;
  cout<<"\nAxis\t\t:\t"<<Axis;
  cout<<"\nController\t:\t"<<Controller;
  cout<<"\nPosition\t:\t"<<Position;
  cout<<"\nSpeed\t\t:\t"<<Speed;
  cout<<"\nLowerLimit\t:\t"<<LowerLimit;
  cout<<"\nUpperLimit\t:\t"<<UpperLimit;
  cout<<"\nInterface\t:\t"<<Interface->getSerial();
  cout<<"\n";
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

double MOTOR::debugPosition()
{
  char buffer[255]="";
  int i=0;

  /* creating an array to save the positions of all motors of controller */
  double args[3];                                         

  /* sends command 'pos ' to controller */
  strcpy(buffer,Interface->send_command((char*)"pos ",1));
  istringstream sstr(buffer);

  /* separates pos values from all the axis of controller returned and white spaces */
  while(sstr.good())                                      
  	{
	  sstr>>buffer;
	  args[i]=atof(buffer);
	  i++;
   	}   
  
  /* returns only the pos of the motor*/
  return args[Axis-1];
}


//-------------------------------------------------------------------------------------------------------------------------------------------------

bool MOTOR::canMotorMove(double position)
{
  // if by any chance the upper and lower limit is -1.. it doesnt really check
  if((UpperLimit==-1)&& (LowerLimit==-1))
	return true;
  if((position<=UpperLimit) && (position>=LowerLimit))
	return true;
  
  return false;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------


void MOTOR::moveRel(double position, int otherAxis, double otherPosition)
{
  /* create the command using string concatenation */
  char command[200]="";

  /* list of positions for all 3 axis in controller */
  char args[3][100]={"0.000 ","0.000 ","0.000 "};
 
  /* converting the position to a char*, adding space after it, copied to args[Axis-1]th element */
  char pos[200]="";                         
  sprintf(pos,"%f",position);
  strcat(pos," ");
  strcpy(args[Axis-1],pos);

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
    if(strcasecmp(Name,"Huber"))
#endif
	  Interface->send_command(command,0);   
#ifndef LASERBOX
  else
	{
	  //change speed to hubers
	  char changeVel[200];
	  sprintf(changeVel,"%f setvel ",Speed);
	  cout<<"motor class:"<<changeVel<<endl;
	  Interface->send_command(changeVel,0);

	  Interface->send_command(command,0); 

	  //change speed back
	  sprintf(changeVel,"%f setvel ",defaultSpeed);
	  cout<<"motor class:"<<changeVel<<endl; 
	  Interface->send_command(changeVel,0);

	}  
#endif
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void MOTOR::moveAbs(double position, int otherAxis, double otherPosition, double otherOrigPosition)
{
#ifdef VERBOSE_MOTOR
  cout<<"move motor of Axis"<<Axis<<"from "<<Position<<" to "<<position<<endl<<endl;
#endif
  /* create the command using string concatenation */
  char command[200]="";

  /* list of positions for all 3 axis in controlleor */
  char args[3][100]={"0.000 ","0.000 ","0.000 "};
  
  /*converting the absolute position to a char*, 
	 adding space after it, copied to args[Axis-1]th element 
  */                
  char pos[200]="";       
  sprintf(pos,"%f",position-Position);
  strcat(pos," ");
  strcpy(args[Axis-1],pos);

  /* for simultaneous movement  */
  if(otherAxis>0)
	{
#ifdef VERBOSE_MOTOR
	   cout<<"move motor of Axis"<<otherAxis<<"from "<<otherOrigPosition<<" to "<<otherPosition<<endl<<endl;
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
  if(strcasecmp(Name,"Huber"))
#endif
	Interface->send_command(command,0);
#ifndef LASERBOX
  else
	{
	  //change speed to hubers
	  char changeVel[200];
	  sprintf(changeVel,"%f setvel ",Speed);
	  cout<<"in motor class:"<<changeVel<<endl;
	  Interface->send_command(changeVel,0);

	  Interface->send_command(command,0); 

	  //change speed back
	  sprintf(changeVel,"%f setvel ",defaultSpeed);
	  cout<<"in motor class:"<<changeVel<<endl; 
	  Interface->send_command(changeVel,0);
	}  
#endif

  //confirming the move is over
  //while(debugPosition()!=position)
  double temp = debugPosition()-position;
  if(temp>0)
    {
      while(temp>0.0001)
	{
	  cout<<"Target Position:"<<position<<"; At position "<<debugPosition()<<" right now. Moving..\n";
	  temp = debugPosition()-position;
	}
    }
  else
    {
      while(-temp>0.0001)
	{
	  cout<<"Target Position:"<<position<<"; At position "<<debugPosition()<<" right now. Moving..\n";
	  temp = debugPosition()-position;
	}
    }
 
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void MOTOR::moveAllAbs(double position1,double position2, double position3)
{
#ifdef VERBOSE_MOTOR
  cout<<"move all the motors of controller "<<Controller<<" to positions"<<position1<<", "<<position2<<" and "<<position3<<endl;
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
  Interface->send_command(command,0);  
  
  //confirming the move is over
  char buffer[255]="";
  int i=0;
  double args[3];                                         
 
  //need to keep checking until all the positions are the same
  while((args[0]!=position1)||(args[1]!=position2)||(args[2]!=position3))
	{
	  strcpy(buffer,Interface->send_command((char*)"pos ",1));
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

void MOTOR::calibrate()
{
  char command[200]="";
  sprintf(command,"%d",Axis);
  strcat(command," ncal ");
  Interface->send_command(command,0);

  //keep checking till the motor has moved to its position 0
  while(debugPosition()!=0) ;

}

//-------------------------------------------------------------------------------------------------------------------------------------------------
