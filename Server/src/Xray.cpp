 /********************************************//**
 * @file Xray.cpp
 * @short defines the xray tube object
 * @author Dhanya
 ***********************************************/ 
#include "Xray.h"
#include <iostream>
#include <stdio.h>
#include <cstring>
using namespace std;

#ifndef LASERBOX
Xray::Xray(Interface* interface)
{
  this->interface=interface;
}

//------------------------------------------------------------------------------------------------------------------------------------------------

int Xray::isOnStandby()
{
  //-9999 = switched off and 76 = standby
  interface->send_command_to_tube((char*)"sr:12 ",1,value,value);
  cout<<"value=:"<<value<<endl;
  if(value==-9999)
	  interface->send_command_to_tube((char*)"sr:12 ",1,value,value);
  if(value==-9999)
    return -9999;
  if(value==76)
    return 1;
  return 0;
}

//------------------------------------------------------------------------------------------------------------------------------------------------

void Xray::setHVSwitch(int HVSwitch)	
{
  sprintf(message,"hv:%d ",HVSwitch);
  interface->send_command_to_tube(message,0,value,value);
}

//------------------------------------------------------------------------------------------------------------------------------------------------

int Xray::getHVSwitch()
{
  //doing this twice cuz it takes time for
  //hv to get set if u read right after
  strcpy(message, interface->send_command_to_tube((char*)"sr:1 ",1,value,value));
  strcpy(message, interface->send_command_to_tube((char*)"sr:1 ",1,value,value));
  return (message[1]-48);
}

//------------------------------------------------------------------------------------------------------------------------------------------------

void Xray::setVoltage(int Voltage)
{
  sprintf(message,"sv:%d ",Voltage);
  interface->send_command_to_tube(message,0,value,value);
}


//------------------------------------------------------------------------------------------------------------------------------------------------

void Xray::setCurrent(int Current)	
{
  sprintf(message,"sc:%d ",Current);
  interface->send_command_to_tube(message,0,value,value);
}

//------------------------------------------------------------------------------------------------------------------------------------------------

void Xray::setVoltageAndCurrent(int Voltage, int Current)	
{
  sprintf(message,"sn:%d,%d ",Voltage,Current);
  interface->send_command_to_tube(message,0,value,value);
}


//------------------------------------------------------------------------------------------------------------------------------------------------
 
