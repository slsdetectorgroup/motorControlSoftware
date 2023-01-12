#pragma once

#include "logger.h"

#include <iostream>
#include <stdexcept>
#include <stdint.h>

#if defined(VIRTUAL) || defined(GENERIC)
#define DEFAULT_SERVER_HOSTNAME ("localhost")
#elif defined(XRAYBOX)
#define DEFAULT_SERVER_HOSTNAME ("mpc2128.psi.ch")
#elif defined(LASERBOX)
#define DEFAULT_SERVER_HOSTNAME ("mpc2085.psi.ch")
#elif defined(VACUUMBOX)
#define DEFAULT_SERVER_HOSTNAME ("mpc2069.psi.ch")
#endif
#define PORT_NO (1111)

#define MAX_STR_LENGTH        (1000)
#define COMMAND_BUFFER_LENGTH (256)
#define TCP_PACKET_LENGTH     (1024)
#define TIME_BUFFER_LENGTH    (28)

#define FWHEEL_TOLERANCE (0.0001)
#define MAX_FLUOR_VALUES (8)

enum {
    OK,     /**< function successfully executed */
    FAIL,   /**< error occurred */
    GOODBYE /**< stop */
};

struct PressureGauge {
    std::string status;
    std::string pressure;
};

struct RuntimeError : public std::runtime_error {
    RuntimeError() : runtime_error("Motor Control Software Failed") {
        LOG(logERROR) << "Motor Control Software Failed";
    }
    RuntimeError(std::string msg) : runtime_error(msg) { LOG(logERROR) << msg; }
    RuntimeError(const char *msg) : runtime_error(msg) { LOG(logERROR) << msg; }
};

struct TubeOffError : public RuntimeError {
  public:
    TubeOffError(std::string msg) : RuntimeError(msg) {}
};

struct TubeStandbyError : public RuntimeError {
  public:
    TubeStandbyError(std::string msg) : RuntimeError(msg) {}
};

struct PressureOffError : public RuntimeError {
  public:
    PressureOffError(std::string msg) : RuntimeError(msg) {}
};

struct BoxInUseError : public RuntimeError {
  public:
    BoxInUseError(std::string msg) : RuntimeError(msg) {}
};

struct AnotherUserError : public RuntimeError {
  public:
    AnotherUserError(std::string msg) : RuntimeError(msg) {}
};

#define TUBE_OFF_ERROR_PHRASE     ("Tube is probably switched off")
#define TUBE_STANDBY_ERROR_PHRASE ("Stand-by")
#define PRESSUR_OFF_ERROR_PHRASE  ("Pressure Gauge is probably switched off")
#define BOX_IN_USE_ERROR_PHRASE   ("box is in use by")
#define ANOTHER_USER_ERROR_PHRASE ("Another user had updated the server")
