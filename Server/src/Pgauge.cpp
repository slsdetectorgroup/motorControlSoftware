/********************************************//**
 * @file Pgauge.cpp
 * @short Defines the Pgauge objects
 * @author Dhanya
 ***********************************************/
#include "Pgauge.h"
#ifdef VACUUMBOX

#include <iostream>
#include <sstream>
#include <cstring>
#include <cstdio>
#include <stdio.h>
using namespace std;


Pgauge::Pgauge(Interface* interface)
{
    this->interface=interface;
}


Interface* Pgauge::getInterface()
{
    return interface;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

bool Pgauge::getPressure(string& status1, double& value1, string& status2, double& value2) {
    cout << "Getting Pressure" << endl;
    char command[10]="com\r\n";
    char crapstring[10]="c\r\n";
    int retsize = 27;
    int size = 255;
    char buffer[size];

    // send command
    memset(buffer, 0, size);
    strncpy(buffer, command, strlen(command));
    if(!interface->send_command_to_pressure(buffer, size, true, false))
        return false;

    // could be fail due to max_retries or reading half the values
    if (strlen(buffer) < retsize)
          return false;

    // scan values
    int s1=-1, s2=-1;
    int sret = sscanf(buffer,"%d,%lf,%d,%lf\r\n",&s1, &value1, &s2, &value2);
    if (sret < 4) {
        cout << "Error: cannot scan pressure return buffer. [sret:" << sret << "]. Buffer:" << buffer << endl;
        return false;
    }
#ifdef VERBOSE_MOTOR
    printf("Status:%d, value:%e\nStatus2:%d, value2:%e\n",status1, value1, status2, value2);
#endif
    switch(s1) {
    case 0: status1.assign("Measurement data okay"); break;
    case 1: status1.assign("Underrange"); break;
    case 2: status1.assign("Overrange"); break;
    case 3: status1.assign("Sensor error"); break;
    case 4: status1.assign("Sensor off"); break;
    case 5: status1.assign("No Sensor"); break;
    default:status1.assign("Identification error"); break;
    }
    switch(s2) {
    case 0: status2.assign("Measurement data okay"); break;
    case 1: status2.assign("Underrange"); break;
    case 2: status2.assign("Overrange"); break;
    case 3: status2.assign("Sensor error"); break;
    case 4: status2.assign("Sensor off"); break;
    case 5: status2.assign("No Sensor"); break;
    default:status2.assign("Identification error"); break;
    }

    //sending crap to stop sending from the controller
    interface->send_command_to_pressure(crapstring, strlen(crapstring), false, false);
    interface->read_from_pressure(crapstring, strlen(crapstring));

    return true;
}


//-------------------------------------------------------------------------------------------------------------------------------------------------

void Pgauge::print()
{
    /* prints all the characteristics of the Pgauge object */
    cout << "Pressure Gauge" << endl;
    cout<<"interface\t:\t"<<interface->getSerial()<<endl;
    string status1="";double value1=-1;string status2="";double value2=-1;
    if(getPressure(status1,value1,status2,value2)) {
        printf("Gauge 1\n\tStatus\t:\t%s\n\tPressure:\t%e\nGauge 2\n\tStatus\t:\t%s\n\tPressure:\t%e\n",
                status1.c_str(), value1, status2.c_str(), value2);
    }else printf("ERROR: Could not read pressure from pressure controller\n");

    cout<<"\n";
}

//-------------------------------------------------------------------------------------------------------------------------------------------------
#endif
