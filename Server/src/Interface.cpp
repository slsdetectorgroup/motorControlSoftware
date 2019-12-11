#include "Interface.h"
#include "commonDefs.h"

#include <sstream>

#include <fcntl.h>  /* File control definitions */
#include <iostream>
#include <cstring>
#include <cstdlib> //exit
#include <unistd.h> //usleep in raspberrypi
#include <stdio.h>
#include <termios.h>  /* POSIX terminal control definitions */
#include <errno.h>

//O_NOCTTY flag tells UNIX that this program doesn't want to be the "controlling terminal" for that port.
//O_NDELAY flag tells UNIX that this program doesn't care what state the DCD signal line is in - whether the other end of the port is up and running. If you do not specify this flag, your process will be put to sleep until the DCD signal line is the space voltage.
//B9600	9600 baud
//CS8	8 data bits
//CSTOPB	2 stop bits (1 otherwise)
//CLOCAL	Local line - do not change "owner" of port
//CREAD	Enable receiver
//IGNPAR	Ignore parity errors


#define CONTROLLER_MAX_TX_ATTEMPTS		(5)
#define CONTROLLER_MAX_RX_ATTEMPTS		(5)
#define CONTROLLER_MAX_WAIT_ATTEMPTS	(1)
#define CONTROLLER_READ_WAIT_US			(50000) // because of raspberry pi
#define TUBE_READ_WAIT_US				(200000)


Interface::Interface(std::string serial, InterfaceIndex index) {
	this->serial = serial;
	switch	(index) {
		case TUBE:
			TubeInterface();
			break;
		case PRESSURE:
			PressureInterface();
			break;
		case FILTER_WHEEL:
			FilterWheelInterface();
			break;
		default:
			ControllerInterface();
			break;
	}
}

Interface::~Interface() {
	close(serialfd);
}

std::string Interface::getSerial() {
	return serial;
}

