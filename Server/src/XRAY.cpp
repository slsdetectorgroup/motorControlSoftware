 /********************************************//**
 * @file XRAY.cpp
 * @short defines the xray tube object
 * @author Dhanya
 ***********************************************/ 
#include "XRAY.h"
#include <iostream>
#include <stdio.h>
#include <cstring>
using namespace std;

#ifndef LASERBOX
XRAY::XRAY(INTERFACE* Interface)
{
  this->Interface=Interface;
}

//------------------------------------------------------------------------------------------------------------------------------------------------

int XRAY::isOnStandby()
{
  //-9999 = switched off and 76 = standby
  Interface->send_command_to_tube((char*)"sr:12 ",1,value,value);
  cout<<"value=:"<<value<<endl;
  if(value==-9999)
	  Interface->send_command_to_tube((char*)"sr:12 ",1,value,value);
  if(value==-9999)
    return -9999;
  if(value==76)
    return 1;
  return 0;
}

//------------------------------------------------------------------------------------------------------------------------------------------------

void XRAY::setHVSwitch(int HVSwitch)	
{
  sprintf(message,"hv:%d ",HVSwitch);
  Interface->send_command_to_tube(message,0,value,value);
}

//------------------------------------------------------------------------------------------------------------------------------------------------

int XRAY::getHVSwitch()
{
  //doing this twice cuz it takes time for
  //hv to get set if u read right after
  strcpy(message, Interface->send_command_to_tube((char*)"sr:1 ",1,value,value));
  strcpy(message, Interface->send_command_to_tube((char*)"sr:1 ",1,value,value));
  return (message[1]-48);
}

//------------------------------------------------------------------------------------------------------------------------------------------------

void XRAY::setVoltage(int Voltage)
{
  sprintf(message,"sv:%d ",Voltage);
  Interface->send_command_to_tube(message,0,value,value);
}


//------------------------------------------------------------------------------------------------------------------------------------------------

void XRAY::setCurrent(int Current)	
{
  sprintf(message,"sc:%d ",Current);
  Interface->send_command_to_tube(message,0,value,value);
}

//------------------------------------------------------------------------------------------------------------------------------------------------

void XRAY::setVoltageAndCurrent(int Voltage, int Current)	
{
  sprintf(message,"sn:%d,%d ",Voltage,Current);
  Interface->send_command_to_tube(message,0,value,value);
}


//------------------------------------------------------------------------------------------------------------------------------------------------
 
