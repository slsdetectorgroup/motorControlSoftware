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
#define CONTROLLER_READ_WAIT_US			(50 * 1000) // because of raspberry pi
#define CONTROLLER_IDENTIFY_CMD			("identify")
#define CONTROLLER_MODEL_RESPONSE		("Corvus Eco")
#define CONTROLLER_CHECK_STATUS_CMD		("st ")
#define CONTROLLER_POSITION_CMD			("pos ")

#define TUBE_MAX_RX_ATTEMPTS			(10)
#define TUBE_MAX_REPEAT_ATTEMPTS		(10)
#define TUBE_READ_WAIT_US				(200 * 1000)
#define TUBE_IDENTIFY_CMD				("id ")
#define TUBE_MODEL_RESPONSE				("ISO-DEBYEFLEX 3003")
#define TUBE_STATUS_CMD					("sr:12 ")

#define PRESSURE_MAX_RX_ATTEMPTS		(5)
#define PRESSURE_MAX_REPEAT_ATTEMPTS	(5)
#define PRESSURE_READ_WAIT_US			(200 * 1000)
#define PRESSURE_ARE_YOU_THERE_CMD		("AYT\r\n")
#define PRESSURE_TYPE_RESPONSE			("TPG362")
#define PRESSURE_ACK_RESPONSE			("\x6\r\n")
#define PRESSURE_NACK_RESPONSE			("\x15\r\n")
#define PRESSURE_ENQUIRY_CMD			("\x5")

#define FILTER_WHEEL_MAX_RX_ATTEMPTS	(10)			// minimum 2
#define FILTER_WHEEL_MAX_REPEAT_ATTEMPTS (5)			
#define FILTER_WHEEL_READ_WAIT_US		(500 * 1000)	// minimum (else pos=1 gets lost)
#define FILTER_WHEEL_IDENTIFY_CMD		("*idn?\r")
#define FILTER_WHEEL_MODEL_RESPONSE		("FW102C/FW212C Filter Wheel")
#define FILTER_WHEEL_UNDEFINED_CMD		("Command error")


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
	std::cout << std::endl;
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
		std::string result = ControllerSendCommand(CONTROLLER_IDENTIFY_CMD, true);
		if (result.find(std::string(CONTROLLER_MODEL_RESPONSE)) == std::string::npos) {
			throw std::runtime_error("Not identified as controller port");
		}
	} catch (...) {
		close(serialfd);
		FILE_LOG(logWARNING) << "Fail";
		throw;
	}

	FILE_LOG(logINFO) << "\tFound controller port";
	ControllerWaitForIdle();
}

