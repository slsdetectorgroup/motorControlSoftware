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




int Initialize::executeCommand(std::vector<std::string> args, char mess[]) {
	string temp;
	int num=1;
	double newPosition;
	double newPosition2;

/*
	//------------------------------------------------- laser box specific list  ---------------------------------------------------------------------
	// --- if command is checkref---------------------------------------
	if(args[0] == "checkref")
	{
		// if number of parameters are wrong
		if(args.size()!=1)
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
	else if(args[0] == "fvals")
	{
		//if it doesnt exist, the error message in the end
		num=0;
		// if number of parameters are wrong
		if(args.size()!=2)
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
	else if(args[0] == "refvals")
	{
		//if it doesnt exist, the error message in the end
		num=4;
		// if number of parameters are wrong
		if(args.size()!=2)
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
	else if(args[0] == "fwlist")
	{
		// if number of parameters are wrong
		if(args.size()!=1)
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
	else if(args[0] == "reflist")
	{
		// if number of parameters are wrong
		if(args.size()!=1)
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
	else if(args[0] == "ref")
	{
		num=4;
		// if number of parameters are wrong
		if(args.size()!=2)
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
	else if(args[0] == "fsetval")
	{
		num=0;
		// if number of parameters are wrong
		if(args.size()!=3)
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
	else if(args[0] == "fgetval")
	{
		num=0;
		// if number of parameters are wrong
		if(args.size()!=2)
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
	if(args[0] == "getpower")
	{
		// if number of parameters are wrong
		if(args.size()!=1)
		{
			strcpy(mess, "ERROR: Required number of parameters: 1");
			return -1;
		}

		sprintf(mess,"%d",maxTubePower);
		return 0;
	}

	// --- if command is readwarmuptiming----------------only for gui----
	else if(args[0] == "readwarmuptiming")
	{
		// if number of parameters are wrong
		if(args.size()!=2)
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
	else if(args[0] == "writewarmuptiming")
	{
		string s_time;
		time_t tdate;

		// if number of parameters are wrong
		if(args.size()!=2)
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

	// --- if command is numflist----------------
	else if(args[0] == "numflist")
	{
		// if number of parameters are wrong
		if(args.size()!=1)
		{
			strcpy(mess, "ERROR: Required number of parameters: 1");
			return -1;
		}

		sprintf(mess, "%d", (int)fluorListArray.size());
		return 0;
	}

	// --- if command is whichflist----------------
	else if(args[0] == "whichflist")
	{
		// if number of parameters are wrong
		if(args.size()!=1)
		{
			strcpy(mess, "ERROR: Required number of parameters: 1");
			return -1;
		}

		if(!fluorListArray.size())
		{
			strcpy(mess, "ERROR: Fluroescence motor does not exist in config file");
			cout<<"\nin here\n";
			return -1;
		}

		// loop through each target holder
		for (size_t i = 0; i < fluorListArray.size(); ++i)
		{
			if (fluorList == fluorListArray[i]) {
				sprintf(mess,"%d", (int)i +1);
				return 0;
			}

		}

		// not in any list
		strcpy(mess, "ERROR: Fluroescence list have inconsistent values");
		return -1;
	}

	// --- if command is loadflist'----------------
	else if(args[0] == "loadflist")
	{
		// if number of parameters are wrong
		if(args.size()!=2)
		{
			strcpy(mess, "ERROR: Required number of parameters: 2");
			return -1;
		}

		if(!fluorListArray.size())
		{
			strcpy(mess, "ERROR: Fluroescence motor does not exist in config file");
			return -1;
		}

		int val = atoi(args[1]);

		if (val > 0 && (size_t)val <= fluorListArray.size()) {
			fluorList = fluorListArray[val - 1];
			sprintf(mess,"The fl list has been updated to %d", val);
			return 0;
		}

		sprintf(mess, "ERROR: Fl list %d doesnt exist in the config file", val);
		return -1;
	}


	// --- if command is getfl'----------------
	else if(args[0] == "getfl")
	{
		// if number of parameters are wrong
		if(args.size()!=1)
		{
			strcpy(mess, "ERROR: Required number of parameters: 1");
			return -1;
		}

		for(int i=0;i<Motor::NumMotors;i++)
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
	else if(args[0] == "movefl")
	{
		int found=0;
		bool fluorNameFound = false;
		// if number of parameters are wrong
		if(args.size()!=2)
		{
			strcpy(mess, "ERROR: Required number of parameters: 2");
			return -1;
		}

		for(int i=0;i<Motor::NumMotors;i++)
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



	// --- if command is createxrayport------------------creates xray port and class-------------------------
	else if(args[0] == "createxrayport")
	{

		// if number of parameters are wrong
		if(args.size()!=1)
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
    else if(args[0] == "pressure")
    {

        // if number of parameters are wrong
        if(args.size()!=1)
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
	else if(args[0] == "geterr")
	{

		// if number of parameters are wrong
		if(args.size()!=1)
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
	else if(args[0] == "getemess")
	{

		// if number of parameters are wrong
		if(args.size()!=1)
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
	else if(args[0] == "clear")
	{

		// if number of parameters are wrong
		if(args.size()!=1)
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
	else if(args[0] == "xrayStatus")
	{

		// if number of parameters are wrong
		if(args.size()!=1)
		{
			strcpy(mess, "ERROR: Required number of parameters: 1");
			return -1;
		}

		sprintf(mess, "%d",xrayTube == NULL ? -1 : 0);
		return 0;
	}




	// --- if command is isStandby--------------------------------------------------------------------------------
	else if(args[0] == "isStandby")
	{
		// if number of parameters are wrong
		if(args.size()!=1)
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
	else if(args[0] == "getHV")
	{
		// if number of parameters are wrong
		if(args.size()!=1)
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
	else if(args[0] == "getV")
	{
		// if number of parameters are wrong
		if(args.size()!=1)
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
	else if(args[0] == "getActualV")
	{
		// if number of parameters are wrong
		if(args.size()!=1)
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
	else if(args[0] == "getC")
	{
		// if number of parameters are wrong
		if(args.size()!=1)
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
	else if(args[0] == "getActualC")
	{
		// if number of parameters are wrong
		if(args.size()!=1)
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
	else if(args[0] == "getVandC")
	{
		// if number of parameters are wrong
		if(args.size()!=1)
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
	else if(args[0] == "getActualVandC")
	{
		// if number of parameters are wrong
		if(args.size()!=1)
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
	else if(args[0] == "getshutter1")
	{
		// if number of parameters are wrong
		if(args.size()!=1)
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
	else if(args[0] == "getshutter2")
	{
		// if number of parameters are wrong
		if(args.size()!=1)
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
	else if(args[0] == "getshutter3")
	{
		// if number of parameters are wrong
		if(args.size()!=1)
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
	else if(args[0] == "getshutter4")
	{
		// if number of parameters are wrong
		if(args.size()!=1)
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
	else if(args[0] == "getshutters")
	{
		// if number of parameters are wrong
		if(args.size()!=1)
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
	else if(args[0] == "HV")
	{
		// if number of parameters are wrong
		if(args.size()!=2)
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
	else if(args[0] == "shutter")
	{
		// if number of parameters are wrong
		if(args.size()!=3)
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
	else if(args[0] == "setv")
	{
		// if number of parameters are wrong
		if(args.size()!=2)
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
	else if(args[0] == "setc")
	{
		// if number of parameters are wrong
		if(args.size()!=2)
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
	else if(args[0] == "setvc")
	{
		// if number of parameters are wrong
		if(args.size()!=3)
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
	else if(args[0] == "warmup")
	{
		// if number of parameters are wrong
		if(args.size()!=2)
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
	else if(args[0] == "getwtime")
	{
		// if number of parameters are wrong
		if(args.size()!=1)
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
	else if(args[0] == "issafe")
	{
		// if number of parameters are wrong
		if(args.size()!=1)
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






	// --- if command is setPos---------------------------------------
	else if(args[0] == "setPos")
	{
		char positions[3][200],command[200];
		for(int i=0;i<3;i++) strcpy(positions[i],"0");

		// if number of parameters are wrong
		if(args.size()!=3)
		{
			strcpy(mess, "ERROR: Required number of parameters: 3");
			return -1;
		}

#ifndef LASERBOX
		//should not be able to set positions for slits or for fluorescence
		if((strcasestr(args[1],"slit")!=NULL) ||(strcasestr(args[1],"fluorescence")))
		{
			sprintf(mess, "ERROR: Position for %s cannot be set to a different value",args[1]);
			return -1;
		}
#endif
		// if position is not a number
		temp.assign(args[2]);
		if(temp.find_first_not_of("0123456789.-")!=string::npos)
		{
			sprintf(mess, "ERROR: %s for position should be a number",args[2]);
			return -1;
		}

		for(int i=0;i<Motor::NumMotors;i++)
			if(!strcasecmp(args[1],motor[i]->getName()))
			{
				sprintf(positions[motor[i]->getAxis()-1],"%f",0 - atof(args[2]));

				for(int j=0;j<Motor::NumMotors;j++)
					if((!strcmp(motor[i]->getController(),motor[j]->getController())) && (motor[i]->getAxis()!=motor[j]->getAxis()))
						sprintf(positions[motor[j]->getAxis()-1],"%f",0 - motor[j]->getPosition());

				sprintf(command,"%s %s %s setpos ",positions[0],positions[1],positions[2]);
				motor[i]->getInterface()->send_command(command,0);
				motor[i]->setPosition(atof(args[2]));
				sprintf(mess,"The cuurent position for motor %s has been set to %s",args[1],args[2]);
				return 0;
			}
	}



	// --- if command is setLower---------------------------------------
	else if(args[0] == "setLower")
	{
		// if number of parameters are wrong
		if(args.size()!=3)
		{
			strcpy(mess, "ERROR: Required number of parameters: 3");
			return -1;
		}

		// if limit is not a number
		temp.assign(args[2]);
		if(temp.find_first_not_of("0123456789.-")!=string::npos)
		{
			sprintf(mess, "ERROR: %s for lower limit should be a number",args[2]);
			return -1;
		}

		for(int i=0;i<Motor::NumMotors;i++)
			if(!strcasecmp(args[1],motor[i]->getName()))
			{
				if(atof(args[2])>motor[i]->getUpperLimit())
				{
					sprintf(mess, "ERROR: %s for lower limit should not be greater than the motor's upper limit of %f",args[2],motor[i]->getUpperLimit());
					return -1;
				}
				motor[i]->setLowerLimit(atof(args[2]));
				sprintf(mess,"The Lower Limit for motor %s has been changed to %s",args[1],args[2]);
				return 0;
			}
	}




	// --- if command is setUpper---------------------------------------
	else if(args[0] == "setUpper")
	{
		// if number of parameters are wrong
		if(args.size()!=3)
		{
			strcpy(mess, "ERROR: Required number of parameters: 3");
			return -1;
		}

		// if limit is not a number
		temp.assign(args[2]);
		if(temp.find_first_not_of("0123456789.-")!=string::npos)
		{
			sprintf(mess, "ERROR: %s for upper limit should be a number",args[2]);
			return -1;
		}

		for(int i=0;i<Motor::NumMotors;i++)
			if(!strcasecmp(args[1],motor[i]->getName()))
			{
				if(atof(args[2])<motor[i]->getLowerLimit())
				{
					sprintf(mess, "ERROR: %s for upper limit should not be less than the motor's lower limit of %f",args[2],motor[i]->getLowerLimit());
					return -1;
				}
				motor[i]->setUpperLimit(atof(args[2]));
				sprintf(mess,"The Upper Limit for motor %s has been changed to %s",args[1],args[2]);
				return 0;
			}
	}




	// --- if command is getSpeed---------------------------------------
	else if(args[0] == "getSpeed")
	{
		// if number of parameters are wrong
		if(args.size()!=2)
		{
			strcpy(mess, "ERROR: Required number of parameters: 2");
			return -1;
		}

		for(int i=0;i<Motor::NumMotors;i++)
			if(!strcasecmp(args[1],motor[i]->getName()))
			{
				sprintf(mess,"The Speed for motor %s is %f",args[1],motor[i]->getSpeed());
				return 0;
			}
	}




	// --- if command is setSpeed---------------------------------------
	else if(args[0] == "setSpeed")
	{
		// if number of parameters are wrong
		if(args.size()!=3)
		{
			strcpy(mess, "ERROR: Required number of parameters: 3");
			return -1;
		}

		// if speed is not a number
		temp.assign(args[2]);
		if(temp.find_first_not_of("0123456789.")!=string::npos)
		{
			sprintf(mess, "ERROR: %s for speed should be a positive number",args[2]);
			return -1;
		}

		for(int i=0;i<Motor::NumMotors;i++)
			if(!strcasecmp(args[1],motor[i]->getName()))
			{
				motor[i]->setSpeed(atof(args[2]));
				sprintf(mess,"The Speed for motor %s has been changed to %s",args[1],args[2]);
				return 0;
			}
	}


	// --- if command is getX1Limit---------------------------------------
	else if(args[0] == "getX1Limit")
	{

		// if number of parameters are wrong
		if(args.size()!=1)
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
	else if(args[0] == "getX2Limit")
	{

		// if number of parameters are wrong
		if(args.size()!=1)
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



	// --- if command is getUpperLimit---------------------------------------
	else if(args[0] == "getUpperLimit")
	{

		// if number of parameters are wrong
		if(args.size()!=2)
		{
			strcpy(mess, "ERROR: Required number of parameters: 2");
			return -1;
		}
		for(int i=0;i<Motor::NumMotors;i++)
			if(!strcasecmp(args[1],motor[i]->getName()))
			{
				sprintf(mess,"%f",motor[i]->getUpperLimit());
				return 0;
			}
	}



	// --- if command is getLowerLimit---------------------------------------
	else if(args[0] == "getLowerLimit")
	{

		// if number of parameters are wrong
		if(args.size()!=2)
		{
			strcpy(mess, "ERROR: Required number of parameters: 2");
			return -1;
		}
		for(int i=0;i<Motor::NumMotors;i++)
			if(!strcasecmp(args[1],motor[i]->getName()))
			{
				sprintf(mess,"%f",motor[i]->getLowerLimit());
				return 0;
			}
	}


	// --- if command is cal---------------------------------------
	else if(args[0] == "cal")
	{
		// if number of parameters are wrong
		if(args.size()!=2)
		{
			strcpy(mess, "ERROR: Required number of parameters: 2");
			return -1;
		}
		for(int i=0;i<Motor::NumMotors;i++)
			if(!strcasecmp(args[1],motor[i]->getName()))
			{
				motor[i]->calibrate();
				motor[i]->setPosition(0);
				sprintf(mess,"Motor %s has been calibrated and moved to position %f",args[1],motor[i]->getPosition());
				return 0;
			}
	}



	// --- if command is list---------------------------------------
	else if(args[0] == "list")
	{
		char *p= new char[1];
		// if number of parameters are wrong
		if(args.size()!=1)
		{
			strcpy(mess, "ERROR: Required number of parameters: 1");
			return -1;
		}
		sprintf(mess,"%d ",Motor::NumMotors);

		// find motor name (case insensitive) and execute cmd
		for(int i=0;i<Motor::NumMotors;i++)
		{
			p = motor[i]->getController();
			//to get the last digit of the controller name
			strcat(mess,p+strlen(motor[i]->getController())-1);
			strcat(mess," ");
			strcat(mess,motor[i]->getName());
			strcat(mess," ");
		}
		return 0;

	}

	// --- if command is fllist---------------------------------------
	else if(args[0] == "fllist")
	{
		// if number of parameters are wrong
		if(args.size()!=1)
		{
			strcpy(mess, "ERROR: Required number of parameters: 1");
			return -1;
		}
		if(!fluorListArray.size())
		{
			strcpy(mess, "ERROR: Fluorescence motor does not exist in the config file");
			return -1;
		}

		strcpy(mess,"");
		for(size_t i = 0; i < maxfluorvalues; ++i)
		{
			for(size_t j = 0; j < fluorList[i].size(); ++j)
			{
				strcat(mess,fluorList[i][j].c_str());
				// space used in sstr.good()
				if ((i == (maxfluorvalues - 1)) && (j == (fluorList[i].size() - 1)))
					;
				else
					strcat(mess," ");
			}
		}
		return 0;
	}


	// --- if command is pos---------------------------------------
	else if(args[0] == "pos")
	{
		// if number of parameters are wrong
		if(args.size()!=2)
		{
			strcpy(mess, "ERROR: Required number of parameters: 2");
			return -1;
		}
		// find motor name (case insensitive) and execute cmd
		for(int i=0;i<Motor::NumMotors;i++)
			if(args[1] == motor[i]->getName())
			{
				sprintf(mess,"%f",motor[i]->getPosition());
				return 0;
			}

	}




	// --- if command is depos for debugging---------------------------
	else if(args[0] == "depos")
	{
		// if number of parameters are wrong
		if(args.size()!=2)
		{
			strcpy(mess, "ERROR: Required number of parameters: 2");
			return -1;
		}
		// find motor name (case insensitive) and execute cmd
		for(int i=0;i<Motor::NumMotors;i++)
			if(args[1] ==motor[i]->getName())
			{
				sprintf(mess,"%f",motor[i]->debugPosition());
				return 0;
			}
	}



	// --- if command is getcenter---------------------------------------
	else if(args[0] == "getcenter")
	{
		//if slits are not included in config file
		if(slit == NULL)
		{
			strcpy(mess, "ERROR: The slit motors do not exist in the config file.");
			return -1;
		}
		// if number of parameters are wrong
		if(args.size()!=1)
		{
			strcpy(mess, "ERROR: Required number of parameters: 1");
			return -1;
		}

		sprintf(mess,"%f",slit->getX1Center());
		return 0;
	}



	// --- if command is getslitwidth---------------------------------------
	else if(args[0] == "getslitwidth")
	{
		//if slits are not included in config file
		if(slit == NULL)
		{
			strcpy(mess, "ERROR: The slit motors do not exist in the config file.");
			return -1;
		}
		// if number of parameters are wrong
		if(args.size()!=1)
		{
			strcpy(mess, "ERROR: Required number of parameters: 1");
			return -1;
		}

		sprintf(mess,"%f",slit->getSlitWidth());
		return 0;
	}






	// --- if command is widthrel---------------------------------------
	else if(args[0] == "widthrel")
	{
		//if slits are not included in config file
		if(slit == NULL)
		{
			strcpy(mess, "ERROR: The slit motors do not exist in the config file.");
			return -1;
		}

		double halfWidth;
		// if number of parameters are wrong
		if(args.size()!=2)
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
	else if(args[0] == "widthabs")
	{
		//if slits are not included in config file
		if(slit == NULL)
		{
			strcpy(mess, "ERROR: The slit motors do not exist in the config file.");
			return -1;
		}

		// if number of parameters are wrong
		if(args.size()!=2)
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
	else if(args[0] == "centerrel")
	{
		//if slits are not included in config file
		if(slit == NULL)
		{
			strcpy(mess, "ERROR: The slit motors do not exist in the config file.");
			return -1;
		}

		// if number of parameters are wrong
		if(args.size()!=2)
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
	else if(args[0] == "centerabs")
	{
		//if slits are not included in config file
		if(slit == NULL)
		{
			strcpy(mess, "ERROR: The slit motors do not exist in the config file.");
			return -1;
		}

		// if number of parameters are wrong
		if(args.size()!=2)
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
	else if(args[0] == "exactcenter")
	{
		//if slits are not included in config file
		if(slit == NULL)
		{
			strcpy(mess, "ERROR: The slit motors do not exist in the config file.");
			return -1;
		}

		// if number of parameters are wrong
		if(args.size()!=1)
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
	else if(args[0] == "x1zerowidth")
	{
		//if slits are not included in config file
		if(slit == NULL)
		{
			strcpy(mess, "ERROR: The slit motors do not exist in the config file.");
			return -1;
		}

		// if number of parameters are wrong
		if(args.size()!=1)
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
	else if(args[0] == "zerowidth")
	{
		//if slits are not included in config file
		if(slit == NULL)
		{
			strcpy(mess, "ERROR: The slit motors do not exist in the config file.");
			return -1;
		}

		// if number of parameters are wrong
		if(args.size()!=2)
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




	// --- if command is moverel------------------------------------
	else if(args[0] == "moverel")
	{

		// if number of parameters are wrong
		if(args.size()!=3)
		{
			strcpy(mess, "ERROR: Required number of parameters: 3");
			return -1;
		}

		// if position is not a number
		temp.assign(args[2]);
		if(temp.find_first_not_of("0123456789.-")!=string::npos)
		{
			sprintf(mess, "ERROR: %s for position is not a number",args[2]);
			return -1;
		}


		// find motor name  and execute cmd
		for(int i=0;i<Motor::NumMotors;i++)
			if(args[1] == motor[i]->getName())
			{
				newPosition=motor[i]->getPosition()+atof(args[2]);

				// if its a slit motor, checks if the slit motor can move to the new position without crashin into other one
				if(args[1] == "slit_x1")
				{
					slitnum=1;
					int move = slit->canX1Move(newPosition);
					if(move==1)
					{
						sprintf(mess, "ERROR: If slit_x1 moves to that position, it will crash into slit_x2");
						return -1;
					}
					else if(move==-1)
					{
						sprintf(mess, "ERROR: The slit_x1 cannot move to a negative position");
						return -1;
					}
				}
				else if(args[1] == "slit_x2")
				{
					slitnum=2;
					int move = slit->canX2Move(newPosition);
					if(move==1)
					{
						sprintf(mess, "ERROR: If slit_x2 moves to that position, it will crash into slit_x1");
						return -1;
					}
					else if(move==-1)
					{
						sprintf(mess, "ERROR: The slit_x2 cannot move to a negative position");
						return -1;
					}
				}

				if(!motor[i]->canMotorMove(newPosition))
				{
					sprintf(mess, "ERROR: Position given to move motor %s is beyond its limits: %f and %f",args[1],motor[i]->getLowerLimit(),motor[i]->getUpperLimit());
					return -1;
				}

				motor[i]->moveRel(atof(args[2]),0,0);

				//set position member of motor to the updated position
				motor[i]->setPosition(newPosition);
				//set slit positions and limits in slit class
				if(slitnum==1)
					slit->setX1pos(newPosition);
				else if(slitnum==2)
					slit->setX2pos(newPosition);
				sprintf(mess,"Moved %s by %s position",args[1],args[2]);
				return 0;
			}
	}






	// --- if command is simmoverel for simultaneous movt-----------------------------
	else if(args[0] == "simmoverel")
	{
		num=2;
		// if number of parameters are wrong
		if(args.size()!=5)
		{
			strcpy(mess, "ERROR: Required number of parameters: 5");
			return -1;
		}

		// if position is not a number
		temp.assign(args[2]);
		if(temp.find_first_not_of("0123456789.-")!=string::npos)
		{
			sprintf(mess, "ERROR: %s for position is not a number",args[2]);
			return -1;
		}
		temp.assign(args[4]);
		if(temp.find_first_not_of("0123456789.-")!=string::npos)
		{
			sprintf(mess, "ERROR: %s for position is not a number",args[4]);
			return -1;
		}

		// find motor name (case insensitive) and execute cmd
		for(int i=0;i<Motor::NumMotors;i++)
			if(args[1] == motor[i]->getName())
				// find second motor to move simultaneously
				for(int j=0;j<Motor::NumMotors;j++)
					if(args[3]  == motor[j]->getName())
					{
						// if both motors are the same, print error and exit
						if(args[1] == args[3])
						{
							strcpy(mess,"ERROR: Both the motors to be moved simultaneously are the same");
							return -1;
						}

						//if they have the same controller, only then move
						if(motor[i]->getController() == motor[j]->getController())
						{
							newPosition=motor[i]->getPosition()+atof(args[2]);
							newPosition2=motor[j]->getPosition()+atof(args[4]);


							// if motor to be moved is a slit
							if(strcasestr(args[1],"slit")!=NULL)
							{
								//checks if the second motor is not a slit
								if(strcasestr(args[3],"slit")==NULL)
								{
									sprintf(mess, "ERROR: Either both the motors to be moved simulataneously should be slits or neither");
									return -1;
								}
								//checking if both slits can be moved to new position without crashing
								else
								{
									//depending on the first motor being slit_X1/slit_x2, the parameters for canbothslitsmove(x1,x2) change
									int move=0;
									if(args[1] == "slit_x1")
									{
										slitnum=1;
										move=slit->canBothSlitsMove(newPosition,newPosition2);
									}
									else
									{
										slitnum=2;
										move=slit->canBothSlitsMove(newPosition2,newPosition);
									}

									if(move==1)
									{
										sprintf(mess, "ERROR: Either one of the positions given will crash slits into each other");
										return -1;
									}
									else if(move==-1)
									{
										sprintf(mess, "ERROR: Either of the slits cannot be moved to negative positions");
										return -1;
									}

								}
							}

							//checks if the motor is going to move within its limits
							if(!motor[i]->canMotorMove(newPosition))
							{
								sprintf(mess, "ERROR: Position given to move motor %s is beyond its limits: %f and %f",args[1],motor[i]->getLowerLimit(),motor[i]->getUpperLimit());
								return -1;
							}
							if(!motor[j]->canMotorMove(newPosition2))
							{
								sprintf(mess, "ERROR: Position given to move motor %s is beyond its limits: %f and %f",args[3],motor[j]->getLowerLimit(),motor[j]->getUpperLimit());
								return -1;
							}

							if(  (!strcasecmp(args[1],"Huber")) ||(!strcasecmp(args[3],"Huber")) )
							{
								motor[i]->moveRel(atof(args[2]),0,0);
								motor[j]->moveRel(atof(args[4]),0,0);
							}
							else

								motor[i]->moveRel(atof(args[2]),motor[j]->getAxis(),atof(args[4]));
							//set position member of motors to the updated position
							motor[i]->setPosition(newPosition);
							motor[j]->setPosition(newPosition2);

							//setting both the positions and limits of the slit object if its the slit motors which moved
							if (slitnum==1)
								slit->setBothpos(newPosition,newPosition2);
							else if (slitnum==2)
								slit->setBothpos(newPosition2,newPosition);

							sprintf(mess,"Moved simultaneously %s by %s position and %s by %s position",args[1],args[2],args[3],args[4]);
							return 0;
						}
						//if they dont have the same controller, print error
						else
						{
							strcpy(mess,"ERROR: Cannot move motors of different controllers simultaneously");
							return -1;
						}
					}
	}





	//--- if command is moveabs--------------------------------------
	else if(args[0] == "moveabs")
	{
		// if number of parameters are wrong
		if(args.size()!=3)
		{
			strcpy(mess, "ERROR: Required number of parameters: 3");
			return -1;
		}

		// if position is not a number
		temp.assign(args[2]);
		if(temp.find_first_not_of("0123456789.-")!=string::npos)
		{
			sprintf(mess, "ERROR: %s for position should be a number for %s",args[2], args[1]);
			return -1;
		}

		// find motor name  and execute cmd
		for(int i=0;i<Motor::NumMotors;i++)
			if(args[1] == motor[i]->getName())
			{

				// if its a slit motor, checks if the slit motor can move to the new position without crashin into other one
				if((args[1] == "slit_x1")
				{
					slitnum=1;
					if(slit->canX1Move(atof(args[2]))>0)
					{
						sprintf(mess, "ERROR: If slit_x1 moves to that position, it will crash into slit_x2");
						return -1;
					}
					else if(slit->canX1Move(atof(args[2]))<0)
					{
						sprintf(mess, "ERROR: Slit_x1 cannot move to a negative position");
						return -1;
					}
				}
				else if(args[1] == "slit_x2")
				{
					slitnum=2;
					if(slit->canX2Move(atof(args[2]))>0)
					{
						sprintf(mess, "ERROR: If slit_x2 moves to that position, it will crash into slit_x1");
						return -1;
					}
					else if(slit->canX1Move(atof(args[2]))<0)
					{
						sprintf(mess, "ERROR: Slit_x2 cannot move to a negative position");
						return -1;
					}
				}


				if(!motor[i]->canMotorMove(atof(args[2])))
				{
					sprintf(mess, "ERROR: Position given to move motor %s is beyond its limits: %f and %f",args[1],motor[i]->getLowerLimit(),motor[i]->getUpperLimit());
					return -1;
				}

				motor[i]->moveAbs(atof(args[2]),0,0,0);
				//set position member of motor to the updated position
				motor[i]->setPosition(atof(args[2]));

				//set slit positions and limits in slit class
				if(slitnum==1)
					slit->setX1pos(atof(args[2]));
				else if(slitnum==2)
					slit->setX2pos(atof(args[2]));


				sprintf(mess,"Moved %s to %s position",args[1],args[2]);
				return 0;
			}
	}





	// --- if command is sim2moveabs for simultaneous movt of 2 motors-----------------------------
	else if(args[0] == "sim2moveabs")
	{
		num=2;
		// if number of parameters are wrong
		if(args.size()!=5)
		{
			strcpy(mess, "ERROR: Required number of parameters: 5");
			return -1;
		}

		// if position is a number
		temp.assign(args[2]);
		if(temp.find_first_not_of("0123456789.-")!=string::npos)
		{
			sprintf(mess, "ERROR: %s for position should be a number for %s",args[2], args[1]);
			return -1;
		}
		temp.assign(args[4]);
		if(temp.find_first_not_of("0123456789.-")!=string::npos)
		{
			sprintf(mess, "ERROR: %s for position should be a  number for %s",args[4], args[3]);
			return -1;
		}

		// find motor name (case insensitive) and execute cmd
		for(int i=0;i<Motor::NumMotors;i++)
			if(args[1] == motor[i]->getName()
				// find second motor to move simultaneously
				for(int j=0;j<Motor::NumMotors;j++)
					if(args[3] == motor[j]->getName())
					{
						// if both motors are the same, print error and exit
						if((args[1] == args[3])
						{
							strcpy(mess,"ERROR: Both the motors to be moved simultaneously are the same");
							return -1;
						}
						//if they have the same controller, move
						if(motor[i]->getController() == motor[j]->getController())
						{


							// if motors to be moved are slits
							if(strcasestr(args[1],"slit")!=NULL)
							{
								//checks if the second motor is not a slit
								if(strcasestr(args[3],"slit")==NULL)
								{
									sprintf(mess, "ERROR: Either both the motors to be moved simulataneously should be slits or neither");
									return -1;
								}
								//checking if both slits can be moved to new position without crashing
								else
								{
									//depending on the first motor being slit_X1/slit_x2, the parameters for canbothslitsmove(x1,x2) change
									int move=0;
									if(args[1] == "slit_x1")
									{
										slitnum=1;
										move=slit->canBothSlitsMove(atof(args[2]),atof(args[4]));
									}
									else
									{
										slitnum=2;
										move=slit->canBothSlitsMove(atof(args[4]),atof(args[2]));
									}

									if(move>0)
									{
										sprintf(mess, "ERROR: If slits move to these positions, they will crash into each other");
										return -1;
									}
									else if(move<0)
									{
										sprintf(mess, "ERROR: Either of the slits cannot be moved to a negative position");
										return -1;
									}
								}
							}


							if(!motor[i]->canMotorMove(atof(args[2])))
							{
								sprintf(mess, "ERROR: Position given to move motor %s is beyond its limits: %f and %f",args[1],motor[i]->getLowerLimit(),motor[i]->getUpperLimit());
								return -1;
							}
							if(!motor[j]->canMotorMove(atof(args[4])))
							{
								sprintf(mess, "ERROR: Position given to move motor %s is beyond its limits: %f and %f",args[3],motor[j]->getLowerLimit(),motor[j]->getUpperLimit());
								return -1;
							}

							if(  (!strcasecmp(args[1],"Huber")) ||(!strcasecmp(args[3],"Huber")) )
							{
								motor[i]->moveAbs(atof(args[2]),0,0,0);
								motor[j]->moveAbs(atof(args[4]),0,0,0);
							}
							else

								motor[i]->moveAbs(atof(args[2]),motor[j]->getAxis(),atof(args[4]), motor[j]->getPosition());

							//set position member of motors to the updated position
							motor[i]->setPosition(atof(args[2]));
							motor[j]->setPosition(atof(args[4]));


							//setting both the positions and limits of the slit object if the motors to be moved were slits
							if(slitnum==1)
								slit->setBothpos(atof(args[2]),atof(args[4]));
							else if(slitnum==2)
								slit->setBothpos(atof(args[4]),atof(args[2]));


							sprintf(mess,"Moved simultaneously %s to %s position and %s to %s position",args[1],args[2],args[3],args[4]);
							return 0;
						}
						//if they dont have the same controller, print error
						else
						{
							strcpy(mess,"ERROR: Cannot move motors of different controllers simultaneously");
							return -1;
						}
					}


	}
	// --- if command is sim3moveabs for simultaneous movt of all 3  motors-----------------------------
	else if(args[0] == "sim3moveabs")
	{
		num=3;
		int x1=0,x2=0;
		double pos1,pos2,pos3;
		// if number of parameters are wrong
		if(args.size()!=7)
		{
			strcpy(mess, "ERROR: Required number of parameters: 7");
			return -1;
		}

		// if position is a positive number
		temp.assign(args[2]);
		if(temp.find_first_not_of("0123456789.-")!=string::npos)
		{
			sprintf(mess, "ERROR: %s for position should be a number for %s",args[2], args[1]);
			return -1;
		}
		temp.assign(args[4]);
		if(temp.find_first_not_of("0123456789.-")!=string::npos)
		{
			sprintf(mess, "ERROR: %s for position should be a number for %s",args[4], args[3]);
			return -1;
		}
		temp.assign(args[6]);
		if(temp.find_first_not_of("0123456789.-")!=string::npos)
		{
			sprintf(mess, "ERROR: %s for position should be a number for %s",args[6], args[5]);
			return -1;
		}

		// find motor name (case insensitive)
		for(int i=0;i<Motor::NumMotors;i++)
			if((args[1] == motor[i]->getName())
				// find second motor
				for(int j=0;j<Motor::NumMotors;j++)
					if((args[3] == motor[j]->getName())
						//find third motor
						for(int k=0;k<Motor::NumMotors;k++)
							if((args[5] == motor[k]->getName())
							{
								// if two motors are the same, print error and exit
								if( (!strcasecmp(args[1],args[3])) ||(!strcasecmp(args[2],args[3])) || (!strcasecmp(args[1],args[2])) )
								{
									strcpy(mess,"ERROR: Two of the motors to be moved simultaneously are the same");
									return -1;
								}
								//if they all have the same controller, move
								if(  (!strcmp(motor[i]->getController(),motor[j]->getController())) &&
										(!strcmp(motor[j]->getController(),motor[k]->getController())) )
								{


									// if any of the motors to be moved are slits
									if(  (strcasestr(args[1],"slit")!=NULL) ||(strcasestr(args[3],"slit")!=NULL) || (strcasestr(args[5],"slit")!=NULL) )
									{
										slitnum=1;
										// finding which argument is slitx1 and which one is slitx2
										for(int loop=1;loop<=5;loop=loop+2)
										{
											if(!strcasecmp(args[loop],"slit_x1"))
												x1=loop;
											else if(!strcasecmp(args[loop],"slit_x2"))
												x2=loop;
										}

										//depending on the first motor being slit_X1/slit_x2, the parameters for canbothslitsmove(x1,x2) change
										if(slit->canBothSlitsMove(atof(args[x1+1]),atof(args[x2+1]))>0)
										{
											sprintf(mess, "ERROR: If slits move to these positions, they will crash into each other");
											return -1;
										}
										else if(slit->canBothSlitsMove(atof(args[x1+1]),atof(args[x2+1]))<0)
										{
											sprintf(mess, "ERROR: Either of the slits cannot be moved to a negative position");
											return -1;
										}

									}


									if(!motor[i]->canMotorMove(atof(args[2])))
									{
										sprintf(mess, "ERROR: Position given to move motor %s is beyond its limits: %f and %f",args[1],motor[i]->getLowerLimit(),motor[i]->getUpperLimit());
										return -1;
									}
									if(!motor[j]->canMotorMove(atof(args[4])))
									{
										sprintf(mess, "ERROR: Position given to move motor %s is beyond its limits: %f and %f",args[3],motor[j]->getLowerLimit(),motor[j]->getUpperLimit());
										return -1;
									}
									if(!motor[k]->canMotorMove(atof(args[6])))
									{
										sprintf(mess, "ERROR: Position given to move motor %s is beyond its limits: %f and %f",args[5],motor[k]->getLowerLimit(),motor[k]->getUpperLimit());
										return -1;
									}


									if(  (!strcasecmp(args[1],"Huber")) ||(!strcasecmp(args[3],"Huber")) || (!strcasecmp(args[5],"Huber")) )
									{
										motor[i]->moveAbs(atof(args[2]),0,0,0);
										motor[j]->moveAbs(atof(args[4]),0,0,0);
										motor[k]->moveAbs(atof(args[6]),0,0,0);
									}
									else
									{

										int axis = motor[i]->getAxis();
										int p =2;
										for(int repeat=0;repeat<3;repeat++)
										{

											if(axis==1)
												pos1 = atof(args[p]);
											else if(axis==2)
												pos2 = atof(args[p]);
											else
												pos3 = atof(args[p]);

											if(repeat==2) break;
											else if(!repeat)
											{
												axis = motor[j]->getAxis();
												p=4;
											}
											else
											{
												axis = motor[k]->getAxis();
												p=6;
											}
										}


										motor[i]->moveAllAbs(pos1,pos2,pos3);

									}

									//set position member of motors to the updated position
									motor[i]->setPosition(atof(args[2]));
									motor[j]->setPosition(atof(args[4]));
									motor[k]->setPosition(atof(args[6]));


									//setting both the positions and limits of the slit object if the motors to be moved were slits
									if(slitnum)
										slit->setBothpos(atof(args[x1+1]),atof(args[x2+1]));

									sprintf(mess,"Moved simultaneously %s to %s position,%s to %s and %s to %s position",
											args[1],args[2],args[3],args[4],args[5],args[6]);
									return 0;
								}
								//if they dont have the same controller, print error
								else
								{
									strcpy(mess,"ERROR: Cannot move motors of different controllers simultaneously");
									return -1;
								}
							}
	}*/
	if(args[0] == "blla") {
		
	}
	// for wrong commands-----------------------------------------
	else
	{
		sprintf(mess,"ERROR: %s is not a Command",args[0].c_str());
		return -1;
	}

	switch(num) {
	case 1:
		sprintf(mess,"ERROR:%s is not listed in the config file.",args[1].c_str());
		break;
	case 2:
		sprintf(mess,"ERROR: Either this motor %s or this motor %s is not listed in the config file.",args[1].c_str(),args[3].c_str());
		break;
	default:
		sprintf(mess,"ERROR: Either this motor %s, this motor %s or this motor %s is not listed in the config file.",args[1].c_str(),args[3].c_str(),args[5].c_str());
		break;
	}
	return -1;
}

