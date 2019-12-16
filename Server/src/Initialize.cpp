#include "Initialize.h"
#include "Xray.h"
#include "Pgauge.h"
#include "Fwheel.h"
#include "ReferencePoints.h"
#include "Fluorescence.h"
#include "Slit.h"
#include "Controller.h"
#include "Motor.h"
#include "commonDefs.h"


#include <sstream>
#include <fstream>
#include <iomanip>
#include <math.h>
#include <time.h>
#include <cstdlib> //exit
#include <cstring> //strncpy
#include "commonDefs.h"
#include <unistd.h> // (usleep,etc.) in raspberrypi
#include <iterator>
using namespace std;

#define CONFIG_FILE 				("config.txt")
#define CONFIG_POWER_NUM_ARGS       (2)
#define CONFIG_FWHEEL_NUM_ARGS      (9)
#define CONFIG_REF_POINT_NUM_ARGS	(5)
#define CONFIG_FLUOR_NUM_ARGS		(3)
#define CONFIG_CONTROLLER_NUM_ARGS  (3)
#define CONFIG_MOTOR_NUM_ARGS       (6)

#define MAX_USB_SERIAL_PORTS 			(10)
#define USB_PORT_PREFIX					("/dev/ttyUSB")


void Initialize::OnlyTubeCommand() {
	if (xrayTube == NULL) {
		try {
			UpdateInterface(TUBE);
		} catch (RuntimeError &e) {
			throw;
		}	
		xrayTube->setMaxPower(maxTubePower);
	}
}

void Initialize::OnlyFluorescenceCommand(string name) {
	if ((strcasecmp(name.c_str(), "fluorescence")) && (strcasecmp(name.c_str(), "fluorescence_wheel"))) {
		throw RuntimeError("Command not allowed for this motor " + name);
	}
}

void Initialize::RestrictedForSlitAndFluorescence(string name) {
	if ((!strcasecmp(name.c_str(), "slit_x1")) || (!strcasecmp(name.c_str(), "slit_x2")) || (!strcasecmp(name.c_str(), "fluorescence")) || (!strcasecmp(name.c_str(), "fluorescence_wheel"))) {
		throw RuntimeError("Command not allowed for this motor " + name);
	}
}

void Initialize::UpdateSlitLimits(string name) {
	// empty name (depos command)
	if (name.empty()) {
		if (slit != NULL) {
			slit->updateLimits();
		}
	}  
	// validate that slit object should exist when using it in the command
	else if ((!strcasecmp(name.c_str(), "slit_x1")) || (!strcasecmp(name.c_str(), "slit_x2"))) {
		if (slit == NULL) {
			throw RuntimeError("No slit object created");
		} else {
			slit->updateLimits();
		}
	}
}

int Initialize::GetFluorescenceIndex(string name) {
	for (unsigned int i = 0; i < fluorescence.size(); ++i) {
		if (!strcasecmp(name.c_str(), fluorescence[i]->getName().c_str())) {
			return i;
		}
	}
	throw RuntimeError("Unknown fluorescence motor " + name);
}

int Initialize::GetMotorIndex(string name) {
	for (unsigned int i = 0; i < motor.size(); ++i) {
		if (!strcasecmp(name.c_str(), motor[i]->getName().c_str())) {
			return i;
		}
	}
	throw RuntimeError("Unknown motor " + name);
}

int Initialize::GetControllerIndex(string name) {
	for (unsigned int i = 0; i < controller.size(); ++i) {
		if (!strcasecmp(name.c_str(), controller[i]->getName().c_str())) {
			return i;
		}
	}
	throw RuntimeError("Unknown controller " + name);
}

