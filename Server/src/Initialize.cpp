#include "Initialize.h"
#include "Xray.h"
#include "Pgauge.h"
#include "Fwheel.h"
#include "ReferencePoint.h"
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


#define CONFIG_POWER_NUM_ARGS       (2)
#define CONFIG_FWHEEL_NUM_ARGS      (9)
#define CONFIG_REF_POINT_NUM_ARGS	(5)
#define CONFIG_FLUOR_NUM_ARGS		(3)
#define CONFIG_CONTROLLER_NUM_ARGS  (3)
#define CONFIG_MOTOR_NUM_ARGS       (6)

void Initialize::OnlyFluorescenceCommand(std::string name) {
	if ((strcasecmp(name.c_str(), "fluorescence")) && (strcasecmp(name.c_str(), "fluorescence_wheel"))) {
		throw RuntimeError("Command not allowed for this motor " + name);
	}
}

void Initialize::RestrictedForSlitAndFluorescence(std::string name) {
	if ((!strcasecmp(name.c_str(), "slit_x1")) || (!strcasecmp(name.c_str(), "slit_x2")) || (!strcasecmp(name.c_str(), "fluorescence")) || (!strcasecmp(name.c_str(), "fluorescence_wheel"))) {
		throw RuntimeError("Command not allowed for this motor " + name);
	}
}

