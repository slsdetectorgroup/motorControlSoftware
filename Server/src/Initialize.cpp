#include "Initialize.h"
#include "Controller.h"
#include "Fluorescence.h"
#include "Fwheel.h"
#include "Motor.h"
#include "Pgauge.h"
#include "ReferencePoints.h"
#include "Slit.h"
#include "Xray.h"
#include "commonDefs.h"

#include "commonDefs.h"
#include <cstdlib> //exit
#include <cstring> //strncpy
#include <fstream>
#include <iomanip>
#include <iterator>
#include <math.h>
#include <sstream>
#include <time.h>
#include <unistd.h> // (usleep,etc.) in raspberrypi
using namespace std;

#define CONFIG_FILE                ("config.txt")
#define CONFIG_POWER_NUM_ARGS      (2)
#define CONFIG_FWHEEL_NUM_ARGS     (9)
#define CONFIG_REF_POINT_NUM_ARGS  (5)
#define CONFIG_FLUOR_NUM_ARGS      (3)
#define CONFIG_CONTROLLER_NUM_ARGS (3)
#define CONFIG_MOTOR_NUM_ARGS      (6)

#define MAX_USB_SERIAL_PORTS (10)
#define USB_PORT_PREFIX      ("/dev/ttyUSB")

void Initialize::OnlySlitCommand() {
    if (slit == NULL) {
        throw RuntimeError("Slits motors do not exist");
    }
}

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
    if ((strcasecmp(name.c_str(), "fluorescence")) &&
        (strcasecmp(name.c_str(), "fluorescence_wheel"))) {
        throw RuntimeError("Command not allowed for this motor " + name);
    }
}