int XRAY::getVoltage()
{
  Interface->send_command_to_tube((char*)"vn ",1,value,value);
  if(value==-9999)
	  Interface->send_command_to_tube((char*)"vn ",1,value,value);
  return value;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
 
int XRAY::getVoltageActual()
{
  Interface->send_command_to_tube((char*)"va ",1,value,value);
  if(value==-9999)
	  Interface->send_command_to_tube((char*)"va ",1,value,value);
  return value;
}

//------------------------------------------------------------------------------------------------------------------------------------------------

int XRAY::getCurrent()
{
  Interface->send_command_to_tube((char*)"cn ",1,value,value);
  if(value==-9999)
	  Interface->send_command_to_tube((char*)"cn ",1,value,value);
  return value;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
 
int XRAY::getCurrentActual()
{
  Interface->send_command_to_tube((char*)"ca ",1,value,value);
  if(value==-9999)
	  Interface->send_command_to_tube((char*)"ca ",1,value,value);
  return value;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
 
void XRAY::getVoltageAndCurrent(int &voltage, int &current)
{
  Interface->send_command_to_tube((char*)"gn ",1,voltage,current);
  if(voltage==-9999)
	  Interface->send_command_to_tube((char*)"gn ",1,voltage,current);
}

//------------------------------------------------------------------------------------------------------------------------------------------------
 
void XRAY::getVoltageAndCurrentActual(int &voltage, int &current)
{
  Interface->send_command_to_tube((char*)"ga ",1,voltage,current);
  if(voltage==-9999)
	  Interface->send_command_to_tube((char*)"gn ",1,voltage,current);
}
//------------------------------------------------------------------------------------------------------------------------------------------------

void XRAY::setShutter(int Shutter, bool on)	
{
  cout<<"setting shutters on/off";

  if(on) sprintf(message,"os:");
  else sprintf(message,"cs:");

  char temp[20];
  sprintf(temp,"%d",Shutter);
  strcat(message,temp);
  strcat(message," ");
  Interface->send_command_to_tube(message,0,value,value);
}
 
//------------------------------------------------------------------------------------------------------------------------------------------------

int XRAY::getShutter1()
{
  strcpy(message, Interface->send_command_to_tube((char*)"sr:3 ",1,value,value));
  if(value==-9999)
	  strcpy(message, Interface->send_command_to_tube((char*)"sr:3 ",1,value,value));
 return (message[0]-48);
}

//------------------------------------------------------------------------------------------------------------------------------------------------

int XRAY::getShutter2()
{
  strcpy(message, Interface->send_command_to_tube((char*)"sr:3 ",1,value,value));
  if(value==-9999)
	  strcpy(message, Interface->send_command_to_tube((char*)"sr:3 ",1,value,value));
  return (message[4]-48);
}

//------------------------------------------------------------------------------------------------------------------------------------------------

int XRAY::getShutter3()
{
  strcpy(message, Interface->send_command_to_tube((char*)"sr:4 ",1,value,value));
  if(value==-9999)
	  strcpy(message, Interface->send_command_to_tube((char*)"sr:4 ",1,value,value));
  return (message[0]-48);
}

//------------------------------------------------------------------------------------------------------------------------------------------------

int XRAY::getShutter4()
{
  strcpy(message, Interface->send_command_to_tube((char*)"sr:4 ",1,value,value));
  if(value==-9999)
	  strcpy(message, Interface->send_command_to_tube((char*)"sr:4 ",1,value,value));
  return (message[4]-48);
}

//------------------------------------------------------------------------------------------------------------------------------------------------

void XRAY::getShutters(int &s1,int &s2,int &s3,int &s4)
{
  strcpy(message, Interface->send_command_to_tube((char*)"sr:3 ",1,value,value));
  if(value==-9999)
	  strcpy(message, Interface->send_command_to_tube((char*)"sr:3 ",1,value,value));
  s1=(message[0]-48); s2=(message[4]-48);
  strcpy(message, Interface->send_command_to_tube((char*)"sr:4 ",1,value,value));
  if(value==-9999)
	  strcpy(message, Interface->send_command_to_tube((char*)"sr:4 ",1,value,value));
 s3=(message[0]-48); s4= (message[4]-48);
}

//------------------------------------------------------------------------------------------------------------------------------------------------

void XRAY::print()
{
  /* prints all the characteristics of the xray object */
  cout<<"High Voltage\t:\t";if(getHVSwitch()) cout<<"ON"; else cout<<"OFF";
  cout<<"\nVoltage\t\t:\t"<<getVoltage();
  cout<<"\nCurrent\t\t:\t"<<getCurrent();
  cout<<"\nShutter 1\t:\t";if(getShutter1()) cout<<"ON"; else cout<<"OFF";
  cout<<"\nShutter 2\t:\t";if(getShutter2()) cout<<"ON"; else cout<<"OFF";
  cout<<"\nShutter 3\t:\t";if(getShutter3()) cout<<"ON"; else cout<<"OFF";
  cout<<"\nShutter 4\t:\t";if(getShutter4()) cout<<"ON"; else cout<<"OFF";
  cout<<"\nInterface\t:\t"<<Interface->getSerial();
  cout<<"\n";	
}

//------------------------------------------------------------------------------------------------------------------------------------------------

void XRAY::startWarmup(int warmupVoltage)
{
  cout<<"\ninside the startwarmup() in XRAY.cpp\n";
  int s1,s2,s3,s4;
  getShutters(s1,s2,s3,s4);
  if(s1) Interface->send_command_to_tube((char*)"cs:1 ",0,value,value);
  if(s2) Interface->send_command_to_tube((char*)"cs:2 ",0,value,value);
  if(s3) Interface->send_command_to_tube((char*)"cs:3 ",0,value,value);
  if(s4) Interface->send_command_to_tube((char*)"cs:4 ",0,value,value);
  sprintf(message,"wu:4,%d ",warmupVoltage);
   Interface->send_command_to_tube(message,0,value,value);
}

//------------------------------------------------------------------------------------------------------------------------------------------------

int XRAY::getWarmupTimeRemaining()
{
  //again wt takes time/or more commands sent to show the right value
  strcpy(message, Interface->send_command_to_tube((char*)"sr:6 ",1,value,value));
  strcpy(message, Interface->send_command_to_tube((char*)"sr:6 ",1,value,value));
  if(value==-9999){
	  strcpy(message, Interface->send_command_to_tube((char*)"sr:6 ",1,value,value));
	  strcpy(message, Interface->send_command_to_tube((char*)"sr:6 ",1,value,value));
  }
  strcpy(message, Interface->send_command_to_tube((char*)"wt ",1,value,value));
  strcpy(message, Interface->send_command_to_tube((char*)"wt ",1,value,value));
  if(value==-9999){
	  strcpy(message, Interface->send_command_to_tube((char*)"wt ",1,value,value));
	  strcpy(message, Interface->send_command_to_tube((char*)"wt ",1,value,value));
  }
  return value;
}

//------------------------------------------------------------------------------------------------------------------------------------------------

bool XRAY::isAccessPossible()
{
  //if either of the shutters AND the HV is on
  if( (getShutter1()||getShutter2()||getShutter3()||getShutter4())&&(getHVSwitch()) )
    return false;
  return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------

int XRAY::getErrorCode()
{
  Interface->send_command_to_tube((char*)"sr:12 ",1,value,value);
  Interface->send_command_to_tube((char*)"sr:12 ",1,value,value);
  return value;
}

//------------------------------------------------------------------------------------------------------------------------------------------------

char* XRAY::getErrorMessage()
{
  strcpy(message, Interface->send_command_to_tube((char*)"er ",1,value,value));
  strcpy(message, Interface->send_command_to_tube((char*)"er ",1,value,value));
  return message;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
void XRAY::clearErrorCode()
{
  Interface->send_command_to_tube((char*)"cl ",0,value,value);
}

//------------------------------------------------------------------------------------------------------------------------------------------------
#endif