void Initialize::UpdateSlitLimits(string name) {
	if ((!strcasecmp(name.c_str(), "slit_x1")) || (!strcasecmp(name.c_str(), "slit_x2"))) {
		slit->updateLimits();
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
	std::string command = args[0];
	int nArg = (int)args.size();
	ostringstream oss;

/*
	for (int i = 0; i < nArg; ++i) {
		FILE_LOG(logINFO) << i << ":" << args[i];
	}
*/

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
		UpdateSlitLimits(name);
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
		std::istringstream iss (args[2].c_str());
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
		std::istringstream iss (args[2].c_str());
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
		std::istringstream iss (args[2].c_str());
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
		std::istringstream iss (args[2].c_str());
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
		std::istringstream iss (args[2].c_str());
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
		std::istringstream iss (args[2].c_str());
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
		oss << fluorescence[ifluor]->getCurrentTargetName(); //debug, calculate again
		return oss.str();
	}		

/*

	// --- if command is getfl'----------------
	else if(command == "getfl")
	{
		// if number of parameters are wrong
		if(nArg!=1)
		{
			strcpy(mess, "ERROR: Required number of parameters: 1");
			return -1;
		}

		for(int i=0;i<motor.size();i++)
		{
			if("Fluorescence" == motor[i]->getName())
			{
				newPosition = motor[i]->getPosition();
				cout << "new position: " << newPosition << endl;
#ifdef XRAYBOX
				// laser position
				if(fabs(laserPosition - newPosition) < 0.0001 )
				{
					sprintf(mess,"Fl is %s and value:%f", FLUOR_LASERNAME, newPosition);
					return 0;
				}
#endif
				// out of limits
				if (newPosition < fluoroffset || newPosition > (maxfluorvalues - 1) * fluorwidth) {
					sprintf(mess,"ERROR: Fluroescence position %f is out of limits",newPosition);
					return -1;
				}

				int ipos = round((newPosition - fluoroffset) / fluorwidth);
				double calcPosition = fluoroffset + (ipos * fluorwidth);

				// somewhere in between
				if (fabs(calcPosition - newPosition) > 0.00001) {
					cout << "Position is somewhere in between" << endl;
					sprintf(mess,"ERROR: Fluroescence position %f is out of limits",newPosition);
					return -1;
				}

				sprintf(mess,"Fl is %s and value:%f",fluorList[ipos][0].c_str(), newPosition);
				return 0;
			}
		}
		sprintf(mess,"ERROR: Fluroescence motor does not exist in config file");
		return -1;
	}



	// --- if command is movefl'-------------------------------------------------------------------
	else if(command == "movefl")
	{
		int found=0;
		bool fluorNameFound = false;
		// if number of parameters are wrong
		if(nArg!=2)
		{
			strcpy(mess, "ERROR: Required number of parameters: 2");
			return -1;
		}

		for(int i=0;i<motor.size();i++)
		{
			if("Fluorescence" == motor[i]->getName())
			{
				found=1;
				// loops through to find fluor name
				for(int j=0;j<maxfluorvalues;j++)
				{
					if(args[1] == fluorList[j][0].c_str())
					{
						fluorNameFound = true;
						newPosition = fluoroffset + (j * fluorwidth);
					}
				}
#ifdef XRAYBOX
				// if not in list (check for laser)
				if (!strcasecmp(FLUOR_LASERNAME, args[1]))
				{
					fluorNameFound = true;
					newPosition = laserPosition;
				}
#endif

				// if fluor name found (or laser)
				if (fluorNameFound) {

					if(!motor[i]->canMotorMove(newPosition))
					{
						sprintf(mess, "ERROR: Position given to move motor %s is beyond its limits: %f and %f",args[1],motor[i]->getLowerLimit(),motor[i]->getUpperLimit());
						return -1;
					}

					motor[i]->moveAbs(newPosition,0,0,0);
					//set position member of motor to the updated position
					motor[i]->setPosition(newPosition);
					sprintf(mess,"Moved to %s: Fl value:%f",args[1],newPosition);
					return 0;
				}
			}
		}

		if(!found)
		{
			sprintf(mess,"ERROR: Fluroescence motor does not exist in config file");
			return -1;
		}
		if (!fluorNameFound)
		{
			sprintf(mess,"ERROR: %s does not exist in fluorescence list in config file", args[1]);
			return -1;
		}

		sprintf(mess,"ERROR: This fluroescence value does not exist in config file");
		return -1;

	}


















	//------------------------------------------------- laser box specific list  ---------------------------------------------------------------------
	// --- if command is checkref---------------------------------------
	if(command == "checkref")
	{
		// if number of parameters are wrong
		if(nArg!=1)
		{
			strcpy(mess, "ERROR: Required number of parameters: 1\nHelp: checkref");
			return -1;
		}

		double x,y,z;
		char *commands[15];
		for(int j=0;j<15;j++)
		    commands[j]=new char[1000];
		strcpy(commands[0],"pos");
		//gets the position of x
		strcpy(commands[1],"detector_x");
		//if it returns an error, return an error
		if(executeCommand(2,commands,mess)==-1)
			return -1;
		else
			x=atof(mess);
		//gets the position of y
		strcpy(commands[1],"detector_y");
		//if it returns an error, return an error
		if(executeCommand(2,commands,mess)==-1)
			return -1;
		else
			y=atof(mess);
		//gets the position of z
		strcpy(commands[1],"detector_z");
		//if it returns an error, return an error
		if(executeCommand(2,commands,mess)==-1)
			return -1;
		else
			z=atof(mess);


		for(int i=0;i<referencePoints.size();i++)
		{
			bool bx=false,by=false,bz=false;
			//if its -1, then you dont care bout that axis
			if((atof(referencePoints[i][1].c_str())<0)||(x==atof(referencePoints[i][1].c_str())))
					bx=true;
			if((atof(referencePoints[i][2].c_str())<0)||(y==atof(referencePoints[i][2].c_str())))
					by=true;
			if((atof(referencePoints[i][3].c_str())<0)||(z==atof(referencePoints[i][3].c_str())))
					bz=true;
			if(bx&&by&&bz)
			{
				strcpy(mess,referencePoints[i][0].c_str());
				return 0;
			}
		}
		strcpy(mess,"None");
		return 0;

	}

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


	// --- if command is refvals---------------------------------------
	else if(command == "refvals")
	{
		//if it doesnt exist, the error message in the end
		num=4;
		// if number of parameters are wrong
		if(nArg!=2)
		{
			strcpy(mess, "ERROR: Required number of parameters: 2\nHelp: refvals [reference_name]");
			return -1;
		}
		strcpy(mess,"");
		//find the refpoint
		for(int i=0;i<referencePoints.size();i++)
			if(!strcasecmp(args[1],referencePoints[i][0].c_str()))
			{
				for(int j=1;j<=3;j++)
				{
					strcat(mess,referencePoints[i][j].c_str());
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


	// --- if command is reflist---------------------------------------
	else if(command == "reflist")
	{
		// if number of parameters are wrong
		if(nArg!=1)
		{
			strcpy(mess, "ERROR: Required number of parameters: 1\nHelp: reflist");
			return -1;
		}
		//all the filter wheels
		for(int i=0;i<referencePoints.size();i++)
		{
			strcat(mess," ");
			strcat(mess,referencePoints[i][0].c_str());
		}
		return 0;

	}


	// --- if command is ref---------------------------------------
	else if(command == "ref")
	{
		num=4;
		// if number of parameters are wrong
		if(nArg!=2)
		{
			strcpy(mess, "ERROR: Required number of parameters: 2\nHelp: ref [reference_point_name]\n");
			return -1;
		}
		//find the ref point
		for(int i=0;i<referencePoints.size();i++)
		{
			if(!strcasecmp(args[1],referencePoints[i][0].c_str()))
			{
				double x,y,z;
				char *commands[15];
				for(int j=0;j<15;j++)
				    commands[j]=new char[1000];
				//get the positions from the vector(from file)
				x= atof(referencePoints[i][1].c_str());
				y= atof(referencePoints[i][2].c_str());
				z= atof(referencePoints[i][3].c_str());
				//detx
				if(x>=0)
				{	//form command
					strcpy(commands[0],"moveabs");
					strcpy(commands[1],"detector_x");
					sprintf(commands[2],"%f",x);
					cout<<"mess:"<<mess<<"."<<endl;
					//if it returns an error, return an error
					if(executeCommand(3,commands,mess)==-1)
						return -1;
				}
				//dety
				if(y>=0)
				{	//form command
					strcpy(commands[0],"moveabs");
					strcpy(commands[1],"detector_y");
					sprintf(commands[2],"%f",y);
					//if it returns an error, return an error
					if(executeCommand(3,commands,mess)==-1)
						return -1;
				}
				//detz
				if(z>=0)
				{	//form command
					strcpy(commands[0],"moveabs");
					strcpy(commands[1],"detector_z");
					sprintf(commands[2],"%f",z);
					//if it returns an error, return an error
					if(executeCommand(3,commands,mess)==-1)
						return -1;
				}
				sprintf(mess,"Moved to reference point %s. (%f,%f,%f)",args[1],x,y,z);
				return 0;
			}
		}
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













	int slitnum=0;
	double midpos;


	// --- if command is getpower----------------------------------------
	if(command == "getpower")
	{
		// if number of parameters are wrong
		if(nArg!=1)
		{
			strcpy(mess, "ERROR: Required number of parameters: 1");
			return -1;
		}

		sprintf(mess,"%d",maxTubePower);
		return 0;
	}

	// --- if command is readwarmuptiming----------------only for gui----
	else if(command == "readwarmuptiming")
	{
		// if number of parameters are wrong
		if(nArg!=2)
		{
			strcpy(mess, "ERROR: Required number of parameters: 2");
			return -1;
		}

		// if voltage is not a number
		temp.assign(args[1]);
		if(temp.find_first_not_of("0123456789")!=string::npos)
		{
			sprintf(mess, "ERROR: %s for voltage should be a positive number",args[1]);
			return -1;
		}
		int voltage = atoi(args[1]);
		if((voltage>60)||(voltage<2))
		{
			sprintf(mess, "ERROR: %skV should be greater than 1 and less than 60(kV)",args[1]);
			return -1;
		}


		sprintf(mess,warmupTimings[voltage].c_str());
		return 0;
	}

	// --- if command is writewarmuptiming----------------only for gui----
	else if(command == "writewarmuptiming")
	{
		string s_time;
		time_t tdate;

		// if number of parameters are wrong
		if(nArg!=2)
		{
			strcpy(mess, "ERROR: Required number of parameters: 2");
			return -1;
		}

		// if voltage is not a number
		temp.assign(args[1]);
		if(temp.find_first_not_of("0123456789")!=string::npos)
		{
			sprintf(mess, "ERROR: %s for voltage should be a positive number",args[1]);
			return -1;
		}
		int voltage = atoi(args[1]);
		if((voltage>60)||(voltage<2))
		{
			sprintf(mess, "ERROR: %skV should be greater than 1 and less than 60(kV)",args[1]);
			return -1;
		}

		//getting the current time stamp
		time(&tdate);
		s_time = ctime(&tdate);
		s_time.erase(s_time.begin()+24,s_time.end());
		for(int i=voltage;i>=0;i--)
			warmupTimings[i].assign(s_time);
		cout<<"\n timestamp for voltage "<<voltage<<" is :"<<warmupTimings[voltage]<<endl;


		sprintf(mess,warmupTimings[voltage].c_str());
		return 0;
	}


	// --- if command is createxrayport------------------creates xray port and class-------------------------
	else if(command == "createxrayport")
	{

		// if number of parameters are wrong
		if(nArg!=1)
		{
			strcpy(mess, "ERROR: Required number of parameters: 1");
			return -1;
		}
		if(xrayTube != NULL)
		{
			strcpy(mess, "ERROR: The Serial Port and Xray class has ALREADY been created before");
			return -1;
		}
		bool success = false;

		//check through all the usb devices connected
		char serial[200]="/dev/ttyUSBX";
		int usbNum=-1;
		bool used;
		while(!success){
			usbNum++;
			if(usbNum >=10) break;
			serial[11]=usbNum+48;
			used=false;
			for(int i=0;i<controller.size();i++)
				if(!(strcmp(interface[i]->getSerial(),serial))){
					used=true;
					break;
				}
			// no need to check filter wheels as this command is not for laser box
#ifdef VACUUMBOX
			if(pgauge != NULL && (!(strcmp(pgauge->getInterface()->getSerial(),serial))))
			    used=true;
#endif
			if(!used) {
				if (xrayTube==NULL){
					Interface* tubeInterface = new Interface(serial , &success, true);
					if(success){
						xrayTube = new Xray(tubeInterface);
#ifdef XRAYBOX
						strcpy(mess, "Serial Port to the xray tube has been created successfully");
						return 0;
#endif
					}else {
                        xrayTube = NULL;
					}
				}
				else if(!(strcmp(xrayTube->getInterface()->getSerial(),serial))){
					used=true;
				}
			}
		}

		if (xrayTube == NULL) {
			strcpy(mess, "ERROR: Unable to create serial port for X-Ray Tube or the tube is switched off.");
			return -1;
		}

		//should not reach here
		return -1;
	}


    // --- if command is pressure------------------creates pgauge if doesnt exist, gets pressure-------------------------
    else if(command == "pressure")
    {

        // if number of parameters are wrong
        if(nArg!=1)
        {
            strcpy(mess, "ERROR: Required number of parameters: 1");
            return -1;
        }
        if(pgauge == NULL) {

            bool success = false;
            //check through all the usb devices connected
            char serial[200]="/dev/ttyUSBX";
            bool used;
            for(int usbNum=0;usbNum<10;usbNum++) {
                serial[11]=usbNum+48;
                used=false;
                for(int i=0;i<controller.size();i++) {
                    if(!(strcmp(interface[i]->getSerial(),serial))){
                        used=true;
                        break;
                    }
                }
                // no need to check filter wheels as this command is not for laser box
                if((xrayTube != NULL) && (!(strcmp(xrayTube->getInterface()->getSerial(),serial))))
                    used=true;
                if(!used) {
                    Interface* pressureInterface = new Interface(serial, true, &success);
                    if(success){
                        pgauge = new Pgauge(pressureInterface);
                        printf("Serial Port to the Pressure Gauge Controller has been created successfully\n");
                        break;
                    }else {
                        pgauge = NULL;
                    }
                }
            }
        }

        if (pgauge == NULL) {
            strcpy(mess, "ERROR: Unable to create Pressure Gauge Controller or the pump is switched off.");
            return -1;
        }

        string status1="";double value1=-1;string status2="";double value2=-1;
        int ret = 0;
        //retries
        for (int i = 0; i < 3; i ++) {
            ret = pgauge->getPressure(status1,value1,status2,value2);
            if (ret)
                break;
        }

        if (!ret) {
            strcpy(mess, "ERROR: Could not get pressure\n");
            return -1;
        }

        sprintf(mess,"Gauge 1 [Status: %s, Pressure: %e], Gauge 2 [Status: %s, Pressure: %e]\n",
                status1.c_str(), value1, status2.c_str(), value2);
        return 0;
    }



	// --- if command is geterr-----------------------------------------------
	else if(command == "geterr")
	{

		// if number of parameters are wrong
		if(nArg!=1)
		{
			strcpy(mess, "ERROR: Required number of parameters: 1");
			return -1;
		}

		if(xrayTube==NULL)
		{
			strcpy(mess, "ERROR: The Xray Tube class has not been instantiated before");
			return -1;
		}

		sprintf(mess, "%d",xrayTube->getErrorCode());
		return 0;
	}

	// --- if command is getemess-----------------------------------------------
	else if(command == "getemess")
	{

		// if number of parameters are wrong
		if(nArg!=1)
		{
			strcpy(mess, "ERROR: Required number of parameters: 1");
			return -1;
		}

		if(xrayTube==NULL)
		{
			strcpy(mess, "ERROR: The Xray Tube class has not been instantiated before");
			return -1;
		}

		strcpy(mess,xrayTube->getErrorMessage().c_str());
		return 0;
	}


	// --- if command is clear----------------------------------------------
	else if(command == "clear")
	{

		// if number of parameters are wrong
		if(nArg!=1)
		{
			strcpy(mess, "ERROR: Required number of parameters: 1");
			return -1;
		}

		if(xrayTube==NULL)
		{
			strcpy(mess, "ERROR: The Xray Tube class has not been instantiated before");
			return -1;
		}

		xrayTube->clearErrorCode();
		sprintf(mess, "The Error code has been cleared");
		return 0;
	}



	// --- if command is xrayStatus-------this si to check if xray class has been created or not------------
	else if(command == "xrayStatus")
	{

		// if number of parameters are wrong
		if(nArg!=1)
		{
			strcpy(mess, "ERROR: Required number of parameters: 1");
			return -1;
		}

		sprintf(mess, "%d",xrayTube == NULL ? -1 : 0);
		return 0;
	}




	// --- if command is isStandby--------------------------------------------------------------------------------
	else if(command == "isStandby")
	{
		// if number of parameters are wrong
		if(nArg!=1)
		{
			strcpy(mess, "ERROR: Required number of parameters: 1");
			return -1;
		}

		if(xrayTube==NULL)
		{
			strcpy(mess, "ERROR: The Xray Tube class has not been instantiated before");
			return -1;
		}

		if(xrayTube->isOnStandby()==1)
			strcpy(mess,"Yes, the X-Ray tube is on standby");
		else if(xrayTube->isOnStandby()==-9999)
			strcpy(mess,"The X-Ray tube is switched off");
		else
			strcpy(mess,"No, the X-Ray tube is NOT on standby");
		return 0;
	}




	// --- if command is getHV-------------------------------------------------------------------------------
	else if(command == "getHV")
	{
		// if number of parameters are wrong
		if(nArg!=1)
		{
			strcpy(mess, "ERROR: Required number of parameters: 1");
			return -1;
		}

		if(xrayTube==NULL)
		{
			strcpy(mess, "ERROR: The Xray Tube class has not been instantiated before");
			return -1;
		}

		if(xrayTube->getHVSwitch())
			strcpy(mess,"Yes, the HV is switched ON");
		else
			strcpy(mess,"No, the HV is NOT switched ON");
		return 0;
	}



	// --- if command is getV---------------------------------------------------------------------------------
	else if(command == "getV")
	{
		// if number of parameters are wrong
		if(nArg!=1)
		{
			strcpy(mess, "ERROR: Required number of parameters: 1");
			return -1;
		}

		if(xrayTube==NULL)
		{
			strcpy(mess, "ERROR: The Xray Tube class has not been instantiated before");
			return -1;
		}

		sprintf(mess,"%d",xrayTube->getVoltage());
		return 0;
	}



	// --- if command is getActualV---------------------------------------------------------------------------------
	else if(command == "getActualV")
	{
		// if number of parameters are wrong
		if(nArg!=1)
		{
			strcpy(mess, "ERROR: Required number of parameters: 1");
			return -1;
		}

		if(xrayTube==NULL)
		{
			strcpy(mess, "ERROR: The Xray Tube class has not been instantiated before");
			return -1;
		}



		sprintf(mess,"%d",xrayTube->getVoltageActual());
		return 0;
	}


	// --- if command is getC---------------------------------------------------------------------------------
	else if(command == "getC")
	{
		// if number of parameters are wrong
		if(nArg!=1)
		{
			strcpy(mess, "ERROR: Required number of parameters: 1");
			return -1;
		}

		if(xrayTube==NULL)
		{
			strcpy(mess, "ERROR: The Xray Tube class has not been instantiated before");
			return -1;
		}

		sprintf(mess,"%d",xrayTube->getCurrent());
		return 0;
	}



	// --- if command is getActualC---------------------------------------------------------------------------------
	else if(command == "getActualC")
	{
		// if number of parameters are wrong
		if(nArg!=1)
		{
			strcpy(mess, "ERROR: Required number of parameters: 1");
			return -1;
		}

		if(xrayTube==NULL)
		{
			strcpy(mess, "ERROR: The Xray Tube class has not been instantiated before");
			return -1;
		}


		sprintf(mess,"%d",xrayTube->getCurrentActual());
		return 0;
	}



	// --- if command is getVandC---------------------------------------------------------------------------------
	else if(command == "getVandC")
	{
		// if number of parameters are wrong
		if(nArg!=1)
		{
			strcpy(mess, "ERROR: Required number of parameters: 1");
			return -1;
		}

		if(xrayTube==NULL)
		{
			strcpy(mess, "ERROR: The Xray Tube class has not been instantiated before");
			return -1;
		}

		int voltage,current;
		xrayTube->getVoltageAndCurrent(voltage,current);
		sprintf(mess,"Voltage:%d, Current:%d.",voltage,current);
		return 0;
	}



	// --- if command is getActualVandC---------------------------------------------------------------------------------
	else if(command == "getActualVandC")
	{
		// if number of parameters are wrong
		if(nArg!=1)
		{
			strcpy(mess, "ERROR: Required number of parameters: 1");
			return -1;
		}

		if(xrayTube==NULL)
		{
			strcpy(mess, "ERROR: The Xray Tube class has not been instantiated before");
			return -1;
		}

		int voltage,current;
		xrayTube->getVoltageAndCurrentActual(voltage,current);
		sprintf(mess,"Actual Voltage:%d, Actual Current:%d.",voltage,current);
		return 0;
	}



	// --- if command is getshutter1---------------------------------------------------------------------------------
	else if(command == "getshutter1")
	{
		// if number of parameters are wrong
		if(nArg!=1)
		{
			strcpy(mess, "ERROR: Required number of parameters: 1");
			return -1;
		}

		if(xrayTube==NULL)
		{
			strcpy(mess, "ERROR: The Xray Tube class has not been instantiated before");
			return -1;
		}

		sprintf(mess,"Shutter 1:"); if(xrayTube->getShutter1()) strcat(mess,"ON  "); else strcat(mess,"OFF  ");
		return 0;
	}


	// --- if command is getshutter2---------------------------------------------------------------------------------
	else if(command == "getshutter2")
	{
		// if number of parameters are wrong
		if(nArg!=1)
		{
			strcpy(mess, "ERROR: Required number of parameters: 1");
			return -1;
		}

		if(xrayTube==NULL)
		{
			strcpy(mess, "ERROR: The Xray Tube class has not been instantiated before");
			return -1;
		}

		sprintf(mess,"Shutter 2:"); if(xrayTube->getShutter2()) strcat(mess,"ON  "); else strcat(mess,"OFF  ");
		return 0;
	}



	// --- if command is getshutter3---------------------------------------------------------------------------------
	else if(command == "getshutter3")
	{
		// if number of parameters are wrong
		if(nArg!=1)
		{
			strcpy(mess, "ERROR: Required number of parameters: 1");
			return -1;
		}

		if(xrayTube==NULL)
		{
			strcpy(mess, "ERROR: The Xray Tube class has not been instantiated before");
			return -1;
		}

		sprintf(mess,"Shutter 3:"); if(xrayTube->getShutter3()) strcat(mess,"ON  "); else strcat(mess,"OFF  ");
		return 0;
	}


	// --- if command is getshutter4---------------------------------------------------------------------------------
	else if(command == "getshutter4")
	{
		// if number of parameters are wrong
		if(nArg!=1)
		{
			strcpy(mess, "ERROR: Required number of parameters: 1");
			return -1;
		}

		if(xrayTube==NULL)
		{
			strcpy(mess, "ERROR: The Xray Tube class has not been instantiated before");
			return -1;
		}

		sprintf(mess,"Shutter 4:"); if(xrayTube->getShutter4()) strcat(mess,"ON  "); else strcat(mess,"OFF  ");
		return 0;
	}



	// --- if command is getshutters---------------------------------------------------------------------------------
	else if(command == "getshutters")
	{
		// if number of parameters are wrong
		if(nArg!=1)
		{
			strcpy(mess, "ERROR: Required number of parameters: 1");
			return -1;
		}

		if(xrayTube==NULL)
		{
			strcpy(mess, "ERROR: The Xray Tube class has not been instantiated before");
			return -1;
		}
		int s1,s2,s3,s4;
		xrayTube->getShutters(s1,s2,s3,s4);
		sprintf(mess,"Shutter 1:"); if(s1) strcat(mess,"ON  "); else strcat(mess,"OFF  ");
		strcat(mess,"Shutter 2:"); if(s2) strcat(mess,"ON  "); else strcat(mess,"OFF  ");
		strcat(mess,"Shutter 3:"); if(s3) strcat(mess,"ON  "); else strcat(mess,"OFF  ");
		strcat(mess,"Shutter 4:"); if(s4) strcat(mess,"ON  "); else strcat(mess,"OFF  ");
		return 0;
	}




	// --- if command is HV---------------------------------------------------------------------------------
	else if(command == "HV")
	{
		// if number of parameters are wrong
		if(nArg!=2)
		{
			strcpy(mess, "ERROR: Required number of parameters: 2");
			return -1;
		}

		if(!( (!strcasecmp(args[1],"on"))||(!strcasecmp(args[1],"off")) ))
		{
			strcpy(mess, "ERROR: The values for HV should be either 'on' or 'off'");
			return -1;
		}
		if(xrayTube==NULL)
		{
			strcpy(mess, "ERROR: The Xray Tube class has not been instantiated before");
			return -1;
		}

		int value;
		if(!strcasecmp(args[1],"on"))
			value=1;
		else if(!strcasecmp(args[1],"off"))
			value=0;

		xrayTube->setHVSwitch(value);
		sprintf(mess,"The High Voltage has been switched %s",args[1]);
		return 0;
	}



	// --- if command is shutter---------------------------------------------------------------------------------
	else if(command == "shutter")
	{
		// if number of parameters are wrong
		if(nArg!=3)
		{
			strcpy(mess, "ERROR: Required number of parameters: 3");
			return -1;
		}

		if(!( (!strcasecmp(args[1],"1"))||(!strcasecmp(args[1],"2"))|| (!strcasecmp(args[1],"3"))||(!strcasecmp(args[1],"4"))  ))
		{
			strcpy(mess, "ERROR: The shutter Id should be 1,2,3 or 4");
			return -1;
		}
		if(!( (!strcasecmp(args[2],"on"))||(!strcasecmp(args[2],"off")) ))
		{
			strcpy(mess, "ERROR: The values for shutter should be either 'on' or 'off'");
			return -1;
		}
		if(xrayTube==NULL)
		{
			strcpy(mess, "ERROR: The Xray Tube class has not been instantiated before");
			return -1;
		}

		bool value;
		if(!strcasecmp(args[2],"on"))
			value=true;
		else if(!strcasecmp(args[2],"off"))
			value=false;


		xrayTube->setShutter(atoi(args[1]),value);
		sprintf(mess,"Shutter %s has been switched %s",args[1],args[2]);
		return 0;
	}



	// --- if command is setv---------------------------------------------------------------------------------
	else if(command == "setv")
	{
		// if number of parameters are wrong
		if(nArg!=2)
		{
			strcpy(mess, "ERROR: Required number of parameters: 2");
			return -1;
		}

		if(xrayTube==NULL)
		{
			strcpy(mess, "ERROR: The Xray Tube class has not been instantiated before");
			return -1;
		}

		// if voltage is not a number
		temp.assign(args[1]);
		if(temp.find_first_not_of("0123456789")!=string::npos)
		{
			sprintf(mess, "ERROR: %s for voltage should be a positive number",args[1]);
			return -1;
		}
		int voltage = atoi(args[1]);
		if((voltage>60)||(voltage<2))
		{
			sprintf(mess, "ERROR: %skV should be greater than 1 and less than 60(kV)",args[1]);
			return -1;
		}

		if((voltage*0.001*xrayTube->getCurrent()) > maxTubePower)
		{
			sprintf(mess, "ERROR: This voltage %skV exceeds the max power of %dW since the current is set to %dmA",args[1],maxTubePower,(xrayTube->getCurrent()/1000));
			return -1;
		}
		xrayTube->setVoltage(voltage);
		sprintf(mess,"The voltage %skV has been set for the X-Ray Tube",args[1]);
		return 0;
	}


	// --- if command is setc---------------------------------------------------------------------------------
	else if(command == "setc")
	{
		// if number of parameters are wrong
		if(nArg!=2)
		{
			strcpy(mess, "ERROR: Required number of parameters: 2");
			return -1;
		}

		if(xrayTube==NULL)
		{
			strcpy(mess, "ERROR: The Xray Tube class has not been instantiated before");
			return -1;
		}

		// if current is not a number
		temp.assign(args[1]);
		if(temp.find_first_not_of("0123456789")!=string::npos)
		{
			sprintf(mess, "ERROR: %s for current should be a positive number",args[1]);
			return -1;
		}

		int current = atoi(args[1]);
		if((current>80)||(current<2))
		{
			sprintf(mess, "ERROR: %smA should be greater than 1 and less than 80(mA)",args[1]);
			return -1;
		}

		if((current*0.001*xrayTube->getVoltage())>maxTubePower)
		{
			sprintf(mess, "ERROR: This current %smA exceeds the max power of %dW since the voltage is set to %dkV",args[1],maxTubePower,(xrayTube->getVoltage()/1000));

			return -1;
		}
		xrayTube->setCurrent(current);
		sprintf(mess,"The current %smA has been set for the X-Ray Tube",args[1]);
		return 0;
	}



	// --- if command is setvc---------------------------------------------------------------------------------
	else if(command == "setvc")
	{
		// if number of parameters are wrong
		if(nArg!=3)
		{
			strcpy(mess, "ERROR: Required number of parameters: 3");
			return -1;
		}

		if(xrayTube==NULL)
		{
			strcpy(mess, "ERROR: The Xray Tube class has not been instantiated before");
			return -1;
		}

		// if voltage is not a number
		temp.assign(args[1]);
		if(temp.find_first_not_of("0123456789")!=string::npos)
		{
			sprintf(mess, "ERROR: %s for voltage should be a positive number",args[1]);
			return -1;
		}

		// if current is not a number
		temp.assign(args[2]);
		if(temp.find_first_not_of("0123456789")!=string::npos)
		{
			sprintf(mess, "ERROR: %s for current should be a positive number",args[2]);
			return -1;
		}

		int voltage = atoi(args[1]);
		int current = atoi(args[2]);
		if((current<2)||(voltage<2))
		{
			strcpy(mess, "ERROR: The voltage and current should be greater than 1");
			return -1;
		}
		if(voltage>60)
		{
			sprintf(mess, "ERROR: %dkV should be less than 60(kV)",voltage);
			return -1;
		}
		if(current>80)
		{
			sprintf(mess, "ERROR: %dmA should be less than 80(mA)",current);
			return -1;
		}

		cout<<"voltage:"<<voltage<<endl;
		cout<<"current:"<<current<<endl;

		if((current*voltage)>maxTubePower)
		{
			sprintf(mess, "ERROR: This voltage %dkV and current %dmA will exceed the max power of %dW",voltage,current,maxTubePower);
			return -1;
		}
		xrayTube->setVoltageAndCurrent(voltage,current);
		sprintf(mess,"The voltage %skV and current %smA has been set for the X-Ray Tube",args[1],args[2]);
		return 0;
	}




	// --- if command is warmup---------------------------------------------------------------------------------
	else if(command == "warmup")
	{
		// if number of parameters are wrong
		if(nArg!=2)
		{
			strcpy(mess, "ERROR: Required number of parameters: 2");
			return -1;
		}

		if(xrayTube==NULL)
		{
			strcpy(mess, "ERROR: The Xray Tube class has not been instantiated before");
			return -1;
		}

		// if voltage is not a number
		temp.assign(args[1]);
		if(temp.find_first_not_of("0123456789")!=string::npos)
		{
			sprintf(mess, "ERROR: %s for warmup voltage should be a positive number",args[1]);
			return -1;
		}

		int voltage = atoi(args[1]);
		if((voltage>60)||(voltage<2))
		{
			sprintf(mess, "ERROR: %s for warmup voltage should be greater than 1 and less than 60(kV)",args[1]);
			return -1;
		}
		if(xrayTube->isOnStandby()==1)
		{
			sprintf(mess, "ERROR: The X-Ray tube is on standby!");
			return -1;
		}
		if(xrayTube->isOnStandby()==-9999)
		{
			sprintf(mess, "ERROR: The X-Ray tube is switched off!");
			return -1;
		}

		xrayTube->startWarmup(voltage);
		xrayTube->getErrorCode();
		xrayTube->setHVSwitch(1);
		sprintf(mess,"The X-Ray tube has started warming up");
		return 0;
	}



	// --- if command is getwtime---------------------------------------------------------------------------------
	else if(command == "getwtime")
	{
		// if number of parameters are wrong
		if(nArg!=1)
		{
			strcpy(mess, "ERROR: Required number of parameters: 1");
			return -1;
		}

		if(xrayTube==NULL)
		{
			strcpy(mess, "ERROR: The Xray Tube class has not been instantiated before");
			return -1;
		}

		sprintf(mess,"%d",xrayTube->getWarmupTimeRemaining());
		return 0;
	}



	// --- if command is issafe---------------------------------------------------------------------------------
	else if(command == "issafe")
	{
		// if number of parameters are wrong
		if(nArg!=1)
		{
			strcpy(mess, "ERROR: Required number of parameters: 1");
			return -1;
		}

		if(xrayTube==NULL)
		{
			strcpy(mess, "ERROR: The Xray Tube class has not been instantiated before. Do that first");
			return -1;
		}
		if(xrayTube->isAccessPossible())
			sprintf(mess,"Yes, You may open the door now!");
		else
			sprintf(mess,"No, You may NOT open the door now! What were you thinking???");
		return 0;
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
	: xrayTube(NULL), maxTubePower(0), pgauge(NULL), slit(NULL) {

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
	std::cout << "Motors: " << motor.size() << std::endl;
	std::cout << "==========" << std::endl;
	for (unsigned int i = 0; i < motor.size(); ++i) {
		motor[i]->print();
	}	
	if (referencePoint.size() > 0) {
		std::cout << "Reference Points: " << referencePoint.size() << std::endl;
		std::cout << "===================" << std::endl;
		for (unsigned int i = 0; i < referencePoint.size(); ++i) {
			referencePoint[i]->print();
		}	
	}
	if (fluorescence.size() > 0) {
		std::cout << "Fluorescence Motors: " << fluorescence.size() << std::endl;
		std::cout << "======================" << std::endl;
		for (unsigned int i = 0; i < fluorescence.size(); ++i) {
			fluorescence[i]->print();
		}	
	}
	if (slit != NULL) {
		std::cout << "Slit" << std::endl;
		std::cout << "====" << std::endl;
		slit->print();
	}
	if (fwheel.size() > 0) {
		std::cout << "Filter Wheels: " << fwheel.size() << std::endl;
		std::cout << "================" << std::endl;
		for (unsigned int i = 0; i < fwheel.size(); ++i) {
			fwheel[i]->print();
		}	
	}	
	std::cout << "Controllers: " << controller.size() << std::endl;
	std::cout << "==============" << std::endl;
	for (unsigned int i = 0; i < controller.size(); ++i) {
		controller[i]->print();
	}	
	if (xrayTube != NULL) {
		std::cout<< "Xray Tube" << std::endl;
		std::cout << "========" << std::endl;
		std:: cout << "\tUsb port:" << xrayTube->getInterface()->getSerial() << std::endl;
	}
	if (pgauge != NULL) {
		std::cout<< "Pressure Gauge" << std::endl;
		std::cout << "=============" << std::endl;
		std:: cout << "\tUsb port:" << pgauge->getInterface()->getSerial() << std::endl;
	}
}


int Initialize::ReadWarmupTimestamps(string const fName) {
#ifdef VERY_VERBOSE
	cout<<"\n-----Reading WarmupTimeStamps-----"<<endl<<endl;
#endif 

	//reading the values from a file to fill the warmupTimings vector
	string sLine;
	ifstream inFile;
	inFile.open(fName.c_str(),ifstream::in);
	if(inFile.is_open())
	{
		while(inFile.good())
		{
			getline(inFile, sLine);

			// Looks out for comments
			if(sLine.find('#')!=string::npos)
			{
				continue;
			}
			//Looks out for empty lines
			else if(sLine.length()<1)
			{
				continue;
			}
			else if(sLine.find("unknown")!=string::npos)
			{
				continue;
			}
			else
			{
				//converts the first part:voltage to a number
				int i = atoi((sLine.substr(0,2)).c_str());
				//gets only the second part:warmup Timestamp
				sLine.erase(0,15);
				//assigns the warmup time for the corresponding voltage value in vector
				warmupTimings[i].assign(sLine);
				//cout<<"The cut string at "<<i<<" is :"<<warmupTimings[i]<<":"<<endl;
			}
		}
	}
	else
		cout<<"WARNING: Could not open warmup timestamps file\n\n";

#ifdef VERY_VERBOSE
	cout<<"\n-----Done reading WarmupTimeStamps-----"<<endl<<endl;
#endif

	return 0;
}

int Initialize::WriteWarmupTimestamps(string const fName) {
#ifdef VERY_VERBOSE
	cout<<"\n-----Writing WarmupTimeStamps-----"<<endl<<endl;
#endif 

	//writing the values to a file before closing window
	ofstream outFile;
	outFile.open(fName.c_str());
	if(outFile.is_open())
	{
		outFile<<setw(15)<<left<<"#Voltage"<<"Last Successful Warmup Timestamp\n\n";

		for(int i=2;i<(int)warmupTimings.size();i++)
			outFile<<setw(15)<<left<<i<<warmupTimings[i]<<endl;

		outFile.close();
	}
	else
		cout<<"WARNING: Could not open file to write warmup timestamps\n";

#ifdef VERY_VERBOSE
	cout<<"\n-----Done writing WarmupTimeStamps-----"<<endl<<endl;
#endif

	return 0;
}

void Initialize::ReadConfigFile() {
	FILE_LOG(logINFO) << "Reading config file" << std::endl;
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
		FILE_LOG(logDEBUG) << "line:"<< sLine << std::endl;
		// scan arguments
		istringstream iss(sLine);
		vector<string> args = vector<string>(istream_iterator<string>(iss), istream_iterator<string>());
		for (unsigned int i = 0; i < args.size(); ++i) {
			FILE_LOG(logDEBUG) << i << ":[" << args[i] << "] length:" << args[i].length() << std::endl;
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
		std::string usbport = oss.str();

		try {
			Interface* interface = new Interface(usbport, index);
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
	throw RuntimeError("Could not find usb serial port");
}



void Initialize::TubeMode(vector<string> args) {
	if (args.size() != CONFIG_POWER_NUM_ARGS) {
		ostringstream oss;
		oss << "Incorrect number of arguments for power. Expected 2, got " << args.size() ;
		throw RuntimeError(oss.str());
	}

	// tube power
	std::istringstream iss (args[1].c_str());
	iss >> maxTubePower;
	if (iss.fail()) {
		throw RuntimeError("Could not scan power argument " + args[1]);
	}
	if (maxTubePower < 0) {
		throw RuntimeError("Invalid power " + args[1]);
	}

	// get tube interface
	try {
		UpdateInterface(TUBE);
	} catch (RuntimeError &e) {
		FILE_LOG(logWARNING) << "Xray Tube is probably switched off. Continuing.";
	}	

	// read warm up timings
	warmupTimings.resize(61);
	for (unsigned int i = 0; i < warmupTimings.size(); ++i) {
		warmupTimings[i].assign("unknown");
	}
	ReadWarmupTimestamps(WARMUP_FILE);
	std::cout << std::endl;
}


void Initialize::PressureMode() {
	// get pressure interface
	try {
		UpdateInterface(PRESSURE);
	} catch (RuntimeError &e) {
		FILE_LOG(logWARNING) << "Pressure Gauge is probably switched off. Continuing.";
	}	
	std::cout << std::endl;
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
		std::istringstream iss (args[3 + i].c_str());
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
	std::cout << std::endl;
 }


 void Initialize::ReferencePointMode(vector<string> args) {
	if (args.size()  != CONFIG_REF_POINT_NUM_ARGS) {
		ostringstream oss;
		oss << "Incorrect number of arguments for reference point. Expected " << CONFIG_REF_POINT_NUM_ARGS << ", got " << args.size() ;
		throw RuntimeError(oss.str());
	}

	// parse
	string name = args[1];
	vector<double> positions(3);
	for (unsigned int i = 0; i < 3; ++i) {
		std::istringstream iss (args[2 + i].c_str());
		iss >> positions[i];
		if (iss.fail()) {
			throw RuntimeError("Could not scan refpoint positions for " + args[2 + i]);
		}
	}

	// create referencepoints object
	referencePoint.push_back(new ReferencePoint(referencePoint.size(), name, positions));
	std::cout << std::endl;
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
	std::cout << std::endl;
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
		std::istringstream iss (args[2].c_str());
		iss >> axis;
		if (iss.fail()) {
			throw RuntimeError("Could not scan axis for motor argument: " + args[2]);
		}
		--axis;
	}
	string controllerName = args[3];
	double lowerLimit = -1;
	{
		std::istringstream iss (args[4].c_str());
		iss >> lowerLimit;
		if (iss.fail()) {
			throw RuntimeError("Could not scan lowerLimit for motor argument: " + args[4]);
		}
	}
	double upperLimit = -1;
	{
		std::istringstream iss (args[5].c_str());
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
	if (axis < 0 || axis >= MAX_NUM_MOTORS_PER_CONTROLLER) {
		std::ostringstream oss; 
		oss << "Invalid axis index " << axis << " to set motor " << name;
		throw RuntimeError(oss.str());			
	}
	if (lowerLimit > upperLimit) {
		std::ostringstream oss; 
		oss << "Invalid limits to set motor " << name;
		throw RuntimeError(oss.str());			
	}

	// find controller index by name
	int icontroller = GetControllerIndex(controllerName);

	// create motor object
	motor.push_back(new Motor(motor.size(), name, axis, icontroller, lowerLimit, upperLimit));
	controller[icontroller]->setMotor(axis, motor[motor.size() - 1]);


	// if slit_x2, create slit object

	// if fluorescence, create fluorescence object
	if (name == "Fluorescence" || name == "Fluorescence_wheel") {
		fluorescence.push_back(new Fluorescence(fluorescence.size(), name, controller[icontroller]));
	} else if (name == "Slit_x2") {
		int imotorX1 = GetMotorIndex("Slit_x1");
		Controller* controller_slitx1 = controller[motor[imotorX1]->getController()];
		int iaxisX1 = motor[imotorX1]->getAxis();
		slit = new Slit(controller_slitx1, controller[icontroller], iaxisX1, axis);
	}
	std::cout << std::endl;
 }