void Initialize::RestrictedForSlitAndFluorescence(string name) {
    if ((!strcasecmp(name.c_str(), "slit_x1")) ||
        (!strcasecmp(name.c_str(), "slit_x2")) ||
        (!strcasecmp(name.c_str(), "fluorescence")) ||
        (!strcasecmp(name.c_str(), "fluorescence_wheel"))) {
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
    else if ((!strcasecmp(name.c_str(), "slit_x1")) ||
             (!strcasecmp(name.c_str(), "slit_x2"))) {
        if (slit == NULL) {
            throw RuntimeError("No slit object created");
        } else {
            slit->updateLimits();
        }
    }
}

int Initialize::GetFwheelIndex(string name) {
    for (unsigned int i = 0; i < fwheel.size(); ++i) {
        if (!strcasecmp(name.c_str(), fwheel[i]->getName().c_str())) {
            return i;
        }
    }
    throw RuntimeError("Unknown filter wheel " + name);
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
    int nArg = static_cast<int>(args.size());
    ostringstream oss;

    /*
            for (int i = 0; i < nArg; ++i) {
                    LOG(logINFO) << i << ":" << args[i];
            }
    */
    try {

        // ----- direct controller advanced
        // --------------------------------------------------------

        if (!strcasecmp(command.c_str(), "sendcontroller")) {
            if (nArg < 3) {
                throw RuntimeError("Requires 3 parameters: sendcontroller "
                                   "[corvus] [commands..]");
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
                throw RuntimeError("Requires 3 parameters: readcontroller "
                                   "[corvus] [commands..]");
            }
            string name = args[1];
            int icontroller = GetControllerIndex(name);
            string advancedCommand;
            for (int j = 2; j < nArg; ++j) {
                advancedCommand += (args[j] + " ");
            }
            return controller[icontroller]->sendCommandAndReadBack(
                advancedCommand);
        }

        else if (!strcasecmp(command.c_str(), "controllerlist")) {
            if (nArg != 1) {
                throw RuntimeError("Requires 1 parameters: controllerlist");
            }
            for (unsigned int i = 0; i < controller.size(); ++i) {
                oss << controller[i]->getName();
                if (i < controller.size() - 1) {
                    oss << ' ';
                }
            }
            return oss.str();
        }

        else if (!strcasecmp(command.c_str(), "getcontserialnumber")) {
            if (nArg != 2) {
                throw RuntimeError(
                    "Requires 2 parameters: getcontserialnumber [name]");
            }
            string name = args[1];
            int icontroller = GetControllerIndex(name);
            oss << controller[icontroller]->getSerialNumber();
            return oss.str();
        }

        else if (!strcasecmp(command.c_str(), "getcontinterface")) {
            if (nArg != 2) {
                throw RuntimeError(
                    "Requires 2 parameters: getcontinterface [name]");
            }
            string name = args[1];
            int icontroller = GetControllerIndex(name);
            oss << controller[icontroller]->getInterface()->getSerial();
            return oss.str();
        }

        else if (!strcasecmp(command.c_str(), "getcontmotorlist")) {
            if (nArg != 2) {
                throw RuntimeError(
                    "Requires 2 parameters: getcontmotorlist [name]");
            }
            string name = args[1];
            int icontroller = GetControllerIndex(name);
            for (unsigned int i = 0; i < 3; ++i) {
                Motor *m = controller[icontroller]->getMotor(i);
                if (m == NULL) {
                    oss << "- ";
                } else {
                    oss << m->getName() << ' ';
                }
            }
            return oss.str();
        }

        // ----- motor --------------------------------------------------------

        else if (!strcasecmp(command.c_str(), "motorlist")) {
            if (nArg != 1) {
                throw RuntimeError("Requires 1 parameters: list");
            }
            for (unsigned int i = 0; i < motor.size(); ++i) {
                oss << motor[i]->getName() << ' ';
            }
            return oss.str();
        }

        else if (!strcasecmp(command.c_str(), "getcontroller")) {
            if (nArg != 2) {
                throw RuntimeError(
                    "Requires 2 parameters: getcontroller [motor]");
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
                throw RuntimeError(
                    "Requires 3 parameters: setUpper [motor] [limit]");
            }
            string name = args[1];
            int imotor = GetMotorIndex(name);
            RestrictedForSlitAndFluorescence(name);
            double value = 0;
            istringstream iss(args[2].c_str());
            iss >> value;
            if (iss.fail()) {
                throw RuntimeError("Could not scan limit argument " + args[2]);
            }

            motor[imotor]->setUpperLimit(value);
            oss << value;
            return oss.str();
        }

        else if (!strcasecmp(command.c_str(), "getlower")) {
            if (nArg != 2) {
                throw RuntimeError("Requires 2 parameters: getLower [motor]");
            }
            string name = args[1];
            int imotor = GetMotorIndex(name);
            oss << motor[imotor]->getLowerLimit();
            return oss.str();
        }

        else if (!strcasecmp(command.c_str(), "setlower")) {
            if (nArg != 3) {
                throw RuntimeError(
                    "Requires 3 parameters: setLower [motor] [limit]");
            }
            string name = args[1];
            int imotor = GetMotorIndex(name);
            RestrictedForSlitAndFluorescence(name);
            double value = 0;
            istringstream iss(args[2].c_str());
            iss >> value;
            if (iss.fail()) {
                throw RuntimeError("Could not scan limit argument " + args[2]);
            }

            motor[imotor]->setLowerLimit(value);
            oss << value;
            return oss.str();
        }

        else if (!strcasecmp(command.c_str(), "stopall")) {
            if (nArg != 1) {
                throw RuntimeError("Requires 1 parameters: stopall ");
            }
            for (unsigned int i = 0; i < controller.size(); ++i) {
                controller[i]->stop();
            }
            return "ok";
        }

        else if (!strcasecmp(command.c_str(), "stop")) {
            if (nArg != 2) {
                throw RuntimeError("Requires 2 parameters: stop [motor]");
            }
            string name = args[1];
            int imotor = GetMotorIndex(name);
            controller[motor[imotor]->getController()]->stop();
            return "ok";
        }

        else if (!strcasecmp(command.c_str(), "cal")) {
            if (nArg != 2) {
                throw RuntimeError("Requires 2 parameters: cal [motor] ");
            }
            string name = args[1];
            int imotor = GetMotorIndex(name);
            controller[motor[imotor]->getController()]->calibrate(
                motor[imotor]->getAxis());
            UpdateSlitLimits(name);
            oss << fixed << motor[imotor]->getPosition();
            return oss.str();
        }

        else if (!strcasecmp(command.c_str(), "rangemeasure")) {
            if (nArg != 2) {
                throw RuntimeError(
                    "Requires 2 parameters: rangemeasure [motor] ");
            }
            string name = args[1];
            int imotor = GetMotorIndex(name);
            RestrictedForSlitAndFluorescence(name);
            controller[motor[imotor]->getController()]->rangeMeasure(
                motor[imotor]->getAxis());
            UpdateSlitLimits(name);
            oss << fixed << motor[imotor]->getPosition();
            return oss.str();
        }

        else if (!strcasecmp(command.c_str(), "moverel")) {
            if (nArg != 3) {
                throw RuntimeError(
                    "Requires 3 parameters: moverel [motor] [value]");
            }
            string name = args[1];
            int imotor = GetMotorIndex(name);
            double value = 0;
            istringstream iss(args[2].c_str());
            iss >> value;
            if (iss.fail()) {
                throw RuntimeError(
                    "Could not scan relative position argument " + args[2]);
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
                throw RuntimeError(
                    "Requires 3 parameters: moveabs [motor] [value]");
            }
            string name = args[1];
            int imotor = GetMotorIndex(name);
            double value = 0;
            istringstream iss(args[2].c_str());
            iss >> value;
            if (iss.fail()) {
                throw RuntimeError(
                    "Could not scan absolute position argument " + args[2]);
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
                throw RuntimeError(
                    "Requires 3 parameters: setpos [motor] [value]");
            }
            string name = args[1];
            int imotor = GetMotorIndex(name);
            RestrictedForSlitAndFluorescence(name);
            double value = 0;
            istringstream iss(args[2].c_str());
            iss >> value;
            if (iss.fail()) {
                throw RuntimeError("Could not scan position argument " +
                                   args[2]);
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
                throw RuntimeError(
                    "Requires 2 parameters: numflist [fluorescence motor]");
            }
            string name = args[1];
            OnlyFluorescenceCommand(name);
            int ifluor = GetFluorescenceIndex(name);
            oss << fluorescence[ifluor]->getNumTargetHolders();
            return oss.str();
        }

        else if (!strcasecmp(command.c_str(), "getholder")) {
            if (nArg != 2) {
                throw RuntimeError(
                    "Requires 2 parameters: getholder [fluorescence motor]");
            }
            string name = args[1];
            OnlyFluorescenceCommand(name);
            int ifluor = GetFluorescenceIndex(name);
            oss << fluorescence[ifluor]->getCurrentTargetHolder();
            return oss.str();
        }

        else if (!strcasecmp(command.c_str(), "setholder")) {
            if (nArg != 3) {
                throw RuntimeError("Requires 3 parameters: setholder "
                                   "[fluorescence motor] [index]");
            }
            string name = args[1];
            OnlyFluorescenceCommand(name);
            int ifluor = GetFluorescenceIndex(name);
            int value = 0;
            istringstream iss(args[2].c_str());
            iss >> value;
            if (iss.fail()) {
                throw RuntimeError("Could not scan index argument " + args[2]);
            }
            fluorescence[ifluor]->setCurrentTargetHolder(value);
            oss << value;
            return oss.str();
        }

        else if (!strcasecmp(command.c_str(), "dellastholder")) {
            if (nArg != 2) {
                throw RuntimeError("Requires 2 parameters: dellastholder "
                                   "[fluorescence motor]");
            }
            string name = args[1];
            OnlyFluorescenceCommand(name);
            int ifluor = GetFluorescenceIndex(name);
            fluorescence[ifluor]->deleteLastTargetHolder();
            oss << "Last holder deleted for " << name << ". Num holders: "
                << fluorescence[ifluor]->getNumTargetHolders();
            return oss.str();
        }

        else if (!strcasecmp(command.c_str(), "addholder")) {
            if (nArg != 2 + MAX_FLUOR_VALUES * 2) {
#ifdef XRAYBOX
                throw RuntimeError("Requires 18 parameters: addholder "
                                   "[fluorescence motor] [Target 8] "
                                   "[Energy 8] ..[Target 1] [Energy 1]");

#else
                throw RuntimeError("Requires 18 parameters: addholder "
                                   "[fluorescence motor] [Target 1] "
                                   "[Energy 1] ..[Target 8] [Energy 8]");

#endif
            }
            string name = args[1];
            OnlyFluorescenceCommand(name);
            int ifluor = GetFluorescenceIndex(name);
            std::vector<std::string> target;
            std::vector<std::string> energy;
            for (int i = 2; i < nArg; ++i) {
                target.push_back(args[i]);
                ++i;
                energy.push_back(args[i]);
            }
            fluorescence[ifluor]->addHolder(target, energy);
            oss << "Holder appended for " << name << " to position "
                << fluorescence[ifluor]->getNumTargetHolders() - 1;
            return oss.str();
        }

        else if (!strcasecmp(command.c_str(), "fllist")) {
            if (nArg != 3) {
                throw RuntimeError(
                    "Requires 3 parameters: fllist [fluorescence motor] "
                    "[holder index]");
            }
            string name = args[1];
            OnlyFluorescenceCommand(name);
            int ifluor = GetFluorescenceIndex(name);
            int index = 0;
            istringstream iss(args[2].c_str());
            iss >> index;
            if (iss.fail()) {
                throw RuntimeError("Could not scan holder index argument " +
                                   args[2]);
            }
            return fluorescence[ifluor]->getList(index);
        }

        else if (!strcasecmp(command.c_str(), "getfl")) {
            if (nArg != 2) {
                throw RuntimeError(
                    "Requires 2 parameters: getfl [fluorescence motor]");
            }
            string name = args[1];
            OnlyFluorescenceCommand(name);
            int ifluor = GetFluorescenceIndex(name);
            oss << fluorescence[ifluor]->updateAndGetCurrentTargetName();
            return oss.str();
        }

        else if (!strcasecmp(command.c_str(), "setfl")) {
            if (nArg != 3) {
                throw RuntimeError("Requires 3 parameters: setfl [fluorescence "
                                   "motor] [target name]");
            }
            string name = args[1];
            OnlyFluorescenceCommand(name);
            int ifluor = GetFluorescenceIndex(name);
            string target = args[2];
            fluorescence[ifluor]->moveToTarget(target);
            oss << fluorescence[ifluor]->updateAndGetCurrentTargetName();
            return oss.str();
        }

        else if (!strcasecmp(command.c_str(), "changefl")) {
            if (nArg != 6) {
                throw RuntimeError(
                    "Requires 6 parameters: changefl "
                    "[fluorescence motor] [holder index] [target index] "
                    "[new target name] [new target energy]");
            }
            string name = args[1];
            OnlyFluorescenceCommand(name);
            int ifluor = GetFluorescenceIndex(name);
            int holderIndex = 0;
            {
                istringstream iss(args[2].c_str());
                iss >> holderIndex;
                if (iss.fail()) {
                    throw RuntimeError("Could not scan holder index argument " +
                                       args[2]);
                }
            }
            int targetIndex = 0;
            {
                istringstream iss(args[3].c_str());
                iss >> targetIndex;
                if (iss.fail()) {
                    throw RuntimeError("Could not scan target index argument " +
                                       args[3]);
                }
            }
            string target = args[4];
            string energy = args[5];
            fluorescence[ifluor]->changeTarget(holderIndex, targetIndex, target,
                                               energy);
            return fluorescence[ifluor]->getList(holderIndex);
        }

        // ----- slits ---------------------------------------------------------

        else if (!strcasecmp(command.c_str(), "getslitwidth")) {
            if (nArg != 1) {
                throw RuntimeError("Requires 1 parameters: getslitwidth");
            }
            OnlySlitCommand();
            oss << fixed << slit->getSlitWidth();
            return oss.str();
        }

        else if (!strcasecmp(command.c_str(), "setslitwidth")) {
            if (nArg != 2) {
                throw RuntimeError(
                    "Requires 2 parameters: setslitwidth [value]");
            }
            OnlySlitCommand();
            double value = 0;
            istringstream iss(args[1].c_str());
            iss >> value;
            if (iss.fail()) {
                throw RuntimeError("Could not scan slit width value argument " +
                                   args[1]);
            }
            slit->setSlitWidth(value);
            oss << fixed << slit->getSlitWidth();
            return oss.str();
        }

        else if (!strcasecmp(command.c_str(), "getcenter")) {
            if (nArg != 1) {
                throw RuntimeError("Requires 1 parameters: getcenter");
            }
            OnlySlitCommand();
            oss << fixed << slit->getCenter();
            return oss.str();
        }

        else if (!strcasecmp(command.c_str(), "setcenter")) {
            if (nArg != 2) {
                throw RuntimeError("Requires 2 parameters: setcenter [value]");
            }
            OnlySlitCommand();
            double value = 0;
            istringstream iss(args[1].c_str());
            iss >> value;
            if (iss.fail()) {
                throw RuntimeError("Could not scan center value argument " +
                                   args[1]);
            }
            slit->setCenter(value);
            oss << fixed << slit->getCenter();
            return oss.str();
        }

        // ----- tube ----------------------------------------------------------

        if (!strcasecmp(command.c_str(), "gettubeinterface")) {
            if (nArg != 1) {
                throw RuntimeError("Requires 1 parameter: gettubeinterface ");
            }
            OnlyTubeCommand();
            oss << xrayTube->getInterface()->getSerial();
            return oss.str();
        }

        if (!strcasecmp(command.c_str(), "sendtube")) {
            if (nArg < 2) {
                throw RuntimeError(
                    "Requires 2 parameters: sendtube [commands..]");
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
                throw RuntimeError(
                    "Requires 2 parameters: readtube [commands..]");
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
            return "ok";
        }

        else if (!strcasecmp(command.c_str(), "istube")) {
            if (nArg != 1) {
                throw RuntimeError("Requires 1 parameters: istube ");
            }
            try {
                OnlyTubeCommand();
            } catch (RuntimeError &e) {
                return "off";
            }
            if (xrayTube->isOnStandby()) {
                return "standby";
            }
            return "on";
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
                throw RuntimeError("Cannot scan hv argument " + value);
            }
            xrayTube->setHVSwitch(on);
            bool result = xrayTube->getHVSwitch();
            if (result != on) {
                oss << "Could not switch " << value << " hv. Hv is "
                    << (result ? "on" : "off") << ". \n\nWarm up required.";
                throw RuntimeError(oss.str());
            }
            oss << (result ? "on" : "off");
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
            istringstream iss(args[1].c_str());
            iss >> value;
            if (iss.fail()) {
                throw RuntimeError("Could not scan voltage argument " +
                                   args[1]);
            }
            xrayTube->setVoltage(value);
            int result = xrayTube->getVoltage();
            if (result != value) {
                oss << "Could not set voltage to " << value
                    << " kV. Voltage is at " << result << " kV.";
                throw RuntimeError(oss.str());
            }
            oss << result << " kV";
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
            istringstream iss(args[1].c_str());
            iss >> value;
            if (iss.fail()) {
                throw RuntimeError("Could not scan current argument " +
                                   args[1]);
            }
            xrayTube->setCurrent(value);
            int result = xrayTube->getCurrent();
            if (result != value) {
                oss << "Could not set current to " << value
                    << " mA. Current is at " << result << " mA.";
                throw RuntimeError(oss.str());
            }
            oss << result << " mA";
            return oss.str();
        }

        else if (!strcasecmp(command.c_str(), "setvc")) {
            if (nArg != 3) {
                throw RuntimeError(
                    "Requires 3 parameters: setvc [voltage] [current]");
            }
            OnlyTubeCommand();
            int voltage = 0;
            {
                istringstream iss(args[1].c_str());
                iss >> voltage;
                if (iss.fail()) {
                    throw RuntimeError("Could not scan voltage argument " +
                                       args[1]);
                }
            }
            int current = 0;
            {
                istringstream iss(args[2].c_str());
                iss >> current;
                if (iss.fail()) {
                    throw RuntimeError("Could not scan current argument " +
                                       args[2]);
                }
            }
            xrayTube->setVoltageAndCurrent(voltage, current);
            pair<int, int> result = xrayTube->getVoltageAndCurrent();
            if (result.first != voltage) {
                oss << "Could not set voltage to " << voltage
                    << " kV. Voltage is at " << result.first << " kV.";
                throw RuntimeError(oss.str());
            }
            if (result.second != current) {
                oss << "Could not set current to " << current
                    << " mA. Current is at " << result.second << " mA.";
                throw RuntimeError(oss.str());
            }
            oss << result.first << " kV, " << result.second << " mA";
            return oss.str();
        }

        else if (!strcasecmp(command.c_str(), "getvc")) {
            if (nArg != 1) {
                throw RuntimeError("Requires 1 parameters: getvc ");
            }
            OnlyTubeCommand();
            pair<int, int> values = xrayTube->getVoltageAndCurrent();
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
            pair<int, int> values = xrayTube->getActualVoltageAndCurrent();
            oss << values.first << " kV, " << values.second << " mA";
            return oss.str();
        }

        else if (!strcasecmp(command.c_str(), "getshutter")) {
            if (nArg != 2) {
                throw RuntimeError(
                    "Requires 2 parameters: getshutter [shutter index] ");
            }
            OnlyTubeCommand();
            int index = 0;
            istringstream iss(args[1].c_str());
            iss >> index;
            if (iss.fail()) {
                throw RuntimeError("Could not scan shutter index argument " +
                                   args[1]);
            }
            oss << (xrayTube->getShutter(index) ? "on" : "off");
            return oss.str();
        }

        else if (!strcasecmp(command.c_str(), "getshutters")) {
            if (nArg != 1) {
                throw RuntimeError("Requires 1 parameters: getshutters ");
            }
            OnlyTubeCommand();
            oss << (xrayTube->getShutter(1) ? "on" : "off") << ' ';
            oss << (xrayTube->getShutter(2) ? "on" : "off") << ' ';
            oss << (xrayTube->getShutter(3) ? "on" : "off") << ' ';
            oss << (xrayTube->getShutter(4) ? "on" : "off");
            return oss.str();
        }

        else if (!strcasecmp(command.c_str(), "connectedshutters")) {
            if (nArg != 1) {
                throw RuntimeError("Requires 1 parameters: connectedshutters ");
            }
            OnlyTubeCommand();
            oss << (xrayTube->isShutterConnected(1) ? "c" : "nc") << ' ';
            oss << (xrayTube->isShutterConnected(2) ? "c" : "nc") << ' ';
            oss << (xrayTube->isShutterConnected(3) ? "c" : "nc") << ' ';
            oss << (xrayTube->isShutterConnected(4) ? "c" : "nc");
            return oss.str();
        }

        else if (!strcasecmp(command.c_str(), "shutter")) {
            if (nArg != 3) {
                throw RuntimeError(
                    "Requires 3 parameters: shutter [shutter index] [on/off]");
            }
            OnlyTubeCommand();
            int index = 0;
            istringstream iss(args[1].c_str());
            iss >> index;
            if (iss.fail()) {
                throw RuntimeError("Could not scan shutter index argument " +
                                   args[1]);
            }
            bool on = false;
            if (!strcasecmp(args[2].c_str(), "on")) {
                on = true;
            } else if (!strcasecmp(args[2].c_str(), "off")) {
                on = false;
            } else {
                throw RuntimeError("Cannot scan shutter value argument " +
                                   args[2]);
            }
            xrayTube->setShutter(index, on);
            bool result = xrayTube->getShutter(index);
            if (result != on) {
                oss << "Could not set shutter " << index << " to "
                    << (on ? "on" : "off") << ". Shutter " << index << " is "
                    << (result ? "on" : "off");
                throw RuntimeError(oss.str());
            }
            oss << (result ? "on" : "off");
            return oss.str();
        }

        else if (!strcasecmp(command.c_str(), "warmup")) {
            if (nArg != 2) {
                throw RuntimeError("Requires 2 parameters: warmup [voltage]");
            }
            OnlyTubeCommand();
            int voltage = 0;
            istringstream iss(args[1].c_str());
            iss >> voltage;
            if (iss.fail()) {
                throw RuntimeError("Could not scan warm up voltage argument " +
                                   args[1]);
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

        // ----- pressure ------------------------------------------------------

        else if (!strcasecmp(command.c_str(), "getpressureinterface")) {
            if (nArg != 1) {
                throw RuntimeError(
                    "Requires 1 parameter: getpressureinterface ");
            }
            if (pgauge == NULL) {
                try {
                    UpdateInterface(PRESSURE);
                } catch (RuntimeError &e) {
                    throw;
                }
            }
            oss << pgauge->getInterface()->getSerial();
            return oss.str();
        }

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
            oss << "1: [" << result[0].status << ", " << result[0].pressure
                << "]\n"
                   "2: ["
                << result[1].status << ", " << result[1].pressure << "]";
            return oss.str();
        }

        else if (!strcasecmp(command.c_str(), "ispressure")) {
            if (nArg != 1) {
                throw RuntimeError("Requires 1 parameters: ispressure");
            }
            try {
                if (pgauge == NULL) {
                    UpdateInterface(PRESSURE);
                } else {
                    pgauge->getPressure();
                }
            } catch (RuntimeError &e) {
                return "off";
            }
            return "on";
        }

        // ----- reference points ----------------------------------------------

        else if (!strcasecmp(command.c_str(), "nref")) {
            if (nArg != 1) {
                throw RuntimeError("Requires 1 parameters: nref");
            }
            if (referencePoints == NULL) {
                oss << 0;
            } else {
                oss << referencePoints->size();
            }
            return oss.str();
        }

        else if (!strcasecmp(command.c_str(), "reflist")) {
            if (nArg != 1) {
                throw RuntimeError("Requires 1 parameters: reflist");
            }
            if (referencePoints == NULL) {
                throw RuntimeError("No reference points added");
            }
            oss << referencePoints->getList();
            return oss.str();
        }

        else if (!strcasecmp(command.c_str(), "refvals")) {
            if (nArg != 2) {
                throw RuntimeError("Requires 2 parameters: refvals [name]");
            }
            if (referencePoints == NULL) {
                throw RuntimeError("No reference points added");
            }
            std::string name = args[1];
            vector<double> positions = referencePoints->getPositions(name);
            for (unsigned int i = 0; i < positions.size(); ++i) {
                oss << positions[i] << ' ';
            }
            return oss.str();
        }

        else if (!strcasecmp(command.c_str(), "getref")) {
            if (nArg != 1) {
                throw RuntimeError("Requires 1 parameters: getref");
            }
            if (referencePoints == NULL) {
                throw RuntimeError("No reference points added");
            }
            oss << referencePoints->getCurrentReferenceName();
            return oss.str();
        }

        else if (!strcasecmp(command.c_str(), "setref")) {
            if (nArg != 2) {
                throw RuntimeError("Requires 2 parameters: setref [name]");
            }
            if (referencePoints == NULL) {
                throw RuntimeError("No reference points added");
            }
            std::string name = args[1];
            referencePoints->moveTo(name);
            return "ok";
        }

        // ----- filter wheels -------------------------------------------------

        else if (!strcasecmp(command.c_str(), "nfw")) {
            if (nArg != 1) {
                throw RuntimeError("Requires 1 parameters: nfw");
            }
            oss << fwheel.size();
            return oss.str();
        }

        else if (!strcasecmp(command.c_str(), "fwlist")) {
            if (nArg != 1) {
                throw RuntimeError("Requires 1 parameters: fwlist");
            }
            if (fwheel.size() == 0) {
                throw RuntimeError("No filter wheels added");
            }
            for (unsigned int i = 0; i < fwheel.size(); ++i) {
                oss << fwheel[i]->getName();
                if (i < fwheel.size() - 1) {
                    oss << ' ';
                }
            }
            return oss.str();
        }

        else if (!strcasecmp(command.c_str(), "fwvals")) {
            if (nArg != 2) {
                throw RuntimeError("Requires 2 parameters: fwvals [name]");
            }
            string name = args[1];
            int ifwheel = GetFwheelIndex(name);
            vector<double> result = fwheel[ifwheel]->getValueList();
            for (unsigned int i = 0; i < result.size(); ++i) {
                oss << result[i];
                if (i < result.size() - 1) {
                    oss << ' ';
                }
            }
            return oss.str();
        }

        else if (!strcasecmp(command.c_str(), "getfw")) {
            if (nArg != 2) {
                throw RuntimeError("Requires 2 parameters: getfw [name]");
            }
            string name = args[1];
            int ifwheel = GetFwheelIndex(name);
            oss << fwheel[ifwheel]->getValue();
            return oss.str();
        }

        else if (!strcasecmp(command.c_str(), "setfw")) {
            if (nArg != 3) {
                throw RuntimeError(
                    "Requires 3 parameters: setfw [name] [value]");
            }
            string name = args[1];
            int ifwheel = GetFwheelIndex(name);
            double value = 0;
            istringstream iss(args[2].c_str());
            iss >> value;
            if (iss.fail()) {
                throw RuntimeError(
                    "Could not scan filter wheel value argument " + args[2]);
            }
            fwheel[ifwheel]->setValue(value);
            oss << fwheel[ifwheel]->getValue();
            return oss.str();
        }

        else if (!strcasecmp(command.c_str(), "getfwserialnumber")) {
            if (nArg != 2) {
                throw RuntimeError(
                    "Requires 2 parameters: getfwserialnumber [name]");
            }
            string name = args[1];
            int ifwheel = GetFwheelIndex(name);
            oss << fwheel[ifwheel]->getSerialNumber();
            return oss.str();
        }

        else if (!strcasecmp(command.c_str(), "getfwinterface")) {
            if (nArg != 2) {
                throw RuntimeError(
                    "Requires 2 parameters: getfwinterface [name]");
            }
            string name = args[1];
            int ifwheel = GetFwheelIndex(name);
            oss << fwheel[ifwheel]->getInterface()->getSerial();
            return oss.str();
        }

        // ----- unknown command -----------------------------------------------

        throw RuntimeError("Unknown command " + command);
    }

    // delete xray class if tube switched off
    catch (const TubeOffError &e) {
        if (xrayTube != NULL) {
            int port = xrayTube->getInterface()->getSerialPortNumber();
            usbSerialPortsUsed[port] = false;
            LOG(logINFO) << "Setting interface /ttyUSB" << port
                         << ": Not in use";
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
            LOG(logINFO) << "Setting interface /ttyUSB" << port
                         << ": Not in use";
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
    : xrayTube(NULL), pgauge(NULL), referencePoints(NULL), slit(NULL),
      maxTubePower(0) {

    for (unsigned int i = 0; i < MAX_USB_SERIAL_PORTS; ++i) {
        usbSerialPortsUsed.push_back(false);
    }

    ReadConfigFile();

    // Validations
    // min 1 motor
    if (motor.size() == 0) {
        throw RuntimeError("No motor found. At least 1 motor required.");
    }
    // min 1 motor per controller
    for (unsigned int i = 0; i < controller.size(); ++i) {
        if (controller[i]->getMotor(0) == NULL &&
            controller[i]->getMotor(1) == NULL &&
            controller[i]->getMotor(2) == NULL) {
            throw RuntimeError("Controller " + controller[i]->getName() +
                               " has no motors connected to it.");
        }
    }
    // min 1 target (1 target list) for each fluorescence motor
    for (unsigned int i = 0; i < fluorescence.size(); ++i) {
        if (fluorescence[i]->getNumTargetHolders() == 0) {
            throw RuntimeError("No targets added to " +
                               fluorescence[i]->getName());
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
    LOG(logINFO) << "Reading config file" << endl;
    ifstream inFile;
    inFile.open(CONFIG_FILE, ifstream::in);
    if (!inFile.is_open()) {
        throw RuntimeError("Could not open config file.");
    }
    while (inFile.good()) {
        string sLine;
        getline(inFile, sLine);
        // delete lines after comments
        if (sLine.find('#') != string::npos) {
            sLine.erase(sLine.find('#'));
        }
        LOG(logDEBUG) << "line:" << sLine << endl;
        // scan arguments
        istringstream iss(sLine);
        vector<string> args = vector<string>(istream_iterator<string>(iss),
                                             istream_iterator<string>());
        for (unsigned int i = 0; i < args.size(); ++i) {
            LOG(logDEBUG) << i << ":[" << args[i]
                          << "] length:" << args[i].length() << endl;
        }
        // blank lines
        if (args.size() == 0 || args[0].empty()) {
            continue;
        }
        string mode = args[0];
        if (mode == "power") {
            TubeMode(args);
        } else if (mode == "pressure") {
            PressureMode();
        } else if (mode == "fwheel") {
            FwheelMode(args);
        } else if (mode == "refpoint") {
            ReferencePointMode(args);
        } else if (mode == "fluorescence") {
            FluorescenceMode(args);
        } else if (mode == "fluorescence_wheel") {
            FluorescenceMode(args);
        } else if (mode == "controller") {
            ControllerMode(args);
        } else if (mode == "motor") {
            MotorMode(args);
        } else {
            throw RuntimeError("Cannot scan command type " + mode +
                               " in config file");
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
        lastIndex = static_cast<int>(controller.size()) - 1;
    } else if (index == FILTER_WHEEL) {
        lastIndex = static_cast<int>(fwheel.size()) - 1;
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
            Interface *interface = new Interface(usbport, i, index);
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
                if (Controller::CheckControllerSerialNumber(
                        interface, controller[lastIndex]->getSerialNumber())) {
                    usbSerialPortsUsed[i] = true;
                    controller[lastIndex]->setInterface(interface);
                    return;
                }
                break;
            case FILTER_WHEEL:
                if (Fwheel::CheckFWSerialNumber(
                        i, fwheel[lastIndex]->getSerialNumber())) {
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
        oss << TUBE_OFF_ERROR_PHRASE << ". Could not find usb serial port.";
        throw TubeOffError(oss.str());
    case PRESSURE:
        oss << PRESSUR_OFF_ERROR_PHRASE << ". Could not find usb serial port.";
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
        oss << "Incorrect number of arguments for power. Expected 2, got "
            << args.size();
        throw RuntimeError(oss.str());
    }

    // tube power
    istringstream iss(args[1].c_str());
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
        xrayTube->setMaxPower(maxTubePower);
    } catch (RuntimeError &e) {
        LOG(logWARNING) << "Xray Tube is probably switched off. Continuing.";
    }
    cout << endl;
}

void Initialize::PressureMode() {
    // get pressure interface
    try {
        UpdateInterface(PRESSURE);
    } catch (RuntimeError &e) {
        LOG(logWARNING)
            << "Pressure Gauge is probably switched off. Continuing.";
    }
    cout << endl;
}

void Initialize::FwheelMode(vector<string> args) {
    if (args.size() != CONFIG_FWHEEL_NUM_ARGS) {
        ostringstream oss;
        oss << "Incorrect number of arguments for fwheel. Expected "
            << CONFIG_FWHEEL_NUM_ARGS << ", got " << args.size();
        throw RuntimeError(oss.str());
    }

    // parse
    string name = args[1];
    string serialNumber = args[2];
    vector<double> values(CONFIG_FWHEEL_NUM_ARGS - 3);
    for (unsigned int i = 0; i < values.size(); ++i) {
        istringstream iss(args[3 + i].c_str());
        iss >> values[i];
        if (iss.fail()) {
            throw RuntimeError("Could not scan fwheel absorption values " +
                               args[3 + i]);
        }
    }

    // create fwheel object
    fwheel.push_back(new Fwheel(static_cast<int>(fwheel.size()), name, serialNumber, values));

    // get fwheel interface
    try {
        UpdateInterface(FILTER_WHEEL);
    } catch (RuntimeError &e) {
        throw RuntimeError("Could not get usb port for Fwheel " + name);
    }
    cout << endl;
}

void Initialize::ReferencePointMode(vector<string> args) {
    if (args.size() != CONFIG_REF_POINT_NUM_ARGS) {
        ostringstream oss;
        oss << "Incorrect number of arguments for reference point. Expected "
            << CONFIG_REF_POINT_NUM_ARGS << ", got " << args.size();
        throw RuntimeError(oss.str());
    }

    // parse
    string name = args[1];
    vector<double> positions;
    for (unsigned int i = 2; i < args.size(); ++i) {
        istringstream iss(args[i].c_str());
        double pos;
        iss >> pos;
        if (iss.fail()) {
            throw RuntimeError("Could not scan refpoint positions for " +
                               args[i]);
        }
        positions.push_back(pos);
    }

    // first referencepoints object
    if (referencePoints == NULL) {
        vector<Controller *> refController;
        vector<int> axis;
        int numPositionMotors = 3;
        const char *motorNames[] = {"Detector_x", "Detector_y", "Detector_z"};
        try {
            for (int i = 0; i < numPositionMotors; ++i) {
                int imotor = GetMotorIndex(motorNames[i]);
                refController.push_back(
                    controller[motor[imotor]->getController()]);
                axis.push_back(motor[imotor]->getAxis());
            }
        } catch (...) {
            throw RuntimeError(
                "Could not add reference point. Missing Detector_x, "
                "Detector_y or Detector_z motor");
        }
        referencePoints = new ReferencePoints(refController, axis);
    }

    referencePoints->add(name, positions);
}

void Initialize::FluorescenceMode(vector<string> args) {
    if (args.size() != CONFIG_FLUOR_NUM_ARGS) {
        ostringstream oss;
        oss << "Incorrect number of arguments for "
               "fluorescence/fluorescence_wheel. "
               "Expected "
            << CONFIG_FLUOR_NUM_ARGS << ", got " << args.size();
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
    if (args.size() != CONFIG_CONTROLLER_NUM_ARGS) {
        ostringstream oss;
        oss << "Incorrect number of arguments for controller. Expected "
            << CONFIG_CONTROLLER_NUM_ARGS << ", got " << args.size();
        throw RuntimeError(oss.str());
    }

    // parse
    string name = args[1];
    string serialNumber = args[2];

    // validations
    for (unsigned int i = 0; i < controller.size(); ++i) {
        if (name == controller[i]->getName() ||
            serialNumber == controller[i]->getSerialNumber()) {
            throw RuntimeError("Two controllers cannot have the same name " +
                               name + " or serial number " + serialNumber);
        }
    }

    // create controller object
    controller.push_back(new Controller(static_cast<int>(controller.size()), name, serialNumber));

    // get controller interface
    try {
        UpdateInterface(CONTROLLER);
    } catch (RuntimeError &e) {
        throw RuntimeError("Could not find usb serial port for controller " +
                           args[1]);
    }
    cout << endl;
}

void Initialize::MotorMode(vector<string> args) {
    if (args.size() != CONFIG_MOTOR_NUM_ARGS) {
        ostringstream oss;
        oss << "Incorrect number of arguments for motor. Expected "
            << CONFIG_MOTOR_NUM_ARGS << ", got " << args.size();
        throw RuntimeError(oss.str());
    }

    // parse
    string name = args[1];
    int axis = -1;
    {
        istringstream iss(args[2].c_str());
        iss >> axis;
        if (iss.fail()) {
            throw RuntimeError("Could not scan axis for motor argument: " +
                               args[2]);
        }
        --axis;
    }
    string controllerName = args[3];
    double lowerLimit = -1;
    {
        istringstream iss(args[4].c_str());
        iss >> lowerLimit;
        if (iss.fail()) {
            throw RuntimeError(
                "Could not scan lowerLimit for motor argument: " + args[4]);
        }
    }
    double upperLimit = -1;
    {
        istringstream iss(args[5].c_str());
        iss >> upperLimit;
        if (iss.fail()) {
            throw RuntimeError(
                "Could not scan upperLimit for motor argument: " + args[5]);
        }
    }

    // validations
    for (unsigned int i = 0; i < motor.size(); ++i) {
        if (name == motor[i]->getName()) {
            throw RuntimeError("Two motors cannot have the same name " + name);
        }
    }
    // axis validated inside controller->setMotor
    // limits validated inside motor
    // find controller index by name
    int icontroller = GetControllerIndex(controllerName);

    // create motor object
    motor.push_back(new Motor(static_cast<int>(motor.size()), name, axis, icontroller, lowerLimit,
                              upperLimit));
    controller[icontroller]->setMotor(axis, motor[motor.size() - 1]);

    // if slit_x2, create slit object

    // if fluorescence, create fluorescence object
    if (name == "Fluorescence" || name == "Fluorescence_wheel") {
        fluorescence.push_back(new Fluorescence(static_cast<int>(fluorescence.size()), name,
                                                controller[icontroller], axis));
    } else if (name == "Slit_x2") {
        int imotorX1 = GetMotorIndex("Slit_x1");
        Controller *controller_slitx1 =
            controller[motor[imotorX1]->getController()];
        int iaxisX1 = motor[imotorX1]->getAxis();
        slit =
            new Slit(controller_slitx1, controller[icontroller], iaxisX1, axis);
    }
    cout << endl;
}