string Initialize::executeCommand(vector<string> args) {
	string command = args[0];
	int nArg = (int)args.size();
	ostringstream oss;

/*
	for (int i = 0; i < nArg; ++i) {
		FILE_LOG(logINFO) << i << ":" << args[i];
	}
*/
	try {

		// ----- direct controller advanced --------------------------------------------------------

		if (!strcasecmp(command.c_str(), "sendcontroller")) {
			if (nArg < 3) {
				throw RuntimeError("Requires 3 parameters: sendcontroller [corvus] [commands..]");
			}
			string name = args[1];
			int icontroller = GetControllerIndex(name);
			string advancedCommand;
			for (int j = 2; j < nArg; ++j) {
				advancedCommand += (args[j] + " ");
			}
			controller[icontroller]->sendCommand(advancedCommand);
			return "ok";
		}

		else if (!strcasecmp(command.c_str(), "readcontroller")) {
			if (nArg < 3) {
				throw RuntimeError("Requires 3 parameters: readcontroller [corvus] [commands..]");
			}
			string name = args[1];
			int icontroller = GetControllerIndex(name);
			string advancedCommand;
			for (int j = 2; j < nArg; ++j) {
				advancedCommand += (args[j] + " ");
			}
			return controller[icontroller]->sendCommandAndReadBack(advancedCommand);
		}


		// ----- motor --------------------------------------------------------

		else if (!strcasecmp(command.c_str(), "listmotors")) {
			if (nArg != 1 ) {
				throw RuntimeError("Requires 1 parameters: list");
			}	
			oss << motor.size() << ' ';
			for(unsigned int i = 0; i < motor.size(); ++i) {
				oss << motor[i]->getName() << ' ';
			}
			return oss.str();
		}

		else if (!strcasecmp(command.c_str(), "getcontroller")) {
			if (nArg != 2) {
				throw RuntimeError("Requires 2 parameters: getcontroller [motor]");
			}
			string name = args[1];
			int imotor = GetMotorIndex(name);
			return controller[motor[imotor]->getController()]->getName();
		}

		else if (!strcasecmp(command.c_str(), "getaxis")) {
			if (nArg != 2) {
				throw RuntimeError("Requires 2 parameters: getaxis [motor]");
			}
			string name = args[1];
			int imotor = GetMotorIndex(name);
			oss << motor[imotor]->getAxis() + 1;
			return oss.str();
		}

		else if (!strcasecmp(command.c_str(), "pos")) {
			if (nArg != 2) {
				throw RuntimeError("Requires 2 parameters: pos [motor]");
			}
			string name = args[1];
			int imotor = GetMotorIndex(name);
			oss << fixed << motor[imotor]->getPosition();
			return oss.str();
		}

		else if (!strcasecmp(command.c_str(), "depos")) {
			if (nArg != 2) {
				throw RuntimeError("Requires 2 parameters: pos [controller]");
			}
			string name = args[1];
			int icontroller = GetControllerIndex(name);
			controller[icontroller]->debugPositions();
			UpdateSlitLimits("");
			return "ok";
		}
		
		else if (!strcasecmp(command.c_str(), "getupper")) {
			if (nArg != 2) {
				throw RuntimeError("Requires 2 parameters: getUpper [motor]");
			}
			string name = args[1];
			int imotor = GetMotorIndex(name);
			oss << motor[imotor]->getUpperLimit();
			return oss.str();
		}

		else if (!strcasecmp(command.c_str(), "setupper")) {
			if (nArg != 3) {
				throw RuntimeError("Requires 3 parameters: setUpper [motor] [limit]");
			}
			string name = args[1];
			int imotor = GetMotorIndex(name);
			RestrictedForSlitAndFluorescence(name);
			double value = 0;
			istringstream iss (args[2].c_str());
			iss >> value;
			if (iss.fail()) {
				throw RuntimeError("Could not scan limit argument " + args[2]);
			}		

			motor[imotor]->setUpperLimit(value);
			oss << value;
			return oss.str();
		}

		else if (!strcasecmp(command.c_str(), "getLower")) {
			if (nArg != 2) {
				throw RuntimeError("Requires 2 parameters: getLower [motor]");
			}
			string name = args[1];
			int imotor = GetMotorIndex(name);
			oss << motor[imotor]->getLowerLimit();
			return oss.str();
		}

		else if (!strcasecmp(command.c_str(), "setLower")) {
			if (nArg != 3) {
				throw RuntimeError("Requires 3 parameters: setLower [motor] [limit]");
			}
			string name = args[1];
			int imotor = GetMotorIndex(name);
			RestrictedForSlitAndFluorescence(name);
			double value = 0;
			istringstream iss (args[2].c_str());
			iss >> value;
			if (iss.fail()) {
				throw RuntimeError("Could not scan limit argument " + args[2]);
			}		

			motor[imotor]->setLowerLimit(value);
			oss << value;
			return oss.str();
		}

		else if (!strcasecmp(command.c_str(), "cal")) {
			if (nArg != 2) {
				throw RuntimeError("Requires 2 parameters: cal [motor] ");
			}
			string name = args[1];
			int imotor = GetMotorIndex(name);
			controller[motor[imotor]->getController()]->calibrate(motor[imotor]->getAxis());
			UpdateSlitLimits(name);
			oss << fixed << motor[imotor]->getPosition();
			return oss.str();
		}

		else if (!strcasecmp(command.c_str(), "rangemeasure")) {
			if (nArg != 2) {
				throw RuntimeError("Requires 2 parameters: rangemeasure [motor] ");
			}
			string name = args[1];
			int imotor = GetMotorIndex(name);
			RestrictedForSlitAndFluorescence(name);
			controller[motor[imotor]->getController()]->rangeMeasure(motor[imotor]->getAxis());
			UpdateSlitLimits(name);
			oss << fixed << motor[imotor]->getPosition();
			return oss.str();
		}
		
		else if (!strcasecmp(command.c_str(), "moverel")) {
			if (nArg != 3) {
				throw RuntimeError("Requires 3 parameters: moverel [motor] [value]");
			}
			string name = args[1];
			int imotor = GetMotorIndex(name);
			double value = 0;
			istringstream iss (args[2].c_str());
			iss >> value;
			if (iss.fail()) {
				throw RuntimeError("Could not scan relative position argument " + args[2]);
			}	

			int icontroller = motor[imotor]->getController();
			int iaxis = motor[imotor]->getAxis();
			controller[icontroller]->moveRel(value, iaxis);
			UpdateSlitLimits(name);
			oss << fixed << motor[imotor]->getPosition();
			return oss.str();
		}

		else if (!strcasecmp(command.c_str(), "moveabs")) {
			if (nArg != 3) {
				throw RuntimeError("Requires 3 parameters: moveabs [motor] [value]");
			}
			string name = args[1];
			int imotor = GetMotorIndex(name);
			double value = 0;
			istringstream iss (args[2].c_str());
			iss >> value;
			if (iss.fail()) {
				throw RuntimeError("Could not scan absolute position argument " + args[2]);
			}	

			int icontroller = motor[imotor]->getController();
			int iaxis = motor[imotor]->getAxis();
			controller[icontroller]->moveAbs(value, iaxis);
			UpdateSlitLimits(name);
			oss << fixed << motor[imotor]->getPosition();
			return oss.str();
		}

		else if (!strcasecmp(command.c_str(), "setpos")) {
			if (nArg != 3) {
				throw RuntimeError("Requires 3 parameters: setpos [motor] [value]");
			}
			string name = args[1];
			int imotor = GetMotorIndex(name);
			RestrictedForSlitAndFluorescence(name);
			double value = 0;
			istringstream iss (args[2].c_str());
			iss >> value;
			if (iss.fail()) {
				throw RuntimeError("Could not scan position argument " + args[2]);
			}	

			int icontroller = motor[imotor]->getController();
			int iaxis = motor[imotor]->getAxis();
			controller[icontroller]->resetPosition(value, iaxis);
			oss << fixed << motor[imotor]->getPosition();
			return oss.str();
		}


		// ----- flourescence -------------------------------------------------

		else if (!strcasecmp(command.c_str(), "numflist")) {
			if (nArg != 2) {
				throw RuntimeError("Requires 2 parameters: numflist [fluorescence motor]");
			}
			string name = args[1];
			OnlyFluorescenceCommand(name);
			int ifluor = GetFluorescenceIndex(name);
			oss << fluorescence[ifluor]->getNumTargetHolders();
			return oss.str();
		}

		else if (!strcasecmp(command.c_str(), "whichflist")) {
			if (nArg != 2) {
				throw RuntimeError("Requires 2 parameters: whichflist [fluorescence motor]");
			}
			string name = args[1];
			OnlyFluorescenceCommand(name);
			int ifluor = GetFluorescenceIndex(name);
			oss << fluorescence[ifluor]->getCurrentTargetHolder();
			return oss.str();
		}

		else if (!strcasecmp(command.c_str(), "loadflist")) {
			if (nArg != 3) {
				throw RuntimeError("Requires 3 parameters: loadflist [fluorescence motor] [index]");
			}
			string name = args[1];
			OnlyFluorescenceCommand(name);
			int ifluor = GetFluorescenceIndex(name);
			int value = 0;
			istringstream iss (args[2].c_str());
			iss >> value;
			if (iss.fail()) {
				throw RuntimeError("Could not scan index argument " + args[2]);
			}		
			fluorescence[ifluor]->setCurrentTargetHolder(value);
			oss << value;
			return oss.str();
		}

		else if (!strcasecmp(command.c_str(), "fllist")) {
			if (nArg != 2) {
				throw RuntimeError("Requires 2 parameters: fllist [fluorescence motor]");
			}
			string name = args[1];
			OnlyFluorescenceCommand(name);
			int ifluor = GetFluorescenceIndex(name);
			return fluorescence[ifluor]->getList();
		}	

		else if (!strcasecmp(command.c_str(), "getfl")) {
			if (nArg != 2) {
				throw RuntimeError("Requires 2 parameters: getfl [fluorescence motor]");
			}
			string name = args[1];
			OnlyFluorescenceCommand(name);
			int ifluor = GetFluorescenceIndex(name);
			oss << fluorescence[ifluor]->updateAndGetCurrentTargetName(); 
			return oss.str();
		}		

		else if (!strcasecmp(command.c_str(), "movefl")) {
			if (nArg != 3) {
				throw RuntimeError("Requires 2 parameters: movefl [fluorescence motor] [target name]");
			}
			string name = args[1];
			OnlyFluorescenceCommand(name);
			int ifluor = GetFluorescenceIndex(name);
			string target = args[2];
			fluorescence[ifluor]->moveToTarget(target);
			oss << fluorescence[ifluor]->updateAndGetCurrentTargetName(); 
			return oss.str();
		}


		// ----- tube ----------------------------------------------------------

		if (!strcasecmp(command.c_str(), "sendtube")) {
			if (nArg < 2) {
				throw RuntimeError("Requires 2 parameters: sendtube [commands..]");
			}
			string advancedCommand;
			for (int j = 1; j < nArg; ++j) {
				advancedCommand += (args[j] + " ");
			}
			OnlyTubeCommand();
			xrayTube->sendCommand(advancedCommand);
			return "ok";
		}

		else if (!strcasecmp(command.c_str(), "readtube")) {
			if (nArg < 2) {
				throw RuntimeError("Requires 2 parameters: readtube [commands..]");
			}
			string advancedCommand;
			for (int j = 1; j < nArg; ++j) {
				advancedCommand += (args[j] + " ");
			}
			OnlyTubeCommand();
			return xrayTube->sendCommandAndReadBack(advancedCommand);
		}

		else if (!strcasecmp(command.c_str(), "getpower")) {
			if (nArg != 1) {
				throw RuntimeError("Requires 1 parameters: getpower ");
			}
			OnlyTubeCommand();
			oss << xrayTube->getMaxPower() << " W";
			return oss.str();
		}

		else if (!strcasecmp(command.c_str(), "geterr")) {
			if (nArg != 1) {
				throw RuntimeError("Requires 1 parameters: geterr ");
			}
			OnlyTubeCommand();
			oss << xrayTube->getErrorCode();
			return oss.str();
		}

		else if (!strcasecmp(command.c_str(), "getemess")) {
			if (nArg != 1) {
				throw RuntimeError("Requires 1 parameters: getemess ");
			}
			OnlyTubeCommand();
			oss << xrayTube->getErrorMessage();
			return oss.str();
		}

		else if (!strcasecmp(command.c_str(), "clear")) {
			if (nArg != 1) {
				throw RuntimeError("Requires 1 parameters: clear ");
			}
			OnlyTubeCommand();
			xrayTube->clearErrorCode();
			oss << xrayTube->getErrorCode();
			return oss.str();
		}

		else if (!strcasecmp(command.c_str(), "isstandby")) {
			if (nArg != 1) {
				throw RuntimeError("Requires 1 parameters: isstandby ");
			}
			oss << (xrayTube->isOnStandby() ? "standby" : "on");
			return oss.str();
		}

		else if (!strcasecmp(command.c_str(), "gethv")) {
			if (nArg != 1) {
				throw RuntimeError("Requires 1 parameters: gethv ");
			}
			OnlyTubeCommand();
			oss << (xrayTube->getHVSwitch() ? "on" : "off");
			return oss.str();
		}
		
		else if (!strcasecmp(command.c_str(), "hv")) {
			if (nArg != 2) {
				throw RuntimeError("Requires 2 parameters: hv [on/off]");
			}
			OnlyTubeCommand();
			string value = args[1];
			bool on = false;
			if (!strcasecmp(value.c_str(), "on")) {
				on = true;
			} else if (!strcasecmp(value.c_str(), "off")) {
				on = false;
			} else {
				throw RuntimeError ("Cannot scan hv argument " + value);
			}
			xrayTube->setHVSwitch(on);
			oss << (xrayTube->getHVSwitch() ? "on" : "off");
			return oss.str();
		}

		else if (!strcasecmp(command.c_str(), "getv")) {
			if (nArg != 1) {
				throw RuntimeError("Requires 1 parameters: getv ");
			}
			OnlyTubeCommand();
			oss << xrayTube->getVoltage() << " kV";
			return oss.str();
		}
	
		else if (!strcasecmp(command.c_str(), "setv")) {
			if (nArg != 2) {
				throw RuntimeError("Requires 2 parameters: setv [voltage] ");
			}
			OnlyTubeCommand();
			int value = 0;
			istringstream iss (args[1].c_str());
			iss >> value;
			if (iss.fail()) {
				throw RuntimeError("Could not scan voltage argument " + args[1]);
			}	
			xrayTube->setVoltage(value);
			oss << xrayTube->getVoltage() << " kV";
			return oss.str();
		}

		else if (!strcasecmp(command.c_str(), "getc")) {
			if (nArg != 1) {
				throw RuntimeError("Requires 1 parameters: getc ");
			}
			OnlyTubeCommand();
			oss << xrayTube->getCurrent() << " mA";
			return oss.str();
		}
	
		else if (!strcasecmp(command.c_str(), "setc")) {
			if (nArg != 2) {
				throw RuntimeError("Requires 2 parameters: setc [current] ");
			}
			OnlyTubeCommand();
			int value = 0;
			istringstream iss (args[1].c_str());
			iss >> value;
			if (iss.fail()) {
				throw RuntimeError("Could not scan current argument " + args[1]);
			}	
			xrayTube->setCurrent(value);
			oss << xrayTube->getCurrent() << " mA";
			return oss.str();
		}

		else if (!strcasecmp(command.c_str(), "setvc")) {
			if (nArg != 3) {
				throw RuntimeError("Requires 3 parameters: setvc [voltage] [current]");
			}
			OnlyTubeCommand();
			int voltage = 0;
			{
				istringstream iss (args[1].c_str());
				iss >> voltage;
				if (iss.fail()) {
					throw RuntimeError("Could not scan voltage argument " + args[1]);
				}
			}
			int current = 0;
			{
				istringstream iss (args[2].c_str());
				iss >> current;
				if (iss.fail()) {
					throw RuntimeError("Could not scan current argument " + args[2]);
				}			
			}
			xrayTube->setVoltageAndCurrent(voltage, current);
			pair <int, int> values = xrayTube->getVoltageAndCurrent();
			oss << values.first << " kV, " << values.second << " mA";
			return oss.str();
		}

		else if (!strcasecmp(command.c_str(), "getvc")) {
			if (nArg != 1) {
				throw RuntimeError("Requires 1 parameters: getvc ");
			}
			OnlyTubeCommand();
			pair <int, int> values = xrayTube->getVoltageAndCurrent();
			oss << values.first << " kV, " << values.second << " mA";
			return oss.str();
		}

		else if (!strcasecmp(command.c_str(), "getactualv")) {
			if (nArg != 1) {
				throw RuntimeError("Requires 1 parameters: getactualv ");
			}
			OnlyTubeCommand();
			oss << xrayTube->getActualVoltage() << " kV";
			return oss.str();
		}

		else if (!strcasecmp(command.c_str(), "getactualc")) {
			if (nArg != 1) {
				throw RuntimeError("Requires 1 parameters: getactualc ");
			}
			OnlyTubeCommand();
			oss << xrayTube->getActualCurrent() << " mA";
			return oss.str();
		}

		else if (!strcasecmp(command.c_str(), "getactualvc")) {
			if (nArg != 1) {
				throw RuntimeError("Requires 1 parameters: getactualvc ");
			}
			OnlyTubeCommand();
			pair <int, int> values = xrayTube->getActualVoltageAndCurrent();
			oss << values.first << " kV, " << values.second << " mA";
			return oss.str();
		}

		else if (!strcasecmp(command.c_str(), "getshutter")) {
			if (nArg != 2) {
				throw RuntimeError("Requires 2 parameters: getshutter [shutter index] ");
			}
			OnlyTubeCommand();
			int index = 0;
			istringstream iss (args[1].c_str());
			iss >> index;
			if (iss.fail()) {
				throw RuntimeError("Could not scan shutter index argument " + args[1]);
			}
			oss << (xrayTube->getShutter(index) ? "on" : "off");
			return oss.str();
		}

		else if (!strcasecmp(command.c_str(), "getshutters")) {
			if (nArg != 1) {
				throw RuntimeError("Requires 1 parameters: getshutters ");
			}
			OnlyTubeCommand();
			oss << "1:" << (xrayTube->getShutter(1) ? "on" : "off") << ' ';
			oss << "2:" << (xrayTube->getShutter(2) ? "on" : "off") << ' ';
			oss << "3:" << (xrayTube->getShutter(3) ? "on" : "off") << ' ';
			oss << "4:" << (xrayTube->getShutter(4) ? "on" : "off");
			return oss.str();
		}

		else if (!strcasecmp(command.c_str(), "connectedshutters")) {
			if (nArg != 1) {
				throw RuntimeError("Requires 1 parameters: connectedshutters ");
			}
			OnlyTubeCommand();
			oss << "1:" << (xrayTube->isShutterConnected(1) ? "c" : "nc") << ' ';
			oss << "2:" << (xrayTube->isShutterConnected(2) ? "c" : "nc") << ' ';
			oss << "3:" << (xrayTube->isShutterConnected(3) ? "c" : "nc") << ' ';
			oss << "4:" << (xrayTube->isShutterConnected(4) ? "c" : "nc");
			return oss.str();
		}

		else if (!strcasecmp(command.c_str(), "shutter")) {
			if (nArg != 3) {
				throw RuntimeError("Requires 3 parameters: shutter [shutter index] [on/off]");
			}
			OnlyTubeCommand();
			int index = 0;
			istringstream iss (args[1].c_str());
			iss >> index;
			if (iss.fail()) {
				throw RuntimeError("Could not scan shutter index argument " + args[1]);
			}
			bool on = false;
			if (!strcasecmp(args[2].c_str(), "on")) {
				on = true;
			} else if (!strcasecmp(args[2].c_str(), "off")) {
				on = false;
			} else {
				throw RuntimeError ("Cannot scan shutter value argument " + args[2]);
			}
			xrayTube->setShutter(index, on);
			oss << (xrayTube->getShutter(index) ? "on" : "off");
			return oss.str();
		}

		else if (!strcasecmp(command.c_str(), "warmup")) {
			if (nArg != 2) {
				throw RuntimeError("Requires 2 parameters: warmup [voltage]");
			}
			OnlyTubeCommand();
			int voltage = 0;
			istringstream iss (args[1].c_str());
			iss >> voltage;
			if (iss.fail()) {
				throw RuntimeError("Could not scan warm up voltage argument " + args[1]);
			}
			xrayTube->startWarmup(voltage);
			oss << (xrayTube->getHVSwitch() ? "on" : "off");
			return oss.str();
		}	

		else if (!strcasecmp(command.c_str(), "getwtime")) {
			if (nArg != 1) {
				throw RuntimeError("Requires 1 parameters: getwtime ");
			}
			OnlyTubeCommand();
			oss << xrayTube->getWarmupTimeRemaining() << " s";
			return oss.str();
		}

		else if (!strcasecmp(command.c_str(), "issafe")) {
			if (nArg != 1) {
				throw RuntimeError("Requires 1 parameters: issafe ");
			}
			OnlyTubeCommand();
			oss << (xrayTube->isAccessPossible() ? "yes" : "no");
			return oss.str();
		}

		else if (!strcasecmp(command.c_str(), "readwarmuptiming")) {
			if (nArg != 2) {
				throw RuntimeError("Requires 2 parameters: readwarmuptiming [voltage] ");
			}
			OnlyTubeCommand();
			int voltage = 0;
			istringstream iss (args[1].c_str());
			iss >> voltage;
			if (iss.fail()) {
				throw RuntimeError("Could not scan timestamp voltage argument " + args[1]);
			}
			oss << xrayTube->getWarmupTimestamp(voltage);
			return oss.str();
		}

		else if (!strcasecmp(command.c_str(), "writeallwarmuptiming")) {
			if (nArg != 1) {
				throw RuntimeError("Requires 1 parameters: writeallwarmuptiming");
			}
			OnlyTubeCommand();
			xrayTube->writeAllWarmupTimestamps();
			return "ok";
		}


		// ----- pressure ------------------------------------------------------

		else if (!strcasecmp(command.c_str(), "pressure")) {
			if (nArg != 1) {
				throw RuntimeError("Requires 1 parameters: pressure");
			}
			if (pgauge == NULL) {
				try {
					UpdateInterface(PRESSURE);
				} catch (RuntimeError &e) {
					throw;
				}	
			}
			std::vector<PressureGauge> result = pgauge->getPressure();
			oss << 	"1: [" << result[0].status << ", " << result[0].pressure << "]\n"
					"2: [" << result[1].status << ", " << result[1].pressure << "]";
			return oss.str();
		}


		// ----- reference points ----------------------------------------------

		else if (!strcasecmp(command.c_str(), "reflist")) {
			if (nArg != 1) {
				throw RuntimeError("Requires 1 parameters: reflist");
			}
			if (referencePoints == NULL) {
				throw RuntimeError ("No reference points added");
			}
			oss << referencePoints->getList();
			return oss.str();
		}

		else if (!strcasecmp(command.c_str(), "refvals")) {
			if (nArg != 2) {
				throw RuntimeError("Requires 2 parameters: refvals [name]");
			}
			if (referencePoints == NULL) {
				throw RuntimeError ("No reference points added");
			}
			std::string name = args[1];
			vector <double> positions = referencePoints->getPositions(name);
			for (unsigned int i = 0; i < positions.size(); ++i) {
				oss << positions[i] << ' ';
			}
			return oss.str();
		}

		else if (!strcasecmp(command.c_str(), "checkref")) {
			if (nArg != 1) {
				throw RuntimeError("Requires 1 parameters: checkref");
			}
			if (referencePoints == NULL) {
				throw RuntimeError ("No reference points added");
			}
			oss << referencePoints->getCurrentReferenceName();
			return oss.str();
		}

		else if (!strcasecmp(command.c_str(), "ref")) {
			if (nArg != 2) {
				throw RuntimeError("Requires 2 parameters: ref [name]");
			}
			if (referencePoints == NULL) {
				throw RuntimeError ("No reference points added");
			}
			std::string name = args[1];
			referencePoints->moveTo(name);
			return "ok";
		}


		// ----- filter wheels -------------------------------------------------


	/*

		// --- if command is fvals---------------------------------------
		else if(command == "fvals")
		{
			//if it doesnt exist, the error message in the end
			num=0;
			// if number of parameters are wrong
			if(nArg!=2)
			{
				strcpy(mess, "ERROR: Required number of parameters: 2\nHelp: fvals [filter_wheel_name]");
				return -1;
			}

			strcpy(mess,"");
			//find the fwheel and move it
			for(int i=0;i<Fwheel::NumFwheels;i++)
				if(!strcasecmp(args[1],fwheel[i]->getName()))
				{
					char cVal[20]="";
					for(int j=0;j<Fwheel::NumSlotsInWheel;j++)
					{
						sprintf(cVal,"%f",fwheel[i]->ValueList[j]);
						strcat(mess,cVal);
						strcat(mess," ");
					}
					return 0;
				}

		}



		// --- if command is fwlist---------------------------------------
		else if(command == "fwlist")
		{
			// if number of parameters are wrong
			if(nArg!=1)
			{
				strcpy(mess, "ERROR: Required number of parameters: 1\nHelp: fwlist");
				return -1;
			}
			//all the filter wheels
			for(int i=0;i<fwheel.size();i++)
			{
				strcat(mess," ");
				strcat(mess,fwheel[i]->getName());
			}
			return 0;

		}



	// --- if command is fsetval---------------------------------------
		else if(command == "fsetval")
		{
			num=0;
			// if number of parameters are wrong
			if(nArg!=3)
			{
				strcpy(mess, "ERROR: Required number of parameters: 3\nHelp: fsetval [filter_wheel_name] [value]\n");
				return -1;
			}

			// if value is not a number
			temp.assign(args[2]);
			if(temp.find_first_not_of("0123456789.-")!=string::npos)
			{
				sprintf(mess, "ERROR: %s for absorption value should be a number",args[2]);
				return -1;
			}

			//find the fwheel and move it
			for(int i=0;i<fwheel.size();i++)
				if(!strcasecmp(args[1],fwheel[i]->getName()))
				{
					if(!fwheel[i]->setValue(atof(args[2])))
					{
						sprintf(mess,"ERROR: %s absorption value for %s is not defined. \nOptions(",args[2],args[1]);
						char cVal[20]="";
						for(int j=0;j<Fwheel::NumSlotsInWheel;j++)
						{
							sprintf(cVal,"%f",fwheel[i]->ValueList[j]);
							strcat(mess,cVal);
							strcat(mess,",");
						}
						strcat(mess,")");
						return -1;
					}
					sprintf(mess,"%s set to %s value",args[1],args[2]);
					return 0;
				}
		}



		// --- if command is fgetval---------------------------------------
		else if(command == "fgetval")
		{
			num=0;
			// if number of parameters are wrong
			if(nArg!=2)
			{
				strcpy(mess, "ERROR: Required number of parameters: 2\nHelp: fgetval [filter_wheel_name]\n");
				return -1;
			}

			//find fwheel and value
			for(int i=0;i<fwheel.size();i++)
				if(!strcasecmp(args[1],fwheel[i]->getName()))
				{
					sprintf(mess,"%f",fwheel[i]->getValue());
					return 0;
				}
		}


































		// --- if command is getX1Limit---------------------------------------
		else if(command == "getX1Limit")
		{

			// if number of parameters are wrong
			if(nArg!=1)
			{
				strcpy(mess, "ERROR: Required number of parameters: 1");
				return -1;
			}

			// if number of parameters are wrong
			if(slit==NULL)
			{
				strcpy(mess, "ERROR: Slits are not entered in the config file");
				return -1;
			}

			sprintf(mess,"%f",slit->getX1Limit());
			return 0;

		}



		// --- if command is getX2Limit---------------------------------------
		else if(command == "getX2Limit")
		{

			// if number of parameters are wrong
			if(nArg!=1)
			{
				strcpy(mess, "ERROR: Required number of parameters: 1");
				return -1;
			}

			// if number of parameters are wrong
			if(slit==NULL)
			{
				strcpy(mess, "ERROR: Slits are not entered in the config file");
				return -1;
			}

			sprintf(mess,"%f",slit->getX2Limit());
			return 0;

		}


		// --- if command is getcenter---------------------------------------
		else if(command == "getcenter")
		{
			//if slits are not included in config file
			if(slit == NULL)
			{
				strcpy(mess, "ERROR: The slit motors do not exist in the config file.");
				return -1;
			}
			// if number of parameters are wrong
			if(nArg!=1)
			{
				strcpy(mess, "ERROR: Required number of parameters: 1");
				return -1;
			}

			sprintf(mess,"%f",slit->getX1Center());
			return 0;
		}



		// --- if command is getslitwidth---------------------------------------
		else if(command == "getslitwidth")
		{
			//if slits are not included in config file
			if(slit == NULL)
			{
				strcpy(mess, "ERROR: The slit motors do not exist in the config file.");
				return -1;
			}
			// if number of parameters are wrong
			if(nArg!=1)
			{
				strcpy(mess, "ERROR: Required number of parameters: 1");
				return -1;
			}

			sprintf(mess,"%f",slit->getSlitWidth());
			return 0;
		}






		// --- if command is widthrel---------------------------------------
		else if(command == "widthrel")
		{
			//if slits are not included in config file
			if(slit == NULL)
			{
				strcpy(mess, "ERROR: The slit motors do not exist in the config file.");
				return -1;
			}

			double halfWidth;
			// if number of parameters are wrong
			if(nArg!=2)
			{
				strcpy(mess, "ERROR: Required number of parameters: 2");
				return -1;
			}

			// if width is not a number
			temp.assign(args[1]);
			if(temp.find_first_not_of("0123456789.-")!=string::npos)
			{
				sprintf(mess, "ERROR: %s for width should be a number",args[1]);
				return -1;
			}

			halfWidth = atof(args[1])/2;
			newPosition = motor[slit1_exists]->getPosition() - halfWidth;
			newPosition2 = motor[slit2_exists]->getPosition() - halfWidth;

			// checks if width is too large, then the motors will go past the original positions
			if(slit->canBothSlitsMove(newPosition,newPosition2))
			{
				strcpy(mess,"ERROR: The width should be small enough so that the slit motors can move");
				return -1;
			}

			motor[slit1_exists]->moveRel(0-halfWidth ,motor[slit2_exists]->getAxis(),0-halfWidth );

			// sets the positions in the motor objects and the slit object
			motor[slit1_exists]->setPosition(newPosition);
			motor[slit2_exists]->setPosition(newPosition2);
			slit->setBothpos(newPosition,newPosition2);

			sprintf(mess,"The Slit width has been increased by %f and slit_x1 has moved to %f and slit_x2 has moved to %f",atof(args[1]),newPosition, newPosition2);
			return 0;
		}






		// --- if command is widthabs---------------------------------------
		else if(command == "widthabs")
		{
			//if slits are not included in config file
			if(slit == NULL)
			{
				strcpy(mess, "ERROR: The slit motors do not exist in the config file.");
				return -1;
			}

			// if number of parameters are wrong
			if(nArg!=2)
			{
				strcpy(mess, "ERROR: Required number of parameters: 2");
				return -1;
			}

			// if width is not a number
			temp.assign(args[1]);
			if(temp.find_first_not_of("0123456789.")!=string::npos)
			{
				sprintf(mess, "ERROR: %s for width should be a positive number",args[1]);
				return -1;
			}

			// the absolute middle position of the slits
			midpos = (motor[slit1_exists]->getPosition()+slit->getX1Limit())/2;
			// the distance to be moved relatively from the middle position of the slits
			double moveby = atof(args[1])/2;

			newPosition = midpos - moveby;
			newPosition2 = slit->getLimit()-(midpos+moveby);

			// checks if width is too large, then the motors will go past the original positions
			if(slit->canBothSlitsMove(newPosition,newPosition2))
			{
				strcpy(mess,"ERROR: The width should be small enough so that the slit motors can move");
				return -1;
			}
			motor[slit1_exists]->moveAbs(newPosition,motor[slit2_exists]->getAxis(),newPosition2,motor[slit2_exists]->getPosition());

			// sets the positions in the motor objects and the slit object
			motor[slit1_exists]->setPosition(newPosition);
			motor[slit2_exists]->setPosition(newPosition2);
			slit->setBothpos(newPosition,newPosition2);
			midpos = (motor[slit1_exists]->getPosition()+slit->getX1Limit())/2;
			sprintf(mess,"Slit_x1 has been moved to %f and slit_x2 to %f, with a width of %f and midpos is %f",newPosition, newPosition2,slit->getSlitWidth(),midpos);
			return 0;
		}





		// --- if command is centerrel---------------------------------------
		else if(command == "centerrel")
		{
			//if slits are not included in config file
			if(slit == NULL)
			{
				strcpy(mess, "ERROR: The slit motors do not exist in the config file.");
				return -1;
			}

			// if number of parameters are wrong
			if(nArg!=2)
			{
				strcpy(mess, "ERROR: Required number of parameters: 2");
				return -1;
			}

			// if position is not a number
			temp.assign(args[1]);
			if(temp.find_first_not_of("0123456789.-")!=string::npos)
			{
				sprintf(mess, "ERROR: %s for position should be a number",args[1]);
				return -1;
			}

			newPosition = motor[slit1_exists]->getPosition()+atof(args[1]);
			newPosition2 = motor[slit2_exists]->getPosition()-atof(args[1]);

			if(slit->canBothSlitsMove(newPosition,newPosition2))
			{
				strcpy(mess,"ERROR: Either of the slits cannot be moved to a negative position ");
				return -1;
			}	  motor[slit1_exists]->moveRel(atof(args[1]),motor[slit2_exists]->getAxis(),0-atof(args[1]));
			// sets the positions in the motor objects and the slit object
			motor[slit1_exists]->setPosition(newPosition);
			motor[slit2_exists]->setPosition(newPosition2);
			slit->setBothpos(newPosition,newPosition2);
			midpos = (motor[slit1_exists]->getPosition()+slit->getX1Limit())/2;
			sprintf(mess,"Slit_x1 has been moved to %f and slit_x2 to %f, with same width %f but midpos is %f",newPosition, newPosition2,slit->getSlitWidth(),midpos );
			return 0;

		}






		// --- if command is centerabs---------------------------------------
		else if(command == "centerabs")
		{
			//if slits are not included in config file
			if(slit == NULL)
			{
				strcpy(mess, "ERROR: The slit motors do not exist in the config file.");
				return -1;
			}

			// if number of parameters are wrong
			if(nArg!=2)
			{
				strcpy(mess, "ERROR: Required number of parameters: 2");
				return -1;
			}

			// if position is not a number
			temp.assign(args[1]);
			if(temp.find_first_not_of("0123456789.")!=string::npos)
			{
				sprintf(mess, "ERROR: %s for position should be a positive number",args[1]);
				return -1;
			}

			// the absolute middle position of the slits
			midpos = (motor[slit1_exists]->getPosition()+slit->getX1Limit())/2;
			// calculate the relative position to be moved
			double relpos =  atof(args[1]) - midpos;

			newPosition = motor[slit1_exists]->getPosition() + relpos;
			newPosition2 = motor[slit2_exists]->getPosition() - relpos;

			//check if the slits will crash into each other
			if(slit->canBothSlitsMove(newPosition,newPosition2))
			{
				strcpy(mess,"ERROR: Either of the slits cannot be moved to a negative position");
				return -1;
			}

			motor[slit1_exists]->moveRel(relpos,motor[slit2_exists]->getAxis(),0-relpos);
			// sets the positions in the motor objects and the slit object
			motor[slit1_exists]->setPosition(newPosition);
			motor[slit2_exists]->setPosition(newPosition2);
			slit->setBothpos(newPosition,newPosition2);
			midpos = (motor[slit1_exists]->getPosition()+slit->getX1Limit())/2;
			sprintf(mess,"Slit_x1 has been moved to %f and slit_x2 to %f, with same width %f but midpos is %f",newPosition, newPosition2,slit->getSlitWidth(),midpos );
			return 0;

		}





		// --- if command is 'exactcenter' to move one of the slits right next to the other----------------
		else if(command == "exactcenter")
		{
			//if slits are not included in config file
			if(slit == NULL)
			{
				strcpy(mess, "ERROR: The slit motors do not exist in the config file.");
				return -1;
			}

			// if number of parameters are wrong
			if(nArg!=1)
			{
				strcpy(mess, "ERROR: Required number of parameters: 1");
				return -1;
			}
			double exactcenter = slit->getLimit()/2;
			// moving both slits to exact center
			motor[slit1_exists]->moveAbs(exactcenter,motor[slit2_exists]->getAxis(),exactcenter,motor[slit2_exists]->getPosition());
			motor[slit1_exists]->setPosition(exactcenter);
			motor[slit2_exists]->setPosition(exactcenter);
			slit->setBothpos(exactcenter,exactcenter);
			sprintf(mess,"Moved both the slits to the exact center at position 51.25");
			return 0;

		}




		// --- if command is 'x1zerowidth' to move slit_x2 towards slit_x1 with width 0----------------
		else if(command == "x1zerowidth")
		{
			//if slits are not included in config file
			if(slit == NULL)
			{
				strcpy(mess, "ERROR: The slit motors do not exist in the config file.");
				return -1;
			}

			// if number of parameters are wrong
			if(nArg!=1)
			{
				strcpy(mess, "ERROR: Required number of parameters: 1");
				return -1;
			}

			newPosition=slit->getX2Limit();
			motor[slit2_exists]->moveAbs(newPosition,0,0,0);
			motor[slit2_exists]->setPosition(newPosition);
			slit->setX2pos(newPosition);
			sprintf(mess,"Moved slit_x2 to %f position, slit_x1:%f position,width:%f",newPosition,motor[slit1_exists]->getPosition(),slit->getSlitWidth());
			return 0;

		}




		// --- if command is 'zerowidth' to move both slits simultaneously
		//right next to each other to an absolute position of the slit specified--------------------------
		else if(command == "zerowidth")
		{
			//if slits are not included in config file
			if(slit == NULL)
			{
				strcpy(mess, "ERROR: The slit motors do not exist in the config file.");
				return -1;
			}

			// if number of parameters are wrong
			if(nArg!=2)
			{
				strcpy(mess, "ERROR: Required number of parameters: 2");
				return -1;
			}

			// if position is a positive number
			temp.assign(args[2]);
			if(temp.find_first_not_of("0123456789.")!=string::npos)
			{
				sprintf(mess, "ERROR: %s for position should be a positive number",args[2]);
				return -1;
			}

			newPosition=atof(args[2]);
			newPosition2=slit->getLimit()-newPosition;

			// check if position given is within limit
			if(newPosition>slit->getLimit())
			{
				sprintf(mess, "ERROR: Position should be less than %f, else slits crash into each other",slit->getLimit());
				return -1;
			}


			motor[slit1_exists]->moveAbs(newPosition,motor[slit2_exists]->getAxis(),newPosition2,motor[slit2_exists]->getPosition());
			motor[slit1_exists]->setPosition(newPosition);
			motor[slit2_exists]->setPosition(newPosition2);
			slit->setBothpos(newPosition,newPosition2);
			sprintf(mess,"Moved slit_x1 to %f position and slit_x2 to %f position with 0 width",newPosition,newPosition2);
			return 0;

		}







	*/
		// unknown command
		throw RuntimeError("Unknown command " + command);
	} 
	// delete xray class if tube switched off
	catch (const TubeOffError &e) {
		if (xrayTube != NULL) {
			int port = xrayTube->getInterface()->getSerialPortNumber();
			usbSerialPortsUsed[port] = false;
			FILE_LOG(logINFO) << "Setting interface /ttyUSB" << port << ": Not in use";
			delete xrayTube;
			xrayTube = NULL;
		}
		throw;
	}
	// delete xray class if tube switched off
	catch (const PressureOffError &e) {
		if (pgauge != NULL) {
			int port = pgauge->getInterface()->getSerialPortNumber();
			usbSerialPortsUsed[port] = false;
			FILE_LOG(logINFO) << "Setting interface /ttyUSB" << port << ": Not in use";
			delete pgauge;
			pgauge = NULL;
		}
		throw;
	}
}