int Xray::getVoltage()
{
  interface->send_command_to_tube((char*)"vn ",1,value,value);
  if(value==-9999)
	  interface->send_command_to_tube((char*)"vn ",1,value,value);
  return value;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
 
int Xray::getVoltageActual()
{
  interface->send_command_to_tube((char*)"va ",1,value,value);
  if(value==-9999)
	  interface->send_command_to_tube((char*)"va ",1,value,value);
  return value;
}

//------------------------------------------------------------------------------------------------------------------------------------------------

int Xray::getCurrent()
{
  interface->send_command_to_tube((char*)"cn ",1,value,value);
  if(value==-9999)
	  interface->send_command_to_tube((char*)"cn ",1,value,value);
  return value;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
 
int Xray::getCurrentActual()
{
  interface->send_command_to_tube((char*)"ca ",1,value,value);
  if(value==-9999)
	  interface->send_command_to_tube((char*)"ca ",1,value,value);
  return value;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
 
void Xray::getVoltageAndCurrent(int &voltage, int &current)
{
  interface->send_command_to_tube((char*)"gn ",1,voltage,current);
  if(voltage==-9999)
	  interface->send_command_to_tube((char*)"gn ",1,voltage,current);
}

//------------------------------------------------------------------------------------------------------------------------------------------------
 
void Xray::getVoltageAndCurrentActual(int &voltage, int &current)
{
  interface->send_command_to_tube((char*)"ga ",1,voltage,current);
  if(voltage==-9999)
	  interface->send_command_to_tube((char*)"gn ",1,voltage,current);
}
//------------------------------------------------------------------------------------------------------------------------------------------------

void Xray::setShutter(int Shutter, bool on)	
{
  cout<<"setting shutters on/off";

  if(on) sprintf(message,"os:");
  else sprintf(message,"cs:");

  char temp[20];
  sprintf(temp,"%d",Shutter);
  strcat(message,temp);
  strcat(message," ");
  interface->send_command_to_tube(message,0,value,value);
}
 
//------------------------------------------------------------------------------------------------------------------------------------------------

int Xray::getShutter1()
{
  strcpy(message, interface->send_command_to_tube((char*)"sr:3 ",1,value,value));
  if(value==-9999)
	  strcpy(message, interface->send_command_to_tube((char*)"sr:3 ",1,value,value));
 return (message[0]-48);
}

//------------------------------------------------------------------------------------------------------------------------------------------------

int Xray::getShutter2()
{
  strcpy(message, interface->send_command_to_tube((char*)"sr:3 ",1,value,value));
  if(value==-9999)
	  strcpy(message, interface->send_command_to_tube((char*)"sr:3 ",1,value,value));
  return (message[4]-48);
}

//------------------------------------------------------------------------------------------------------------------------------------------------

int Xray::getShutter3()
{
  strcpy(message, interface->send_command_to_tube((char*)"sr:4 ",1,value,value));
  if(value==-9999)
	  strcpy(message, interface->send_command_to_tube((char*)"sr:4 ",1,value,value));
  return (message[0]-48);
}

//------------------------------------------------------------------------------------------------------------------------------------------------

int Xray::getShutter4()
{
  strcpy(message, interface->send_command_to_tube((char*)"sr:4 ",1,value,value));
  if(value==-9999)
	  strcpy(message, interface->send_command_to_tube((char*)"sr:4 ",1,value,value));
  return (message[4]-48);
}

//------------------------------------------------------------------------------------------------------------------------------------------------

void Xray::getShutters(int &s1,int &s2,int &s3,int &s4)
{
  strcpy(message, interface->send_command_to_tube((char*)"sr:3 ",1,value,value));
  if(value==-9999)
	  strcpy(message, interface->send_command_to_tube((char*)"sr:3 ",1,value,value));
  s1=(message[0]-48); s2=(message[4]-48);
  strcpy(message, interface->send_command_to_tube((char*)"sr:4 ",1,value,value));
  if(value==-9999)
	  strcpy(message, interface->send_command_to_tube((char*)"sr:4 ",1,value,value));
 s3=(message[0]-48); s4= (message[4]-48);
}

//------------------------------------------------------------------------------------------------------------------------------------------------

void Xray::print()
{
  /* prints all the characteristics of the xray object */
  cout<<"High Voltage\t:\t";if(getHVSwitch()) cout<<"ON"; else cout<<"OFF";
  cout<<"\nVoltage\t\t:\t"<<getVoltage();
  cout<<"\nCurrent\t\t:\t"<<getCurrent();
  cout<<"\nShutter 1\t:\t";if(getShutter1()) cout<<"ON"; else cout<<"OFF";
  cout<<"\nShutter 2\t:\t";if(getShutter2()) cout<<"ON"; else cout<<"OFF";
  cout<<"\nShutter 3\t:\t";if(getShutter3()) cout<<"ON"; else cout<<"OFF";
  cout<<"\nShutter 4\t:\t";if(getShutter4()) cout<<"ON"; else cout<<"OFF";
  cout<<"\nInterface\t:\t"<<interface->getSerial();
  cout<<"\n";	
}

//------------------------------------------------------------------------------------------------------------------------------------------------

void Xray::startWarmup(int warmupVoltage)
{
  cout<<"\ninside the startwarmup() in Xray.cpp\n";
  int s1,s2,s3,s4;
  getShutters(s1,s2,s3,s4);
  if(s1) interface->send_command_to_tube((char*)"cs:1 ",0,value,value);
  if(s2) interface->send_command_to_tube((char*)"cs:2 ",0,value,value);
  if(s3) interface->send_command_to_tube((char*)"cs:3 ",0,value,value);
  if(s4) interface->send_command_to_tube((char*)"cs:4 ",0,value,value);
  sprintf(message,"wu:4,%d ",warmupVoltage);
   interface->send_command_to_tube(message,0,value,value);
}

//------------------------------------------------------------------------------------------------------------------------------------------------

int Xray::getWarmupTimeRemaining()
{
  //again wt takes time/or more commands sent to show the right value
  strcpy(message, interface->send_command_to_tube((char*)"sr:6 ",1,value,value));
  strcpy(message, interface->send_command_to_tube((char*)"sr:6 ",1,value,value));
  if(value==-9999){
	  strcpy(message, interface->send_command_to_tube((char*)"sr:6 ",1,value,value));
	  strcpy(message, interface->send_command_to_tube((char*)"sr:6 ",1,value,value));
  }
  strcpy(message, interface->send_command_to_tube((char*)"wt ",1,value,value));
  strcpy(message, interface->send_command_to_tube((char*)"wt ",1,value,value));
  if(value==-9999){
	  strcpy(message, interface->send_command_to_tube((char*)"wt ",1,value,value));
	  strcpy(message, interface->send_command_to_tube((char*)"wt ",1,value,value));
  }
  return value;
}

//------------------------------------------------------------------------------------------------------------------------------------------------

bool Xray::isAccessPossible()
{
  //if either of the shutters AND the HV is on
  if( (getShutter1()||getShutter2()||getShutter3()||getShutter4())&&(getHVSwitch()) )
    return false;
  return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------

int Xray::getErrorCode()
{
  interface->send_command_to_tube((char*)"sr:12 ",1,value,value);
  interface->send_command_to_tube((char*)"sr:12 ",1,value,value);
  return value;
}

//------------------------------------------------------------------------------------------------------------------------------------------------

char* Xray::getErrorMessage()
{
  strcpy(message, interface->send_command_to_tube((char*)"er ",1,value,value));
  strcpy(message, interface->send_command_to_tube((char*)"er ",1,value,value));
  return message;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
void Xray::clearErrorCode()
{
  interface->send_command_to_tube((char*)"cl ",0,value,value);
}

//------------------------------------------------------------------------------------------------------------------------------------------------
#endif
