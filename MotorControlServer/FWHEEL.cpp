 /********************************************//**
 * @file FWHEEL.cpp
 * @short Defines the FWHEEL objects
 * @author Dhanya
 ***********************************************/
#include "FWHEEL.h"
#ifdef LASERBOX

#include <iostream>
#include <sstream>
#include <cstring>
using namespace std;


int FWHEEL::NumFwheels(0);

FWHEEL::FWHEEL(char* Name, char* Serialno, vector<double> ValueList, INTERFACE* Interface)
{
  /* FWHEEL parameters are set according to arguments*/
  strcpy(this->Name,Name);
  strcpy(this->Serialno,Serialno);
  this->Interface=Interface;

  ValueList.resize(NumSlotsInWheel);
  this->ValueList=ValueList;

  //assign the first value
  Value=ValueList[0];

  //FWHEEL::NumFwheels++;
}

char* FWHEEL::getName()
{
	return Name;
}

char* FWHEEL::getSerialno()
{
	return Serialno;
}

INTERFACE* FWHEEL::getInterface()
{
  return Interface;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void FWHEEL::setStartPosition()
{
#ifdef MOTOR_VERBOSE
		cout<<"Sending command : pos=1"<<endl;
#endif
	//start position
	Interface->send_command_to_fw((char*)"pos=1",0);
	Value=ValueList[0];
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

double FWHEEL::getValue()
{
	return Value;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

int FWHEEL::setValue(double Value)
{
	char command[200]="pos=X";
	for(int i=0;i<NumSlotsInWheel;i++)
	{
		if(ValueList[i]==Value)
		{
			//+1 because positions for filter wheel in display are from 1 to 6
			command[4]=i+48+1;
#ifdef MOTOR_VERBOSE
		cout<<"Sending command "<<command<<endl;
#endif
			Interface->send_command_to_fw(command,0);
			this->Value = Value;
			return 1;
		}
	}

	//did not find the value in the list
	return 0;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void FWHEEL::print()
{
  /* prints all the characteristics of the FWHEEL object */
  cout<<"\n\nName of FWHEEL\t:\t"<<Name;
  cout<<"\nSerialno\t:\t"<<Serialno;
  cout<<"\nValue\t\t:\t"<<Value;
  cout<<"\nInterface\t:\t"<<Interface->getSerial();
  for(int i=0;i<NumSlotsInWheel;i++)
  	  cout<<"\n\tValue["<<i<<"]:\t"<<ValueList[i];
  cout<<"\n";
}

//-------------------------------------------------------------------------------------------------------------------------------------------------
#endif