Initialize::~Initialize() {
	if (xrayTube != NULL) {
		delete xrayTube;
	}
	if (pgauge != NULL) {
		delete pgauge;
	}
	if (slit != NULL) {
		delete slit;
	}
}



Initialize::Initialize() 
	: xrayTube(NULL), pgauge(NULL), slit(NULL), maxTubePower(0) {

	usbSerialPortsUsed.resize(MAX_USB_SERIAL_PORTS);
	for (unsigned int i = 0; i < usbSerialPortsUsed.size(); ++i) {
		usbSerialPortsUsed[i] = false;
	}

	ReadConfigFile();
	

	// Validations
	// min 1 motor
	if (motor.size() == 0) {
		throw RuntimeError("No motor found. At least 1 motor required.");
	}
	// min 1 motor per controller
	for (unsigned int i = 0; i < controller.size(); ++i) {
		if (controller[i]->getMotor(0) == NULL && controller[i]->getMotor(1) == NULL && controller[i]->getMotor(2) == NULL) {
			throw RuntimeError("Controller " + controller[i]->getName() + " has no motors connected to it.");
		}
	}	
	// min 1 target (1 target list) for each fluorescence motor
	for (unsigned int i = 0; i < fluorescence.size(); ++i) {
		if (fluorescence[i]->getNumTargetHolders() == 0) {
			throw RuntimeError("No targets added to " + fluorescence[i]->getName());
		}
	}

	// update motor axis for each controller and positions (debug)
	for (unsigned int i = 0; i < controller.size(); ++i) {
		controller[i]->updateAxisEnabled();
		controller[i]->debugPositions();
	}

	// update limits now that the positions have been debugged
	if (slit != NULL) {
		slit->updateLimits();
	}


	// Print
	cout << "Motors: " << motor.size() << endl;
	cout << "==========" << endl;
	for (unsigned int i = 0; i < motor.size(); ++i) {
		motor[i]->print();
	}	
	if (referencePoints != NULL) {
		cout << "Reference Points" << endl;
		cout << "================" << endl;
		referencePoints->print();
	}
	if (fluorescence.size() > 0) {
		cout << "Fluorescence Motors: " << fluorescence.size() << endl;
		cout << "======================" << endl;
		for (unsigned int i = 0; i < fluorescence.size(); ++i) {
			fluorescence[i]->print();
		}	
	}
	if (slit != NULL) {
		slit->print();
	}
	if (fwheel.size() > 0) {
		cout << "Filter Wheels: " << fwheel.size() << endl;
		cout << "================" << endl;
		for (unsigned int i = 0; i < fwheel.size(); ++i) {
			fwheel[i]->print();
		}	
	}	
	cout << "Controllers: " << controller.size() << endl;
	cout << "==============" << endl;
	for (unsigned int i = 0; i < controller.size(); ++i) {
		controller[i]->print();
	}	
	if (xrayTube != NULL) {
		xrayTube->print();
	}
	if (pgauge != NULL) {
		pgauge->print();
	}
	cout << endl;
}