Initialize::~Initialize() {
	if (xrayTube != NULL)
		delete xrayTube;
	if (pgauge != NULL) {
		delete pgauge;
	}
	if (slit != NULL) {
		delete slit;
	}

	/* for(int i=0;i<controller.size();i++)
		interface[i]->close_serialfd();
	if(xrayTube!=NULL)
		xrayTube->getInterface()->close_serialfd();
	if(pgauge!=NULL)
		pgauge->getInterface()->close_serialfd();
		*/
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
		if (fluorescence[i]->getNumTargetLists() == 0) {
			throw RuntimeError("No targets added to fluorescence" + fluorescence[i]->isCircular() ? "_wheel" : "");
		}
	}

	// update motor axis for each controller and positions (debug)
	for (unsigned int i = 0; i < controller.size(); ++i) {
		controller[i]->updateAxisEnabled();
		controller[i]->debugPositions();
	}

	//setOrigPositions(POSITIONS_FILE); TODO

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

	// find index in fluorescence list (using circular or linear)
	bool circular = false;
	if (name == "fluorescence_wheel") {
		circular = true;
	}
	int index = -1;
	for (unsigned int i = 0; i < fluorescence.size(); ++i) {
		if (fluorescence[i]->isCircular() == circular) {
			index = i;
			break;
		}
	}
	if (index == -1) {
		throw RuntimeError("Did not find " + name  + " in the motor list.");
	}

	// add targets
	fluorescence[index]->addTarget(targetName, targetEnergy);
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
		std::istringstream iss (args[4].c_str());
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
	int icontroller = -1;
	for (unsigned int i = 0; i < controller.size(); ++i) {
		if (controller[i]->getName() == controllerName) {
			icontroller = (int)i;
			break;
		}
	}
	if (icontroller == -1) {
		throw RuntimeError("Motor" + name + " has unknown controller " + controllerName);
	}

	// create motor object
	motor.push_back(new Motor(motor.size(), name, axis, icontroller, lowerLimit, upperLimit));
	controller[icontroller]->setMotor(axis, motor[motor.size() - 1]);


	// if slit_x2, create slit object

	// if fluorescence, create fluorescence object
	if (name == "Fluorescence") {
		fluorescence.push_back(new Fluorescence(fluorescence.size(), false, controller[icontroller]));
	}
	else if (name == "Fluorescence_wheel") {
		fluorescence.push_back(new Fluorescence(fluorescence.size(), true, controller[icontroller]));
	} else if (name == "Slit_x2") {
		Controller* controller_slitx1 = NULL;
		int iaxisX1 = -1;
		for (unsigned int i = 0; i < motor.size(); ++i) {
			if (motor[i]->getName() == "Slit_x1") {
				controller_slitx1 = controller[motor[i]->getController()];
				iaxisX1 = motor[i]->getAxis();
				break;
			}
		}
		if (controller_slitx1 == NULL) {
			throw RuntimeError("Could not find slit_x1 before slit_x2 in config file.");
		}
		slit = new Slit(controller_slitx1, controller[icontroller], iaxisX1, axis);
	}
	std::cout << std::endl;
 }