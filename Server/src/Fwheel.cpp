 /********************************************//**
 * @file Fwheel.cpp
 * @short Defines the Fwheel objects
 * @author Dhanya
 ***********************************************/
#include "Fwheel.h"
#ifdef LASERBOX

#include <iostream>
#include <sstream>
#include <cstring>
using namespace std;


int Fwheel::NumFwheels(0);

Fwheel::Fwheel(char* Name, char* Serialno, vector<double> ValueList, Interface* interface)
{
  /* Fwheel parameters are set according to arguments*/
  strcpy(this->Name,Name);
  strcpy(this->Serialno,Serialno);
  this->interface=interface;

  ValueList.resize(NumSlotsInWheel);
  this->ValueList=ValueList;

  //assign the first value
  Value=ValueList[0];

  //Fwheel::NumFwheels++;
}

char* Fwheel::getName()
{
	return Name;
}

char* Fwheel::getSerialno()
{
	return Serialno;
}

Interface* Fwheel::getInterface()
{
  return interface;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void Fwheel::setStartPosition()
{
#ifdef MOTOR_VERBOSE
		cout<<"Sending command : pos=1"<<endl;
#endif
	//start position
	interface->send_command_to_fw((char*)"pos=1",0);
	Value=ValueList[0];
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

double Fwheel::getValue()
{
	return Value;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

int Fwheel::setValue(double Value)
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
			interface->send_command_to_fw(command,0);
			this->Value = Value;
			return 1;
		}
	}

	//did not find the value in the list
	return 0;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void Fwheel::print()
{
  /* prints all the characteristics of the Fwheel object */
  cout<<"\n\nName of Fwheel\t:\t"<<Name;
  cout<<"\nSerialno\t:\t"<<Serialno;
  cout<<"\nValue\t\t:\t"<<Value;
  cout<<"\nInterface\t:\t"<<interface->getSerial();
  for(int i=0;i<NumSlotsInWheel;i++)
  	  cout<<"\n\tValue["<<i<<"]:\t"<<ValueList[i];
  cout<<"\n";
}

//-------------------------------------------------------------------------------------------------------------------------------------------------
#endif