void Initialize::ReadConfigFile() {
	FILE_LOG(logINFO) << "Reading config file" << endl;
	ifstream inFile;
	inFile.open(CONFIG_FILE, ifstream::in);
	if (!inFile.is_open()) {
		throw RuntimeError("Could not open config file.");
	}
	while(inFile.good()) {
		string sLine;
		getline(inFile,sLine);
		// delete lines after comments
        if (sLine.find('#') != string::npos) {
            sLine.erase(sLine.find('#'));
        }
		FILE_LOG(logDEBUG) << "line:"<< sLine << endl;
		// scan arguments
		istringstream iss(sLine);
		vector<string> args = vector<string>(istream_iterator<string>(iss), istream_iterator<string>());
		for (unsigned int i = 0; i < args.size(); ++i) {
			FILE_LOG(logDEBUG) << i << ":[" << args[i] << "] length:" << args[i].length() << endl;
		}
		// blank lines
		if (args.size() == 0 || args[0].empty()) {
			continue;
		}
		string mode = args[0];
		if(mode == "power") {
			TubeMode(args);
		} else if(mode == "pressure") {
			PressureMode();
		} else if(mode == "fwheel") {
			FwheelMode(args);
		} else if(mode=="refpoint") {
			ReferencePointMode(args);
		} else if(mode=="fluorescence") {
			FluorescenceMode(args);
		} else if(mode=="fluorescence_wheel") {
			FluorescenceMode(args);
		} else if(mode=="controller")	{
			ControllerMode(args);
		} else if(mode=="motor") {
			MotorMode(args);
		} else {
			throw RuntimeError("Cannot scan command type " + mode + " in config file");
		}		
	}
	inFile.close();
}


