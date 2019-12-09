#include "Pgauge.h"
#include "Interface.h"
#include "commonDefs.h"

#include <sstream>
#include <cstring>

#define NUM_PRESSURE_GAUGES_PER_CONTROLLER  (2)


Pgauge::Pgauge(Interface* interface) {
    this->interface=interface;
    FILE_LOG(logINFO) << "Pressure Gauge: [usbPort:" << interface->getSerial() << "]";
}


Interface* Pgauge::getInterface() {
    return interface;
}


bool Pgauge::getPressure(std::string& status1, double& value1, std::string& status2, double& value2) {
    std::cout << "Getting Pressure" << std::endl;
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
        FILE_LOG(logERROR) << "Cannot scan pressure return buffer. [sret:" << sret << "]. Buffer:" << buffer;
        return false;
    }
    //printf("Status:%d, value:%e\nStatus2:%d, value2:%e\n",status1, value1, status2, value2);
    
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



void Pgauge::print() {
    std::cout << "Pressure Gauge" << std::endl;
    std::cout << "\tUsb port: "<< interface->getSerial() << std::endl;
    std::string status1,status2;
    double value1 = -1, value2 = -1;
    if(getPressure(status1, value1, status2, value2)) {
        printf("\tGauge 1\n\tStatus\t:\t%s\n\tPressure:\t%e\nGauge 2\n\tStatus\t:\t%s\n\tPressure:\t%e\n",
                status1.c_str(), value1, status2.c_str(), value2);
    } else {
        FILE_LOG(logERROR) << "Could not read pressure from pressure controller";
    } 
    std::cout << std::endl;
}

