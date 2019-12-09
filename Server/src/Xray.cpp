#include "Xray.h"
#include "commonDefs.h"

#include <iostream>
#include <stdio.h>
#include <cstring>
using namespace std;

Xray::Xray(Interface* interface){
  this->interface=interface;
  FILE_LOG(logINFO) << "Tube: [usbPort:" << interface->getSerial() << "]";
}


Interface* Xray::getInterface() {
  return interface;
}


int Xray::isOnStandby() {
  //-9999 = switched off and 76 = standby
  int value = 0;
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


void Xray::setHVSwitch(int HVSwitch) {
  char buffer[COMMAND_BUFFER_LENGTH];
  memset(buffer, 0, sizeof(buffer));
  sprintf(buffer,"hv:%d ",HVSwitch);
  int value = 0;
  interface->send_command_to_tube(buffer,0,value,value);
}


int Xray::getHVSwitch() {
  //doing this twice cuz it takes time for
  //hv to get set if u read right after
  char buffer[COMMAND_BUFFER_LENGTH];
  memset(buffer, 0, sizeof(buffer));
  int value = 0;
  strcpy(buffer, interface->send_command_to_tube((char*)"sr:1 ",1,value,value));
  strcpy(buffer, interface->send_command_to_tube((char*)"sr:1 ",1,value,value));
  return (buffer[1]-48);
}


void Xray::setVoltage(int Voltage) {
  char buffer[COMMAND_BUFFER_LENGTH];
  memset(buffer, 0, sizeof(buffer));
  sprintf(buffer,"sv:%d ",Voltage);
  int value = 0;
  interface->send_command_to_tube(buffer,0,value,value);
}



void Xray::setCurrent(int Current)  {
  char buffer[COMMAND_BUFFER_LENGTH];
  memset(buffer, 0, sizeof(buffer));
  sprintf(buffer,"sc:%d ",Current);
  int value = 0;
  interface->send_command_to_tube(buffer,0,value,value);
}


void Xray::setVoltageAndCurrent(int Voltage, int Current)	{
  char buffer[COMMAND_BUFFER_LENGTH];
  memset(buffer, 0, sizeof(buffer));
  sprintf(buffer,"sn:%d,%d ",Voltage,Current);
  int value = 0;
  interface->send_command_to_tube(buffer,0,value,value);
}


 
int Xray::getVoltage() {
  int value = 0;
  interface->send_command_to_tube((char*)"vn ",1,value,value);
  if(value==-9999)
	  interface->send_command_to_tube((char*)"vn ",1,value,value);
  return value;
}

 
int Xray::getVoltageActual() {
  int value = 0;
  interface->send_command_to_tube((char*)"va ",1,value,value);
  if(value==-9999)
	  interface->send_command_to_tube((char*)"va ",1,value,value);
  return value;
}


int Xray::getCurrent() {
  int value = 0;
  interface->send_command_to_tube((char*)"cn ",1,value,value);
  if(value==-9999)
	  interface->send_command_to_tube((char*)"cn ",1,value,value);
  return value;
}

 
int Xray::getCurrentActual() {
  int value = 0;
  interface->send_command_to_tube((char*)"ca ",1,value,value);
  if(value==-9999)
	  interface->send_command_to_tube((char*)"ca ",1,value,value);
  return value;
}

 
void Xray::getVoltageAndCurrent(int &voltage, int &current) {
  interface->send_command_to_tube((char*)"gn ",1,voltage,current);
  if(voltage==-9999)
	  interface->send_command_to_tube((char*)"gn ",1,voltage,current);
}

 
void Xray::getVoltageAndCurrentActual(int &voltage, int &current) {
  interface->send_command_to_tube((char*)"ga ",1,voltage,current);
  if(voltage==-9999)
	  interface->send_command_to_tube((char*)"gn ",1,voltage,current);
}

void Xray::setShutter(int Shutter, bool on)	{
  cout<<"setting shutters on/off";

  char buffer[COMMAND_BUFFER_LENGTH];
  memset(buffer, 0, sizeof(buffer));
  if(on) sprintf(buffer,"os:");
  else sprintf(buffer,"cs:");

  char temp[20];
  sprintf(temp,"%d",Shutter);
  strcat(buffer,temp);
  strcat(buffer," ");
  int value = 0;
  interface->send_command_to_tube(buffer,0,value,value);
}
 

int Xray::getShutter1() {
  char buffer[COMMAND_BUFFER_LENGTH];
  memset(buffer, 0, sizeof(buffer));
  int value = 0;
  strcpy(buffer, interface->send_command_to_tube((char*)"sr:3 ",1,value,value));
  if(value==-9999)
	  strcpy(buffer, interface->send_command_to_tube((char*)"sr:3 ",1,value,value));
 return (buffer[0]-48);
}


int Xray::getShutter2() {
  char buffer[COMMAND_BUFFER_LENGTH];
  memset(buffer, 0, sizeof(buffer));
  int value = 0;
  strcpy(buffer, interface->send_command_to_tube((char*)"sr:3 ",1,value,value));
  if(value==-9999)
	  strcpy(buffer, interface->send_command_to_tube((char*)"sr:3 ",1,value,value));
  return (buffer[4]-48);
}


int Xray::getShutter3() {
  char buffer[COMMAND_BUFFER_LENGTH];
  memset(buffer, 0, sizeof(buffer));
  int value = 0;
  strcpy(buffer, interface->send_command_to_tube((char*)"sr:4 ",1,value,value));
  if(value==-9999)
	  strcpy(buffer, interface->send_command_to_tube((char*)"sr:4 ",1,value,value));
  return (buffer[0]-48);
}


int Xray::getShutter4() {
  char buffer[COMMAND_BUFFER_LENGTH];
  memset(buffer, 0, sizeof(buffer));
  int value = 0;
  strcpy(buffer, interface->send_command_to_tube((char*)"sr:4 ",1,value,value));
  if(value==-9999)
	  strcpy(buffer, interface->send_command_to_tube((char*)"sr:4 ",1,value,value));
  return (buffer[4]-48);
}


void Xray::getShutters(int &s1,int &s2,int &s3,int &s4) {
  char buffer[COMMAND_BUFFER_LENGTH];
  memset(buffer, 0, sizeof(buffer));
  int value = 0;
  strcpy(buffer, interface->send_command_to_tube((char*)"sr:3 ",1,value,value));
  if(value==-9999)
	  strcpy(buffer, interface->send_command_to_tube((char*)"sr:3 ",1,value,value));
  s1=(buffer[0]-48); s2=(buffer[4]-48);
  strcpy(buffer, interface->send_command_to_tube((char*)"sr:4 ",1,value,value));
  if(value==-9999)
	  strcpy(buffer, interface->send_command_to_tube((char*)"sr:4 ",1,value,value));
 s3=(buffer[0]-48); s4= (buffer[4]-48);
}


void Xray::startWarmup(int warmupVoltage) {
  cout<<"\ninside the startwarmup() in Xray.cpp\n";
  int s1,s2,s3,s4;
  getShutters(s1,s2,s3,s4);
  int value = 0;
  if(s1) interface->send_command_to_tube((char*)"cs:1 ",0,value,value);
  if(s2) interface->send_command_to_tube((char*)"cs:2 ",0,value,value);
  if(s3) interface->send_command_to_tube((char*)"cs:3 ",0,value,value);
  if(s4) interface->send_command_to_tube((char*)"cs:4 ",0,value,value);
  char buffer[COMMAND_BUFFER_LENGTH];
  memset(buffer, 0, sizeof(buffer));
  sprintf(buffer,"wu:4,%d ",warmupVoltage);
   interface->send_command_to_tube(buffer,0,value,value);
}


int Xray::getWarmupTimeRemaining() {
  //again wt takes time/or more commands sent to show the right value
  char buffer[COMMAND_BUFFER_LENGTH];
  memset(buffer, 0, sizeof(buffer));
 int value = 0;
   strcpy(buffer, interface->send_command_to_tube((char*)"sr:6 ",1,value,value));
  strcpy(buffer, interface->send_command_to_tube((char*)"sr:6 ",1,value,value));
  if(value==-9999){
	  strcpy(buffer, interface->send_command_to_tube((char*)"sr:6 ",1,value,value));
	  strcpy(buffer, interface->send_command_to_tube((char*)"sr:6 ",1,value,value));
  }
  strcpy(buffer, interface->send_command_to_tube((char*)"wt ",1,value,value));
  strcpy(buffer, interface->send_command_to_tube((char*)"wt ",1,value,value));
  if(value==-9999){
	  strcpy(buffer, interface->send_command_to_tube((char*)"wt ",1,value,value));
	  strcpy(buffer, interface->send_command_to_tube((char*)"wt ",1,value,value));
  }
  return value;
}


bool Xray::isAccessPossible() {
  //if either of the shutters AND the HV is on
  if( (getShutter1()||getShutter2()||getShutter3()||getShutter4())&&(getHVSwitch()) )
    return false;
  return true;
}


int Xray::getErrorCode() {
  int value = 0;
  interface->send_command_to_tube((char*)"sr:12 ",1,value,value);
  interface->send_command_to_tube((char*)"sr:12 ",1,value,value);
  return value;
}


std::string Xray::getErrorMessage() {
  char buffer[COMMAND_BUFFER_LENGTH];
  memset(buffer, 0, sizeof(buffer));
  int value = 0;
  std::string result;
  result.assign(interface->send_command_to_tube((char*)"er ",1,value,value));
  result.assign(interface->send_command_to_tube((char*)"er ",1,value,value));
  return result;
}

void Xray::clearErrorCode() {
  int value = 0;
  interface->send_command_to_tube((char*)"cl ",0,value,value);
}

void Xray::print() {
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