void Initialize::UpdateInterface(InterfaceIndex index) {
	if (index == TUBE && xrayTube != NULL) {
		return;
	}
	if (index == PRESSURE && pgauge != NULL) {
		return;
	}	
	// get last index to update interface
	int lastIndex = 0;
	if (index == CONTROLLER) {
		lastIndex = controller.size() - 1;
	} else if (index == FILTER_WHEEL) {
		lastIndex = fwheel.size() - 1;
	}

	// loop through usb ports
	for (unsigned int i = 0; i < usbSerialPortsUsed.size(); ++i) {
		// ignore used ones
		if (usbSerialPortsUsed[i]) {
			continue;
		}
		ostringstream oss;
		oss << USB_PORT_PREFIX << i;
		string usbport = oss.str();

		try {
			Interface* interface = new Interface(usbport, i, index);
			switch (index) {
				case TUBE:
					usbSerialPortsUsed[i] = true;
					xrayTube = new Xray(interface);
					return;
				case PRESSURE:
					usbSerialPortsUsed[i] = true;
					pgauge = new Pgauge(interface);
					return;
				case CONTROLLER:
					if (Controller::CheckControllerSerialNumber(i, interface, controller[lastIndex]->getSerialNumber())) {
						usbSerialPortsUsed[i] = true;
						controller[lastIndex]->setInterface(interface);
						return;
					}
					break;
				case FILTER_WHEEL:
					if (Fwheel::CheckFWSerialNumber(i, fwheel[lastIndex]->getSerialNumber())) {
						usbSerialPortsUsed[i] = true;
						fwheel[lastIndex]->setInterface(interface);
						return;
					}
				default:
					break;
			}
		} catch (...) {
			continue;
		}
	}
	ostringstream oss;
	switch (index) {
	case TUBE:
		oss << "Tube is probably switched off. Could not find usb serial port.";
		throw TubeOffError (oss.str());
	case PRESSURE:
		oss << "Pressure Gauge is probably switched off. Could not find usb serial port.";
		throw PressureOffError(oss.str());
	case CONTROLLER:
		oss << "Could not find usb serial port for controller.";
		break;
	case FILTER_WHEEL:
		oss << "Could not find usb serial port for filter wheel.";
		break;
	default:
		break;
	}
	throw RuntimeError(oss.str());
}