bool Interface::ControllerIsIdle(std::string result) {
	std::istringstream iss(result);
	int status = 1;
	iss >> status;
	if (iss.fail()) {
		std::ostringstream oss;
		oss << "Unknown controller status " + result;
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
            return ControllerSendCommand(command, readBack);
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
	std::cout << std::endl;
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
		std::string result = TubeSend(TUBE_IDENTIFY_CMD, true);
		if (result.find(std::string(TUBE_MODEL_RESPONSE)) == std::string::npos) {
			throw std::runtime_error("Not identified as tube port");
		}
		TubeSend(TUBE_STATUS_CMD, true);
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
				oss << TUBE_OFF_ERROR_PHRASE << ". " << e.what();
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
	std::cout << std::endl;
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
            return  PressureGaugeSendCommand(command);
        } catch (const std::exception& e) {
			++attempts;
			if (attempts == PRESSURE_MAX_REPEAT_ATTEMPTS) {
				std::ostringstream oss;
				oss << PRESSUR_OFF_ERROR_PHRASE << ". " << e.what();
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
			for (size_t i = 0; i < strlen(result); ++i) {
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
	std::cout << std::endl;
	FILE_LOG(logINFO) << "\tFilter wheel, checking:" << serial;
	serialfd = open (serial.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
	if (serialfd == -1) {
		FILE_LOG(logDEBUG) << "Unable to open port " << serial << " for filter wheel";
        FILE_LOG(logWARNING) << "Fail";
		throw std::runtime_error("Fail to open port");
	}

	struct termios new_serial_conf;
	memset(&new_serial_conf, 0, sizeof(new_serial_conf));
	new_serial_conf.c_cflag = B115200 | CS8 | CREAD | CLOCAL; // control options
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

	ValidateFilterWheel();
}

void Interface::ValidateFilterWheel() { 
	FILE_LOG(logINFO) << "\tValidating Filter Wheel";
	int attempts = 0;
	try {
		std::string result = FilterWheelSendCommand(FILTER_WHEEL_IDENTIFY_CMD, true);
		if (result.find(std::string(FILTER_WHEEL_MODEL_RESPONSE)) == std::string::npos) {
			throw std::runtime_error("Not identified as filter wheel port");
		}
	} catch (const std::exception& e) {
		++attempts;
		if (attempts == FILTER_WHEEL_MAX_REPEAT_ATTEMPTS) {
			close(serialfd);
			FILE_LOG(logWARNING) << "Fail";
			throw;
		}
	}
	FILE_LOG(logINFO) << "\tFound filter wheel port";
}

std::string Interface::FilterWheelSend(std::string command, bool readBack) {
	for (;;) {
        try {
            return FilterWheelSendCommand(command, readBack);
        } catch (const std::exception& e) {
			;
        }	
		usleep(FILTER_WHEEL_READ_WAIT_US);	
	}
}


std::string Interface::FilterWheelSendCommand(std::string command, bool readBack) {
	FILE_LOG(logINFO) << "\tSend Filter Wheel [" << command.substr(0, command.length() - 1) << "]:\t(port:" << serial << ", read:" << readBack << ')';
	
	bool verbose = false;

	// send command
	char buffer[COMMAND_BUFFER_LENGTH];
	memset(buffer, 0, sizeof(buffer));
	strcpy(buffer, command.c_str());	

	int ret = write (serialfd, buffer, strlen(buffer));
	if (ret == -1) {
		std::ostringstream oss;
		oss << "Could not write to filter wheel";
		if (verbose) {
			FILE_LOG(logERROR) << oss.str();
		}
		throw std::runtime_error(oss.str());
	}

	// read back
	char result[COMMAND_BUFFER_LENGTH];
	ret = -1;
	int attempt = 0;
	// reads atlease twice
	while (ret == -1) {
		memset(result, 0, sizeof(result));	
		usleep(FILTER_WHEEL_READ_WAIT_US);
		ret = read (serialfd, result, sizeof(result));
		++attempt;
		if (attempt == FILTER_WHEEL_MAX_RX_ATTEMPTS) {
			std::ostringstream oss;
			oss << "Receive attempt number " << FILTER_WHEEL_MAX_RX_ATTEMPTS << " for [" << buffer << "] to filter wheel " << serial << ". Aborting read.";
			if (verbose) {
				FILE_LOG(logERROR) << oss.str();
			}
			throw std::runtime_error(oss.str());
		}
	}
	if (attempt > 0 && verbose) {
		FILE_LOG(logINFORED) << "receive attempt " << attempt;
	}			

	// throw error if read buffer empty
	if (strlen(result) == 0) {
		std::ostringstream oss;
		oss << "Empty receive buffer from filter wheel";
		if (verbose) {
			FILE_LOG(logERROR) << oss.str();
		}
		throw std::runtime_error(oss.str());
	}

	// error
	if (strstr(result, FILTER_WHEEL_UNDEFINED_CMD) != NULL) {
		for (size_t i = 0; i < strlen(result); ++i) {
			if (result[i] == '\r') {
				result[i] = 'R';
			} else if (result[i] == '\n') {
				result[i] = 'N';
			}
		}
		std::ostringstream oss;
		oss << "Filter wheel returns unknown command or argument: " << result;
		if (verbose) {
			FILE_LOG(logERROR) << oss.str();
		}
		throw std::runtime_error(oss.str());
	}

	// no need to parse result (just read it off the buffer)
	if (!readBack) {
		for (size_t i = 0; i < strlen(result); ++i) {
			if (result[i] == '\r') {
				result[i] = 'R';
			} else if (result[i] == '\n') {
				result[i] = 'N';
			}
		}
		FILE_LOG(logINFO) << "\tRead Filter Wheel [" << command.substr(0, command.length() - 1) << "]: [" << result << ']';
		return command;
	}

	// extract result
	// if segfault here, it needs more time to read (increase FILTER_WHEEL_READ_WAIT_US)
	char* firstCR = strchr(result,'\r');
	if (firstCR == NULL) {
		throw RuntimeError("Could not scan first \r in result");
		//throw std::runtime_error(oss.str());
	}
	char* secondCR = strchr(firstCR + 1,'\r');
	if (secondCR == NULL) {
		throw RuntimeError("Could not scan second \r in result");
		//throw std::runtime_error(oss.str());
	}
	char output[COMMAND_BUFFER_LENGTH];
	memset(output, 0, sizeof(output));
	memcpy(output, firstCR + 1, secondCR - firstCR);
	output[strlen(output) - 1] = '\0';

	result[strlen(result) - 1] = '\0';
	FILE_LOG(logINFO) << "\tRead Filter Wheel [" << command.substr(0, command.length() - 1) << "]: [" << output << ']';
	return std::string(output);
}