void Interface::ControllerInterface() {
	FILE_LOG(logINFO) << "\tMotorcontroller, checking:" << serial;
	serialfd = open (serial.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
	if (serialfd == -1) {
		FILE_LOG(logWARNING) << "Unable to open port " << serial << " for controller";
		FILE_LOG(logWARNING) << "Fail";
		throw std::runtime_error("Failed to open port");
	}

	struct termios new_serial_conf;
	memset(&new_serial_conf, 0, sizeof(new_serial_conf));
	new_serial_conf.c_cflag = B57600 | CS8 | CREAD | CLOCAL; // control options
	new_serial_conf.c_iflag = 0; // input options
	new_serial_conf.c_oflag = 0; // output options
	new_serial_conf.c_lflag = 0; // line options
	// flush input
	sleep(2);
	tcflush(serialfd, TCIOFLUSH);
	if (tcsetattr(serialfd, TCSANOW, &new_serial_conf) != 0) {
		std::ostringstream oss;
		oss << errno << " from tcsetattr: " << strerror(errno);
		throw std::runtime_error(oss.str());
	}

	ValidateController(); 
}


void Interface::ValidateController() {
	FILE_LOG(logINFO) << "\tValidating Controller";
	ControllerSend("st ", true, true);
	FILE_LOG(logINFO) << "\tSuccess";
	ControllerWaitForIdle();
}


void Interface::ControllerWaitForIdle() {
	for (;;) {
		std::string result = ControllerSend("st ", true, false);
		int status = atoi(result.c_str());
		if (status == 0) {
			FILE_LOG(logDEBUG) << "Controller Idle";
			break;
		}
		FILE_LOG(logDEBUG) << "Controller Busy";
	}
}


std::string Interface::ControllerSend(std::string command, bool readBack, bool validate) {
	int waitAttempts = 0;
	bool verbose =  false; //validate ? false : true; // for debuggign
	for (;;) {
        try {
            std::string result = ControllerSendCommand(command, readBack, verbose);
			return result;
        } catch (...) {
			if (validate) {
				++waitAttempts;
				if (waitAttempts == CONTROLLER_MAX_WAIT_ATTEMPTS) {
					std::ostringstream oss;
					oss << "Wait attempt number " << CONTROLLER_MAX_WAIT_ATTEMPTS << " for idle at port " << serial << ". Aborting wait.";			
					close(serialfd);
					FILE_LOG(logWARNING) << "Fail";
					throw std::runtime_error(oss.str());
				}
			} 
        }	
		usleep(CONTROLLER_READ_WAIT_US);	
	}
}

std::string Interface::ControllerSendCommand(std::string command, bool readBack, bool verbose) {
	if (command != "st " && command != "pos ") { // for debugging
		FILE_LOG(logINFO) << "\tSending [" << command << "]   (port:" << serial << ", read:" << readBack << ')';
	}

	// send command to controller
	char buffer[COMMAND_BUFFER_LENGTH];
	memset(buffer, 0, sizeof(buffer));
	strcpy(buffer, command.c_str());
	int ret = -1;
	int attempt = 0;
	while (ret == -1) {
		ret = write (serialfd, buffer, sizeof(buffer));
		++attempt;
		if (attempt == CONTROLLER_MAX_TX_ATTEMPTS) {
			std::ostringstream oss;
			oss << "Send attempt number " << CONTROLLER_MAX_TX_ATTEMPTS << " for [" << buffer << "] to port " << serial << ". Aborting command.";
			if (verbose) {
				FILE_LOG(logERROR) << oss.str();
			}
			throw std::runtime_error(oss.str());
		}
	}
	if (attempt > 1) {
		FILE_LOG(logDEBUG) << "Send attempt " << attempt;
	}
	// no read back
	if (!readBack) {
		return command;
	} 
	// read back
	char result[COMMAND_BUFFER_LENGTH];
	ret = -1;
	attempt = 0;
	while (ret == -1) {
		memset(result, 0, sizeof(result));	
		usleep(CONTROLLER_READ_WAIT_US);
		ret = read (serialfd, result, sizeof(result));
		++attempt;
		if (attempt == CONTROLLER_MAX_RX_ATTEMPTS) {
			std::ostringstream oss;
			oss << "Receive attempt number " << CONTROLLER_MAX_RX_ATTEMPTS << " for [" << buffer << "] to port " << serial << ". Aborting read.";
			if (verbose) {
				FILE_LOG(logERROR) << oss.str();
			}
			throw std::runtime_error(oss.str());
		}		
	}
	if (attempt > 1) {
		FILE_LOG(logDEBUG) << "receive attempt " << attempt;
	}

	// throw error if read buffer empty
	if (strlen(result) == 0) {
		std::ostringstream oss;
		oss << "Empty receive buffer to controller port " << serial;
		//FILE_LOG(logERROR) << oss.str();
		throw std::runtime_error(oss.str());		
	}

	if (command != "st ") {
		FILE_LOG(logINFO) << "\tReceived [" << command << "]: " << result;
	}
	return std::string(result);
}



void Interface::TubeInterface() {
	FILE_LOG(logINFO) << "\tXray tube, checking:" << serial;
	serialfd = open (serial.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
	if (serialfd == -1) {
		FILE_LOG(logWARNING) << "Unable to open port " << serial << " for xray tube\n";
        FILE_LOG(logWARNING) << "Fail";
		throw std::runtime_error("Failed to open port");
	}

	struct termios new_serial_conf;
	memset(&new_serial_conf, 0, sizeof(new_serial_conf));
	new_serial_conf.c_cflag = B9600 | CS8 | CREAD | CLOCAL; // control options
	new_serial_conf.c_iflag = IGNPAR; // input options
	new_serial_conf.c_oflag = 0; // output options
	new_serial_conf.c_lflag = 0; // line options
	// flush input 
	sleep(2);
	tcflush(serialfd, TCIOFLUSH);
	if (tcsetattr(serialfd, TCSANOW, &new_serial_conf)!= 0) {
		std::ostringstream oss;
		oss << errno << " from tcsetattr: " << strerror(errno);
		throw std::runtime_error(oss.str());
	}

	ValidateTube();
}

void Interface::ValidateTube() {
	FILE_LOG(logINFO) << "\tValidating Tube";
	TubeSend("sr:12 ", true, true);
	FILE_LOG(logINFO) << "\tSuccess";
}

std::string Interface::TubeSend(std::string command, bool readBack, bool validate) {
	FILE_LOG(logINFO) << "\tSending [" << command << "]   (port:" << serial << ", read:" << readBack << ')';
	
	// send command
	char buffer[COMMAND_BUFFER_LENGTH];
	memset(buffer, 0, sizeof(buffer));
	strcpy(buffer, command.c_str());	

	int ret = write (serialfd, buffer, sizeof(buffer));
	if (ret == -1) {
		std::ostringstream oss;
		oss << "Could not write to tube";
		if (validate) {
			FILE_LOG(logWARNING) << "Fail";
			close(serialfd);
		}
		throw std::runtime_error(oss.str());
	}

	// read back
	if (!readBack) {
		return command;
	}
	char result[COMMAND_BUFFER_LENGTH];
	memset(result, 0, sizeof(result));	
	usleep(TUBE_READ_WAIT_US);
	ret = read (serialfd, result, sizeof(result));
	if (ret == -1) {
		std::ostringstream oss;
		oss << "Could not read from tube";
		if (validate) {
			FILE_LOG(logWARNING) << "Fail";
			close(serialfd);
		}
		throw std::runtime_error(oss.str());	
	}

	// throw error if read buffer empty
	if (strlen(result) == 0) {
		std::ostringstream oss;
		oss << "Empty receive buffer from tube";
		if (validate) {
			FILE_LOG(logWARNING) << "Fail";
			close(serialfd);
		}		
		throw std::runtime_error(oss.str());
	}

	FILE_LOG(logINFO) << "\tReceived [" << command << "]: " << result;
	return std::string(result);
}


char* Interface::send_command_to_tube(char* c, int rb, int &value, int &value2)  {
	//for safety,if a usb serial port calls this function, it exits
	//if(strstr (serial,"USB")!=NULL) exit(-1);


	char buffer[255]="", command[200]="", binaryNumber[9]="00000000";
	char* p = binaryNumber;
	int temp;

	strcpy(command,c);

#ifdef VERBOSE_MOTOR
	std::cout<<"Tube Sending command:"<<command<<std::endl;
#endif

	if (write (serialfd,command,strlen(command))==-1)
		std::cout<<"error sending the command \n";

	if (rb)
	{
		buffer[0]='\0';
		int count = 0;
		while(!strcmp(buffer,""))
		{
			usleep(200000);

			if ( read (serialfd, buffer, 255)==-1)
				std::cout<<"error receiving data back \n";
			count++;
			if(count==5)//if((count==5)&&(!strcmp(c,"sr:12 ")))
			{
				std::cout<<"ERROR:The tube is probably switched off. Received no output from Tube for command:"<<c<<std::endl;
				value=-9999;
				value2=-9999;
				strcpy(binaryNumber,"99999999");
				return p;
			}
		}
		//gets rid of asterix in front
		strncpy (buffer,buffer+1,strlen(buffer)-1);

		if(!strcmp(c,"er "))
		{
			p=buffer;
			return p;
		}

		//lookin for a colon in btw, means theres 2 numbers..for eg.  gn = voltage:current
		char* pch;
		pch = strchr(buffer,':');
		if (pch!=NULL)
		{
			char ctemp1[200],ctemp2[200];
			strncpy(ctemp1,buffer,pch-buffer);
			value = atoi(ctemp1);
			strncpy(ctemp2,pch+1,11);
			value2 = atoi(ctemp2);
		}
		else
		{
			temp=atoi(buffer);
			value = temp;

			//converts the data received to the binary form
			if(temp<0)
			{
				std::cout<<"ERROR: the data received for command '"<<command<<"' is :"<<temp<<std::endl;
				exit(-1);
			}
			else if (temp>0)
			{
				for(int i=7;i>=0;i--)
				{
					if(temp<1)
						break;
					binaryNumber[i]='0'+(temp%2);
					temp/=2;
				}
			}
		}

	}
#ifdef VERBOSE_MOTOR
	std::cout<<"Interface: Command:"<<c<<"  Received value:"<<p<<" : value:"<<value<<std::endl;
#endif 
	return p;
}



void Interface::PressureInterface() {
	FILE_LOG(logINFO) << "Pressure, checking:" << serial;
	serialfd = open (serial.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
	if (serialfd == -1) {
		FILE_LOG(logWARNING) << "Unable to open port " << serial << " for pressure, check permissions to use it\n";
        FILE_LOG(logWARNING) << "Fail" << std::endl;
		throw std::runtime_error("Fail to open port");
	}
	struct termios new_serial_conf;
	memset(&new_serial_conf, 0, sizeof(new_serial_conf));
	/* control options */
	//B9600	9600 baud
	//CS8	8 data bits
	//CSTOPB	2 stop bits (1 otherwise)
	//CLOCAL	Local line - do not change "owner" of port
	//CREAD	Enable receiver
	new_serial_conf.c_cflag = B9600 | CS8 | CLOCAL | CREAD ;

	/* input options */
	//IGNPAR	Ignore parity errors
	new_serial_conf.c_iflag = IGNPAR;

	/* output options */
	new_serial_conf.c_oflag = 0;

	/* line options */
	new_serial_conf.c_lflag = 0; // doesnt work with ICANON

	/* flush input */
	sleep(2);
	tcflush(serialfd, TCIOFLUSH);
	tcsetattr(serialfd, TCSANOW, &new_serial_conf);


	if (!checkPressureGaugePort()) {
		FILE_LOG(logWARNING) << "Fail" << std::endl;
		close(serialfd);
		throw std::runtime_error("Fail to communicate with pressure gauge");
	}
    FILE_LOG(logINFO) << "Success" << std::endl;
}


void Interface::FilterWheelInterface() {

	//O_NOCTTY flag tells UNIX that this program doesn't want to be the "controlling terminal" for that port.
	//O_NDELAY flag tells UNIX that this program doesn't care what state the DCD signal line is in - whether the other end of the port is up and running. If you do not specify this flag, your process will be put to sleep until the DCD signal line is the space voltage.
	FILE_LOG(logINFO) << "Filter wheel, checking:" << serial;
	serialfd= open (serial.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
	if (serialfd == -1) {
		FILE_LOG(logWARNING) << "Unable to open port " << serial << " for filter wheel, check permissions to use it\n";
        FILE_LOG(logWARNING) << "Fail" << std::endl;
		throw std::runtime_error("Fail to open port");
	}
	struct termios new_serial_conf;
	memset(&new_serial_conf, 0, sizeof(new_serial_conf));
	/* control options */
	//B9600	115200 baud
	//CS8	8 data bits
	//CSTOPB	2 stop bits (1 otherwise)
	//CLOCAL	Local line - do not change "owner" of port
	//CREAD	Enable receiver
	new_serial_conf.c_cflag = B115200 | CS8 | CREAD | CLOCAL;

	/* input options */
	//IGNPAR	Ignore parity errors
	new_serial_conf.c_iflag = IGNPAR;

	/* output options */
	new_serial_conf.c_oflag = 0;

	/* line options */
	new_serial_conf.c_lflag = 0;//ICANON;

	/* flush input */
	sleep(2);
	tcflush(serialfd, TCIOFLUSH);
	tcsetattr(serialfd, TCSANOW, &new_serial_conf);

    FILE_LOG(logINFO) << "Success";
}



bool Interface::checkPressureGaugePort() {

    char crapstring[10]="c\r\n";
    const int size = 255;
    char modelcommand[10]="ayt\r\n";
    char modelstring[10]="TPG36";
    char buffer[size];

    // was messed up as we were sending many messages thinking it is a motor or an xraytube
    // reset it to normal by sending crap and reading the negative acknowledge
    send_command_to_pressure(crapstring, strlen(crapstring), false, false);
    read_from_pressure(crapstring, strlen(crapstring));

    memset(buffer, 0, size);
    strncpy(buffer, modelcommand, strlen(modelcommand));
    if(!send_command_to_pressure(buffer, size))
        return false;
    if (strlen(buffer) < strlen(modelstring)) // could be fail due to max_retries
          return false;
    if (strncmp(buffer, modelstring, strlen(modelstring))) {
        std::cout << serial << " is not Pressure Gauge Controller" << std::endl << std::endl;
        return false;
    }

    return true;
}


int Interface::read_from_pressure(char* c, int size) {
    const int max_retries = 10;
    const int usleep_time = 200000; // 100ms
    int ret = -1;

    for (int retry = 0; retry < max_retries; ++retry) {

        usleep(usleep_time);
        memset(c, 0, size);

        ret = read (serialfd, c, size);
        if (ret < 1)
            std::cout << "Error reading back. Attempt #" << retry << " of " <<
            max_retries << ". Received " << ret << " bytes." << std::endl;
        else
            break;
    }
    return ret;
}


bool Interface::send_command_to_pressure(char* c, const int size, bool rb, bool enq) {
    char ackstring[10]="\x6\r\n"; //\x6\xd\xa
    char enqstring[10]="\x5";

    // send command
    std::cout << "Pressure, sending command (size:" << strlen(c) << ") "
            "[" << c << "]" << std::endl;
    if (write(serialfd, c, strlen(c)) == -1) {
        std::cout << "Error sending command" << std::endl;
        return false;
    }

    // read
    if (rb) {
        // read acknowledge
        if (read_from_pressure(c, size) < strlen(ackstring)) // could be fail due to max_retries
            return false;
        if (strncmp(c, ackstring, strlen(ackstring))) {
            std::cout << "Error reading acknowledge. Read";
            for (int i = 0; i < strlen(c); ++i) printf("0x%x ",c[i]);
            std::cout << std::endl;
            return false;
        }std::cout << "Pressure, received ack"<<std::endl;

        // send enquiry
        if (enq) {
            std::cout << "Pressure, sending command (size:" << strlen(enqstring) << ") "
                    "[" << enqstring << "]" << std::endl;
            if (write(serialfd, enqstring, strlen(enqstring)) == -1) {
                std::cout << "Error sending enquiry command" << std::endl;
                return false;
            }
        }

        // read output
        if (read_from_pressure(c, size) < 1)
            return false;
    }

    return true;
}


char* Interface::send_command_to_fw(char* c, int rb) {
	//rb=1;
	char buffer[255]="", command[200]="";
	char* p = buffer;

	strcpy(command,c);
#ifdef VERBOSE_MOTOR
	std::cout<<"Sending command:*"<<command<<"*"<<;
#endif
	strcat(command,"\r");

#ifdef VERBOSE_MOTOR
	std::cout<<" to port "<<serial<<std::endl;
#endif

	if (write (serialfd,command,strlen(command))==-1)
		std::cout<<"error sending the command \n";

		return p;
}