void Initialize::TubeMode(vector<string> args) {
	if (args.size() != CONFIG_POWER_NUM_ARGS) {
		ostringstream oss;
		oss << "Incorrect number of arguments for power. Expected 2, got " << args.size() ;
		throw RuntimeError(oss.str());
	}

	// tube power
	istringstream iss (args[1].c_str());
	maxTubePower = 0;
	iss >> maxTubePower;
	if (iss.fail()) {
		throw RuntimeError("Could not scan power argument " + args[1]);
	}
	if (maxTubePower <= 0) {
		throw RuntimeError("Invalid power " + args[1]);
	}

	// get tube interface
	try {
		UpdateInterface(TUBE);
	} catch (RuntimeError &e) {
		FILE_LOG(logWARNING) << "Xray Tube is probably switched off. Continuing.";
	}	
	xrayTube->setMaxPower(maxTubePower);
	cout << endl;
}


void Initialize::PressureMode() {
	// get pressure interface
	try {
		UpdateInterface(PRESSURE);
	} catch (RuntimeError &e) {
		FILE_LOG(logWARNING) << "Pressure Gauge is probably switched off. Continuing.";
	}	
	cout << endl;
}


void Initialize::FwheelMode(vector<string> args) {
	if (args.size()  != CONFIG_FWHEEL_NUM_ARGS) {
		ostringstream oss;
		oss << "Incorrect number of arguments for fwheel. Expected " << CONFIG_FWHEEL_NUM_ARGS << ", got " << args.size() ;
		throw RuntimeError(oss.str());
	}

	// parse
	string name = args[1];
	string serialNumber = args[2];
	vector<double> values(CONFIG_FWHEEL_NUM_ARGS - 3);
	for (unsigned int i = 0; i < values.size(); ++i) {
		istringstream iss (args[3 + i].c_str());
		iss >> values[i];
		if (iss.fail()) {
			throw RuntimeError("Could not scan fwheel absorption values " + args[3 + i]);
		}
	}
	
	// create fwheel object
	fwheel.push_back(new Fwheel(fwheel.size(), name, serialNumber, values));

	// get fwheel interface
	try {
		UpdateInterface(FILTER_WHEEL);
	} catch (RuntimeError &e) {
		throw RuntimeError("Could not get usb port for Fwheel " + name);
	}

	// ensure they start at position 1
	fwheel[fwheel.size() - 1]->setStartPosition();
	fwheel[fwheel.size() - 1]->setStartPosition(); //the second time it works
	cout << endl;
 }


 void Initialize::ReferencePointMode(vector<string> args) {
	if (args.size()  != CONFIG_REF_POINT_NUM_ARGS) {
		ostringstream oss;
		oss << "Incorrect number of arguments for reference point. Expected " << CONFIG_REF_POINT_NUM_ARGS << ", got " << args.size() ;
		throw RuntimeError(oss.str());
	}

	// parse
	string name = args[1];
	vector<double> positions;
	for (unsigned int i = 2; i < args.size(); ++i) {
		istringstream iss (args[i].c_str());
		double pos;
		iss >> pos;
		if (iss.fail()) {
			throw RuntimeError("Could not scan refpoint positions for " + args[i]);
		}
		positions.push_back(pos);
	}

	// first referencepoints object
	if (referencePoints == NULL) {
		vector < Controller* > refController;
		vector <int> axis;
		int numPositionMotors = 3;
		const char* motorNames[] = {"Detector_x", "Detector_y", "Detector_z"};
		try {
			for (unsigned int i = 0; i < numPositionMotors; ++i) {
				int imotor =  GetMotorIndex(motorNames[i]);
				refController.push_back(controller[motor[imotor]->getController()]);
				axis.push_back(motor[imotor]->getAxis());
			}
		} catch (...) {
			throw RuntimeError("Could not add reference point. Missing Detector_x, Detector_y or Detector_z motor");
		}
		referencePoints = new ReferencePoints(refController, axis);
	}

	referencePoints->add(name, positions);
 }


 void Initialize::FluorescenceMode(vector<string> args) {
	if (args.size()  != CONFIG_FLUOR_NUM_ARGS) {
		ostringstream oss;
		oss << "Incorrect number of arguments for fluorescence/fluorescence_wheel. Expected " << CONFIG_FLUOR_NUM_ARGS << ", got " << args.size() ;
		throw RuntimeError(oss.str());
	}

	// parse
	string name = args[0];
	string targetName = args[1];
	string targetEnergy = args[2];

	// add targets 
	int ifluor = GetFluorescenceIndex(name);
	fluorescence[ifluor]->addTarget(targetName, targetEnergy);
 }


 void Initialize::ControllerMode(vector<string> args) {
	if (args.size()  != CONFIG_CONTROLLER_NUM_ARGS) {
		ostringstream oss;
		oss << "Incorrect number of arguments for controller. Expected " << CONFIG_CONTROLLER_NUM_ARGS << ", got " << args.size() ;
		throw RuntimeError(oss.str());
	}

	// parse
	string name = args[1];
	string serialNumber = args[2];

	// validations
	for (unsigned int i = 0; i < controller.size(); ++i) {
			if (name == controller[i]->getName() || serialNumber == controller[i]->getSerialNumber()) {
			throw RuntimeError("Two controllers cannot have the same name "  + name + " or serial number " + serialNumber);
		}
	}

	// create controller object
	controller.push_back(new Controller(controller.size(), name, serialNumber));

	// get controller interface
	try {
		UpdateInterface(CONTROLLER);
	} catch (RuntimeError& e) {
		throw RuntimeError("Could not find usb serial port for controller " + args[1]);
	}
	cout << endl;
 }


