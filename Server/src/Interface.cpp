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
#define CONTROLLER_READ_WAIT_US			(50000) // because of raspberry pi
#define CONTROLLER_CHECK_STATUS_CMD		("st ")
#define CONTROLLER_POSITION_CMD			("pos ")

#define TUBE_MAX_RX_ATTEMPTS			(10)
#define TUBE_MAX_REPEAT_ATTEMPTS		(5)
#define TUBE_READ_WAIT_US				(200000)
#define TUBE_STATUS_CMD					("sr:12 ")

#define PRESSURE_MAX_RX_ATTEMPTS		(5)
#define PRESSURE_MAX_REPEAT_ATTEMPTS	(5)
#define PRESSURE_READ_WAIT_US			(200000)
#define PRESSURE_ARE_YOU_THERE_CMD		("AYT\r\n")
#define PRESSURE_TYPE_RESPONSE			("TPG362")
#define PRESSURE_ACK_RESPONSE			("\x6\r\n")
#define PRESSURE_NACK_RESPONSE			("\x15\r\n")
#define PRESSURE_ENQUIRY_CMD			("\x5")


Interface::Interface(std::string serial, int serialPortNumber, InterfaceIndex index) 
	: serial(serial), serialPortNumber(serialPortNumber), serialfd(-1) {
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

int Interface::getSerialPortNumber() {
	return serialPortNumber;
}

void Interface::ControllerInterface() {
	FILE_LOG(logINFO) << "\tMotorcontroller, checking:" << serial;
	serialfd = open (serial.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
	if (serialfd == -1) {
		FILE_LOG(logDEBUG) << "Unable to open port " << serial << " for controller";
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
	try {
		// will throw for ports that cant communicate
		std::string result = ControllerSendCommand(CONTROLLER_CHECK_STATUS_CMD, true);
		// will throw for ports (tube) that can communciate, but wrong port (strange answer)
		ControllerIsIdle(result, true);
	} catch (...) {
		close(serialfd);
		FILE_LOG(logWARNING) << "Fail";
		throw;
	}

	FILE_LOG(logINFOGREEN) << "\tSuccess";
	ControllerWaitForIdle();
}

bool Interface::ControllerIsIdle(std::string result, bool validate) {
	std::istringstream iss(result);
	int status = 1;
	iss >> status;
	if (iss.fail()) {
		std::ostringstream oss;
		oss << "Unknown controller status " + result;
		if (validate) {
			throw std::runtime_error(oss.str());
		}
		FILE_LOG(logDEBUG) << oss.str();
		return false;
	}
	if (status == 0) {
		FILE_LOG(logDEBUG) << "Controller Idle";
		return true;
	}
	FILE_LOG(logDEBUG) << "Controller Busy";
	return false;
}

void Interface::ControllerWaitForIdle() {
	for (;;) {
		std::string result = ControllerSend(CONTROLLER_CHECK_STATUS_CMD, true);
		if (ControllerIsIdle(result)) {
			FILE_LOG(logDEBUG) << "Controller Idle";
			break;
		}
		FILE_LOG(logDEBUG) << "Controller Busy";
	}
}

std::string Interface::ControllerSend(std::string command, bool readBack) {
	for (;;) {
        try {
            std::string result = ControllerSendCommand(command, readBack);
			return result;
        } catch (...) {
			;
        }	
		usleep(CONTROLLER_READ_WAIT_US);	
	}
}

std::string Interface::ControllerSendCommand(std::string command, bool readBack) {
	if (command != std::string(CONTROLLER_CHECK_STATUS_CMD) && command != std::string(CONTROLLER_POSITION_CMD)) { // for debugging
		FILE_LOG(logINFO) << "\tSending [" << command << "]   (port:" << serial << ", read:" << readBack << ')';
	}
	bool verbose = true;

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
	if (attempt > 0) {
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
	if (attempt > 0) {
		FILE_LOG(logDEBUG) << "receive attempt " << attempt;
	}

	// throw error if read buffer empty
	if (strlen(result) == 0) {
		std::ostringstream oss;
		oss << "Empty receive buffer to controller port " << serial;
		//FILE_LOG(logERROR) << oss.str();
		throw std::runtime_error(oss.str());		
	}

	if (command != std::string(CONTROLLER_CHECK_STATUS_CMD)) {
		FILE_LOG(logINFO) << "\tReceived [" << command << "]: " << result;
	}
	return std::string(result);
}



void Interface::TubeInterface() {
	FILE_LOG(logINFO) << "\tXray tube, checking:" << serial;
	serialfd = open (serial.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
	if (serialfd == -1) {
		FILE_LOG(logDEBUG) << "Unable to open port " << serial << " for xray tube";
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
	try {
		std::string result = TubeSend(TUBE_STATUS_CMD, true);
	} catch (...) {
			close(serialfd);
			FILE_LOG(logWARNING) << "Fail";
			throw;
	}
	FILE_LOG(logINFOGREEN) << "\tSuccess";
}

std::string Interface::TubeSend(std::string command, bool readBack) {
	int attempts = 0;
	for (;;) {
        try {
            std::string result = TubeSendCommand(command, readBack);
			// ensure valid data as well for error code
			if (command == std::string(TUBE_STATUS_CMD)) {
				result.erase(result.begin());
				std::istringstream iss(result);
				int status = -1;
				iss >> status;
				if (iss.fail()) {
					std::ostringstream oss;
					oss << "Unknown tube status " + result;
					FILE_LOG(logERROR) << oss.str();
					throw std::runtime_error(oss.str());
				}
			}
			return result;
        } catch (const std::exception& e) {
			++attempts;
			if (attempts == TUBE_MAX_REPEAT_ATTEMPTS) {
				std::ostringstream oss;
				oss << "Tube is probably switched off. " << e.what();
				throw TubeOffError(oss.str());
			}
        }	
		usleep(TUBE_READ_WAIT_US);	
	}
}

std::string Interface::TubeSendCommand(std::string command, bool readBack) {
	FILE_LOG(logINFO) << "\tSend Tube [" << command << "]:\t(port:" << serial << ", read:" << readBack << ')';
	
	bool verbose = false;

	// send command
	char buffer[COMMAND_BUFFER_LENGTH];
	memset(buffer, 0, sizeof(buffer));
	strcpy(buffer, command.c_str());	

	int ret = write (serialfd, buffer, strlen(buffer));
	if (ret == -1) {
		std::ostringstream oss;
		oss << "Could not write to tube";
		if (verbose) {
			FILE_LOG(logERROR) << oss.str();
		}
		throw std::runtime_error(oss.str());
	}

	// read back
	if (!readBack) {
		return command;
	}
	char result[COMMAND_BUFFER_LENGTH];
	ret = -1;
	int attempt = 0;
	while (ret == -1) {
		memset(result, 0, sizeof(result));	
		usleep(TUBE_READ_WAIT_US);
		ret = read (serialfd, result, sizeof(result));
		++attempt;
		if (attempt == TUBE_MAX_RX_ATTEMPTS) {
			std::ostringstream oss;
			oss << "Receive attempt number " << TUBE_MAX_RX_ATTEMPTS << " for [" << buffer << "] to tube " << serial << ". Aborting read.";
			if (verbose) {
				FILE_LOG(logERROR) << oss.str();
			}
			throw std::runtime_error(oss.str());
		}
	}
	if (attempt > 0) {
		FILE_LOG(logDEBUG) << "receive attempt " << attempt;
	}			

	// throw error if read buffer empty
	if (strlen(result) == 0) {
		std::ostringstream oss;
		oss << "Empty receive buffer from tube";
		if (verbose) {
			FILE_LOG(logERROR) << oss.str();
		}
		throw std::runtime_error(oss.str());
	}

	FILE_LOG(logINFO) << "\tRead Tube [" << command << "]:\t" << result;
	return std::string(result);
}


void Interface::PressureInterface() {
	FILE_LOG(logINFO) << "\tPressure, checking:" << serial;
	serialfd = open (serial.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
	if (serialfd == -1) {
		FILE_LOG(logDEBUG) << "Unable to open port " << serial << " for pressure gauge";
        FILE_LOG(logWARNING) << "Fail";
		throw std::runtime_error("Fail to open port");
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

	ValidatePressureGauge();
}

void Interface::ValidatePressureGauge() {
	// send \r\n to clear commands sent by other interfaces
	try {
		PressureGaugeSendCommand("\r\n");
	} catch (...) {
		;
	}
	// validate
	FILE_LOG(logINFO) << "\tValidating Pressure Gauge";
	try {
		std::string result = PressureGaugeSend(PRESSURE_ARE_YOU_THERE_CMD);
		if (result.find(PRESSURE_TYPE_RESPONSE) == std::string::npos) {
			std::ostringstream oss;
			oss << "Unknown pressure gauge status " + result;
			FILE_LOG(logERROR) << oss.str();
			PressureGaugeSendCommand("RES\r\n");
			throw std::runtime_error(oss.str());
		}
	} catch (...) {
			close(serialfd);
			FILE_LOG(logWARNING) << "Fail";
			throw;
	}
	FILE_LOG(logINFOGREEN) << "\tSuccess";
}

std::string Interface::PressureGaugeSend(std::string command) {
	int attempts = 0;
	for (;;) {
        try {
            std::string result = PressureGaugeSendCommand(command);
			return result;
        } catch (const std::exception& e) {
			++attempts;
			if (attempts == PRESSURE_MAX_REPEAT_ATTEMPTS) {
				std::ostringstream oss;
				oss << "Pressure Gauge is probably switched off. " << e.what();
				throw PressureOffError(oss.str());
			}
        }	
		usleep(PRESSURE_READ_WAIT_US);	
	}
}

std::string Interface::PressureGaugeSendCommand(std::string command) {
	bool verbose = false;
	FILE_LOG(logINFO) << "\tSend Gauge [" << command.substr(0, command.length() - 2) << "\\r\\n]:\t(port:" << serial << ')';

	// send command
	char buffer[COMMAND_BUFFER_LENGTH];
	memset(buffer, 0, sizeof(buffer));
	strcpy(buffer, command.c_str());	
	int ret = write (serialfd, buffer, strlen(buffer));
	if (ret == -1) {
		std::ostringstream oss;
		oss << "Could not write command [" << buffer << "] to pressure gauge";
		if (verbose) {
			FILE_LOG(logERROR) << oss.str();
		}
		throw std::runtime_error(oss.str());
	}

	// read acknowledge
	char result[COMMAND_BUFFER_LENGTH];
	memset(result, 0, sizeof(result));	
	usleep(PRESSURE_READ_WAIT_US);
	ret = read (serialfd, result, sizeof(result));
	if (ret == -1) {
		std::ostringstream oss;
		oss << "Could not read ack status from pressure gauge";
		if (verbose) {
			FILE_LOG(logERROR) << oss.str();
		}
		throw std::runtime_error(oss.str());		
	}

	// validate ack
	// length
	if (strlen(result) == 0) {
		std::ostringstream oss;
		oss << "Could not read ack status from pressure gauge. Empty buffer";
		if (verbose) {
			FILE_LOG(logERROR) << oss.str();
		}
		throw std::runtime_error(oss.str());
	}
	// value
	if (strncmp(result, PRESSURE_ACK_RESPONSE, strlen(PRESSURE_ACK_RESPONSE))) {
		std::ostringstream oss;
		oss << "Could not read acknowledge. Read ";
		if (!strncmp(result, PRESSURE_NACK_RESPONSE, strlen(PRESSURE_NACK_RESPONSE))) {
			oss << "negative acknowledge";
		} else {
			for (int i = 0; i < strlen(result); ++i) {
				oss << std::hex << result[i] << std::dec;
			}
		}
		if (verbose) {
			FILE_LOG(logERROR) << oss.str();
		}
		throw std::runtime_error(oss.str());		
	}
	FILE_LOG(logDEBUG) << "\tAck rxd";

	// send enquiry
	memset(buffer, 0, sizeof(buffer));
	strcpy(buffer, PRESSURE_ENQUIRY_CMD);
	ret = write (serialfd, buffer, strlen(buffer));
	if (ret == -1) {
		std::ostringstream oss;
		oss << "Could not write enquiry [" << buffer << "] to pressure gauge";
		if (verbose) {
			FILE_LOG(logERROR) << oss.str();
		}
		throw std::runtime_error(oss.str());
	}	
	FILE_LOG(logDEBUG) << "\tSent enquiry";

	// read back
	int attempt = 0;
		memset(result, 0, sizeof(result));	
	while (ret == -1 || strlen(result) < 5) {
		memset(result, 0, sizeof(result));	
		usleep(PRESSURE_READ_WAIT_US);
		ret = read (serialfd, result, sizeof(result));
		++attempt;
		if (attempt == PRESSURE_MAX_RX_ATTEMPTS) {
			std::ostringstream oss;
			oss << "Receive attempt number " << PRESSURE_MAX_RX_ATTEMPTS << " for [" << buffer << "] to pressure gauge " << serial << ". Aborting read.";
			if (verbose) {
				FILE_LOG(logERROR) << oss.str();
			}
			throw std::runtime_error(oss.str());
		}
	}
	if (attempt > 0) {
		FILE_LOG(logDEBUG) << "receive attempt " << attempt;
	}	

	// throw error if read buffer empty
	if (strlen(result) == 0) {
		std::ostringstream oss;
		oss << "Could not read pressure gauge. Empty buffer";
		if (verbose) {
			FILE_LOG(logERROR) << oss.str();
		}
		throw std::runtime_error(oss.str());
	}

	FILE_LOG(logINFO) << "\tRead Gauge [" << command.substr(0, command.length() - 2) << "\\r\\n]:\t" << result;
	return std::string(result);
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

    FILE_LOG(logINFOGREEN) << "Success";
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

