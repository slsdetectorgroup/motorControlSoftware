#pragma once

#include "logger.h"

#include <stdint.h> 
#include <iostream>
#include <stdexcept>


#define MAX_STR_LENGTH 1000
#define PORT_NO 1111

#ifdef XRAYBOX
#define DEFAULT_SERVER_HOSTNAME "mpc2128.psi.ch"
#elif LASERBOX
#define DEFAULT_SERVER_HOSTNAME "mpc2085.psi.ch"
#else
#define DEFAULT_SERVER_HOSTNAME "mpc2069.psi.ch"
#endif

#define MAX_USB_SERIAL_PORTS 			(10)
#define USB_PORT_PREFIX					("/dev/ttyUSB")

#define MAX_NUM_MOTORS_PER_CONTROLLER 	(3)

#define COMMAND_BUFFER_LENGTH			(256)
#define TCP_PACKET_LENGTH				(1024)
#define TIME_BUFFER_LENGTH				(28)

#define CONFIG_FILE "config.txt"
#define WARMUP_FILE "warmupTimestamps.txt"

enum {
  OK, /**< function successfully executed */
  FAIL,  /**< error occurred */
  GOODBYE  /**< stop */
};


struct RuntimeError : public std::runtime_error {
	RuntimeError(): runtime_error("Motor Control Software Failed") {
		FILE_LOG(logERROR) << "Motor Control Software Failed";
	}
  RuntimeError(std::string msg): runtime_error(msg) {
		FILE_LOG(logERROR) << msg;
	}
	RuntimeError(const char* msg): runtime_error(msg) {
		FILE_LOG(logERROR) << msg;
	}
};