void Initialize::MotorMode(vector<string> args) {
	if (args.size()  != CONFIG_MOTOR_NUM_ARGS) {
		ostringstream oss;
		oss << "Incorrect number of arguments for motor. Expected " << CONFIG_MOTOR_NUM_ARGS << ", got " << args.size() ;
		throw RuntimeError(oss.str());
	}

	// parse
	string name = args[1];
	int axis = -1; 
	{
		istringstream iss (args[2].c_str());
		iss >> axis;
		if (iss.fail()) {
			throw RuntimeError("Could not scan axis for motor argument: " + args[2]);
		}
		--axis;
	}
	string controllerName = args[3];
	double lowerLimit = -1;
	{
		istringstream iss (args[4].c_str());
		iss >> lowerLimit;
		if (iss.fail()) {
			throw RuntimeError("Could not scan lowerLimit for motor argument: " + args[4]);
		}
	}
	double upperLimit = -1;
	{
		istringstream iss (args[5].c_str());
		iss >> upperLimit;
		if (iss.fail()) {
			throw RuntimeError("Could not scan upperLimit for motor argument: " + args[5]);
		}
	}

	// validations
	for (unsigned int i = 0; i < motor.size(); ++i) {
			if (name == motor[i]->getName()) {
			throw RuntimeError("Two motors cannot have the same name "  + name);
		}
	}
	// axis validated inside controller->setMotor
	// limits validated inside motor
	// find controller index by name
	int icontroller = GetControllerIndex(controllerName);

	// create motor object
	motor.push_back(new Motor(motor.size(), name, axis, icontroller, lowerLimit, upperLimit));
	controller[icontroller]->setMotor(axis, motor[motor.size() - 1]);


	// if slit_x2, create slit object

	// if fluorescence, create fluorescence object
	if (name == "Fluorescence" || name == "Fluorescence_wheel") {
		fluorescence.push_back(new Fluorescence(fluorescence.size(), name, controller[icontroller], axis));
	} else if (name == "Slit_x2") {
		int imotorX1 = GetMotorIndex("Slit_x1");
		Controller* controller_slitx1 = controller[motor[imotorX1]->getController()];
		int iaxisX1 = motor[imotorX1]->getAxis();
		slit = new Slit(controller_slitx1, controller[icontroller], iaxisX1, axis);
	}
	cout << endl;
 }
