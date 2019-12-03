/********************************************//** 
 * @file Initialize.cpp
 * @short reads config files, moves the motors, and saves positions
 * @author Dhanya
 ***********************************************/


#include "Initialize.h"


#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <math.h>
#include <time.h>
#include <cstdlib> //exit
#include <cstring> //strncpy
#include "commonDefs.h"
#include <unistd.h> // (usleep,etc.) in raspberrypi
using namespace std;


//-------------------------------------------------------------------------------------------------------------------------------------------------

int Initialize::executeCommand(int argc, char* args[], char mess[])
{
	string temp;
	int num=1;
	double newPosition;
	double newPosition2;

#ifdef LASERBOX
	//------------------------------------------------- laser box specific list  ---------------------------------------------------------------------
	// --- if command is checkref---------------------------------------
	if(strcasecmp(args[0],"checkref")==0)
	{
		// if number of parameters are wrong
		if(argc!=1)
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


		for(int i=0;i<NUMBER_OF_REFPOINTS;i++)
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
	else if(strcasecmp(args[0],"fvals")==0)
	{
		//if it doesnt exist, the error message in the end
		num=0;
		// if number of parameters are wrong
		if(argc!=2)
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
	else if(strcasecmp(args[0],"refvals")==0)
	{
		//if it doesnt exist, the error message in the end
		num=4;
		// if number of parameters are wrong
		if(argc!=2)
		{
			strcpy(mess, "ERROR: Required number of parameters: 2\nHelp: refvals [reference_name]");
			return -1;
		}
		strcpy(mess,"");
		//find the refpoint
		for(int i=0;i<NUMBER_OF_REFPOINTS;i++)
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
	else if(strcasecmp(args[0],"fwlist")==0)
	{
		// if number of parameters are wrong
		if(argc!=1)
		{
			strcpy(mess, "ERROR: Required number of parameters: 1\nHelp: fwlist");
			return -1;
		}
		//all the filter wheels
		for(int i=0;i<Fwheel::NumFwheels;i++)
		{
			strcat(mess," ");
			strcat(mess,fwheel[i]->getName());
		}
		return 0;

	}


	// --- if command is reflist---------------------------------------
	else if(strcasecmp(args[0],"reflist")==0)
	{
		// if number of parameters are wrong
		if(argc!=1)
		{
			strcpy(mess, "ERROR: Required number of parameters: 1\nHelp: reflist");
			return -1;
		}
		//all the filter wheels
		for(int i=0;i<NUMBER_OF_REFPOINTS;i++)
		{
			strcat(mess," ");
			strcat(mess,referencePoints[i][0].c_str());
		}
		return 0;

	}


	// --- if command is ref---------------------------------------
	else if(strcasecmp(args[0],"ref")==0)
	{
		num=4;
		// if number of parameters are wrong
		if(argc!=2)
		{
			strcpy(mess, "ERROR: Required number of parameters: 2\nHelp: ref [reference_point_name]\n");
			return -1;
		}
		//find the ref point
		for(int i=0;i<NUMBER_OF_REFPOINTS;i++)
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
	else if(strcasecmp(args[0],"fsetval")==0)
	{
		num=0;
		// if number of parameters are wrong
		if(argc!=3)
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
		for(int i=0;i<Fwheel::NumFwheels;i++)
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
/*					sprintf(mess,"ERROR: %s absorption value for %s is not defined. \nOptions(%f,%f,%f,%f,%f,%f)",
							args[2],args[1],
							fwheel[i]->ValueList[0],fwheel[i]->ValueList[1],fwheel[i]->ValueList[2],
							fwheel[i]->ValueList[3],fwheel[i]->ValueList[4],fwheel[i]->ValueList[5]);*/
					return -1;
				}
				sprintf(mess,"%s set to %s value",args[1],args[2]);
				return 0;
			}
	}



	// --- if command is fgetval---------------------------------------
	else if(strcasecmp(args[0],"fgetval")==0)
	{
		num=0;
		// if number of parameters are wrong
		if(argc!=2)
		{
			strcpy(mess, "ERROR: Required number of parameters: 2\nHelp: fgetval [filter_wheel_name]\n");
			return -1;
		}

		//find fwheel and value
		for(int i=0;i<Fwheel::NumFwheels;i++)
			if(!strcasecmp(args[1],fwheel[i]->getName()))
			{
				sprintf(mess,"%f",fwheel[i]->getValue());
				return 0;
			}
	}


	//------------------------------------------------- end of laser box specific list ---------------------------------------------------------------
#else
	//------------------------------------------------- xray tube specific list (not laser)--------------------------------------------------------------
	int slitnum=0;
	double midpos;


	// --- if command is getpower----------------------------------------
	if(strcasecmp(args[0],"getpower")==0)
	{
		// if number of parameters are wrong
		if(argc!=1)
		{
			strcpy(mess, "ERROR: Required number of parameters: 1");
			return -1;
		}

		sprintf(mess,"%d",maxTubePower);
		return 0;
	}

	// --- if command is readwarmuptiming----------------only for gui----
	else if(strcasecmp(args[0],"readwarmuptiming")==0)
	{
		// if number of parameters are wrong
		if(argc!=2)
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
	else if(strcasecmp(args[0],"writewarmuptiming")==0)
	{
		string s_time;
		time_t tdate;

		// if number of parameters are wrong
		if(argc!=2)
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
	else if(strcasecmp(args[0],"numflist")==0)
	{
		// if number of parameters are wrong
		if(argc!=1)
		{
			strcpy(mess, "ERROR: Required number of parameters: 1");
			return -1;
		}

		sprintf(mess, "%d", (int)fluorListArray.size());
		return 0;
	}

	// --- if command is whichflist----------------
	else if(strcasecmp(args[0],"whichflist")==0)
	{
		// if number of parameters are wrong
		if(argc!=1)
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
	else if(strcasecmp(args[0],"loadflist")==0)
	{
		// if number of parameters are wrong
		if(argc!=2)
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
	else if(strcasecmp(args[0],"getfl")==0)
	{
		// if number of parameters are wrong
		if(argc!=1)
		{
			strcpy(mess, "ERROR: Required number of parameters: 1");
			return -1;
		}

		for(int i=0;i<Motor::NumMotors;i++)
		{
			if(strcasecmp("Fluorescence",motor[i]->getName())==0)
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
	else if(strcasecmp(args[0],"movefl")==0)
	{
		int found=0;
		bool fluorNameFound = false;
		// if number of parameters are wrong
		if(argc!=2)
		{
			strcpy(mess, "ERROR: Required number of parameters: 2");
			return -1;
		}

		for(int i=0;i<Motor::NumMotors;i++)
		{
			if(strcasecmp("Fluorescence",motor[i]->getName())==0)
			{
				found=1;
				// loops through to find fluor name
				for(int j=0;j<maxfluorvalues;j++)
				{
					if(strcasecmp(args[1],fluorList[j][0].c_str())==0)
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
	else if(strcasecmp(args[0],"createxrayport")==0)
	{

		// if number of parameters are wrong
		if(argc!=1)
		{
			strcpy(mess, "ERROR: Required number of parameters: 1");
			return -1;
		}
		if(xrayStatus==0)
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
			for(int i=0;i<NUMBER_OF_CONTROLLERS;i++)
				if(!(strcmp(interface[i]->getSerial(),serial))){
					used=true;
					break;
				}
			// no need to check filter wheels as this command is not for laser box
#ifdef VACUUMBOX
			if(pressureInterface != NULL && (!(strcmp(pressureInterface->getSerial(),serial))))
			    used=true;
#endif
			if(!used) {
				if (tubeInterface==NULL){
					tubeInterface = new Interface(serial , &success, true);
					if(success){
						xrayStatus=0;
						XrayTube = new Xray(tubeInterface);
#ifdef XRAYBOX
						strcpy(mess, "Serial Port to the xray tube has been created successfully");
						return 0;
#endif
					}else {
                        xrayStatus=-1;
                        tubeInterface = NULL;
					}
				}
				else if(!(strcmp(tubeInterface->getSerial(),serial))){
					used=true;
				}
			}
		}

		if (xrayStatus) {
			strcpy(mess, "ERROR: Unable to create serial port for X-Ray Tube or the tube is switched off.");
			xrayStatus=-1;
			tubeInterface = NULL;
			return -1;
		}

		//should not reach here
		return -1;
	}


#ifdef VACUUMBOX
    // --- if command is pressure------------------creates pgauge if doesnt exist, gets pressure-------------------------
    else if(strcasecmp(args[0],"pressure")==0)
    {

        // if number of parameters are wrong
        if(argc!=1)
        {
            strcpy(mess, "ERROR: Required number of parameters: 1");
            return -1;
        }
        if(pressureInterface == NULL) {

            bool success = false;
            //check through all the usb devices connected
            char serial[200]="/dev/ttyUSBX";
            bool used;
            for(int usbNum=0;usbNum<10;usbNum++) {
                serial[11]=usbNum+48;
                used=false;
                for(int i=0;i<NUMBER_OF_CONTROLLERS;i++) {
                    if(!(strcmp(interface[i]->getSerial(),serial))){
                        used=true;
                        break;
                    }
                }
                // no need to check filter wheels as this command is not for laser box
                if((tubeInterface != NULL) && (!(strcmp(tubeInterface->getSerial(),serial))))
                    used=true;
                if(!used) {
                    pressureInterface = new Interface(serial, true, &success);
                    if(success){
                        pgauge = new Pgauge(pressureInterface);
                        printf("Serial Port to the Pressure Gauge Controller has been created successfully\n");
                        break;
                    }else {
                        pgauge = NULL;
                        pressureInterface = NULL;
                    }
                }
            }
        }

        if (pressureInterface == NULL) {
            strcpy(mess, "ERROR: Unable to create Pressure Gauge Controller or the pump is switched off.");
            pgauge = NULL;
            pressureInterface = NULL;
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
#endif

	// --- if command is geterr-----------------------------------------------
	else if(strcasecmp(args[0],"geterr")==0)
	{

		// if number of parameters are wrong
		if(argc!=1)
		{
			strcpy(mess, "ERROR: Required number of parameters: 1");
			return -1;
		}

		if(XrayTube==NULL)
		{
			strcpy(mess, "ERROR: The Xray Tube class has not been instantiated before");
			return -1;
		}

		sprintf(mess, "%d",XrayTube->getErrorCode());
		return 0;
	}

	// --- if command is getemess-----------------------------------------------
	else if(strcasecmp(args[0],"getemess")==0)
	{

		// if number of parameters are wrong
		if(argc!=1)
		{
			strcpy(mess, "ERROR: Required number of parameters: 1");
			return -1;
		}

		if(XrayTube==NULL)
		{
			strcpy(mess, "ERROR: The Xray Tube class has not been instantiated before");
			return -1;
		}

		strcpy(mess,XrayTube->getErrorMessage());
		return 0;
	}


	// --- if command is clear----------------------------------------------
	else if(strcasecmp(args[0],"clear")==0)
	{

		// if number of parameters are wrong
		if(argc!=1)
		{
			strcpy(mess, "ERROR: Required number of parameters: 1");
			return -1;
		}

		if(XrayTube==NULL)
		{
			strcpy(mess, "ERROR: The Xray Tube class has not been instantiated before");
			return -1;
		}

		XrayTube->clearErrorCode();
		sprintf(mess, "The Error code has been cleared");
		return 0;
	}



	// --- if command is xrayStatus-------this si to check if xray class has been created or not------------
	else if(strcasecmp(args[0],"xrayStatus")==0)
	{

		// if number of parameters are wrong
		if(argc!=1)
		{
			strcpy(mess, "ERROR: Required number of parameters: 1");
			return -1;
		}

		sprintf(mess, "%d",xrayStatus);
		return 0;
	}




	// --- if command is isStandby--------------------------------------------------------------------------------
	else if(strcasecmp(args[0],"isStandby")==0)
	{
		// if number of parameters are wrong
		if(argc!=1)
		{
			strcpy(mess, "ERROR: Required number of parameters: 1");
			return -1;
		}

		if(XrayTube==NULL)
		{
			strcpy(mess, "ERROR: The Xray Tube class has not been instantiated before");
			return -1;
		}

		if(XrayTube->isOnStandby()==1)
			strcpy(mess,"Yes, the X-Ray tube is on standby");
		else if(XrayTube->isOnStandby()==-9999)
			strcpy(mess,"The X-Ray tube is switched off");
		else
			strcpy(mess,"No, the X-Ray tube is NOT on standby");
		return 0;
	}




	// --- if command is getHV-------------------------------------------------------------------------------
	else if(strcasecmp(args[0],"getHV")==0)
	{
		// if number of parameters are wrong
		if(argc!=1)
		{
			strcpy(mess, "ERROR: Required number of parameters: 1");
			return -1;
		}

		if(XrayTube==NULL)
		{
			strcpy(mess, "ERROR: The Xray Tube class has not been instantiated before");
			return -1;
		}

		if(XrayTube->getHVSwitch())
			strcpy(mess,"Yes, the HV is switched ON");
		else
			strcpy(mess,"No, the HV is NOT switched ON");
		return 0;
	}



	// --- if command is getV---------------------------------------------------------------------------------
	else if(strcasecmp(args[0],"getV")==0)
	{
		// if number of parameters are wrong
		if(argc!=1)
		{
			strcpy(mess, "ERROR: Required number of parameters: 1");
			return -1;
		}

		if(XrayTube==NULL)
		{
			strcpy(mess, "ERROR: The Xray Tube class has not been instantiated before");
			return -1;
		}

		sprintf(mess,"%d",XrayTube->getVoltage());
		return 0;
	}



	// --- if command is getActualV---------------------------------------------------------------------------------
	else if(strcasecmp(args[0],"getActualV")==0)
	{
		// if number of parameters are wrong
		if(argc!=1)
		{
			strcpy(mess, "ERROR: Required number of parameters: 1");
			return -1;
		}

		if(XrayTube==NULL)
		{
			strcpy(mess, "ERROR: The Xray Tube class has not been instantiated before");
			return -1;
		}



		sprintf(mess,"%d",XrayTube->getVoltageActual());
		return 0;
	}


	// --- if command is getC---------------------------------------------------------------------------------
	else if(strcasecmp(args[0],"getC")==0)
	{
		// if number of parameters are wrong
		if(argc!=1)
		{
			strcpy(mess, "ERROR: Required number of parameters: 1");
			return -1;
		}

		if(XrayTube==NULL)
		{
			strcpy(mess, "ERROR: The Xray Tube class has not been instantiated before");
			return -1;
		}

		sprintf(mess,"%d",XrayTube->getCurrent());
		return 0;
	}



	// --- if command is getActualC---------------------------------------------------------------------------------
	else if(strcasecmp(args[0],"getActualC")==0)
	{
		// if number of parameters are wrong
		if(argc!=1)
		{
			strcpy(mess, "ERROR: Required number of parameters: 1");
			return -1;
		}

		if(XrayTube==NULL)
		{
			strcpy(mess, "ERROR: The Xray Tube class has not been instantiated before");
			return -1;
		}

		/*
      time(&currTime);
      cout<<"\nThe current local time is :"<<ctime(&currTime)<<endl;

      timeinfo = localtime( &currTime);
      cout<<"The current date/time is:"<<asctime(timeinfo)<<endl;
		 */


		sprintf(mess,"%d",XrayTube->getCurrentActual());
		return 0;
	}



	// --- if command is getVandC---------------------------------------------------------------------------------
	else if(strcasecmp(args[0],"getVandC")==0)
	{
		// if number of parameters are wrong
		if(argc!=1)
		{
			strcpy(mess, "ERROR: Required number of parameters: 1");
			return -1;
		}

		if(XrayTube==NULL)
		{
			strcpy(mess, "ERROR: The Xray Tube class has not been instantiated before");
			return -1;
		}

		int voltage,current;
		XrayTube->getVoltageAndCurrent(voltage,current);
		sprintf(mess,"Voltage:%d, Current:%d.",voltage,current);
		return 0;
	}



	// --- if command is getActualVandC---------------------------------------------------------------------------------
	else if(strcasecmp(args[0],"getActualVandC")==0)
	{
		// if number of parameters are wrong
		if(argc!=1)
		{
			strcpy(mess, "ERROR: Required number of parameters: 1");
			return -1;
		}

		if(XrayTube==NULL)
		{
			strcpy(mess, "ERROR: The Xray Tube class has not been instantiated before");
			return -1;
		}

		int voltage,current;
		XrayTube->getVoltageAndCurrentActual(voltage,current);
		sprintf(mess,"Actual Voltage:%d, Actual Current:%d.",voltage,current);
		return 0;
	}



	// --- if command is getshutter1---------------------------------------------------------------------------------
	else if(strcasecmp(args[0],"getshutter1")==0)
	{
		// if number of parameters are wrong
		if(argc!=1)
		{
			strcpy(mess, "ERROR: Required number of parameters: 1");
			return -1;
		}

		if(XrayTube==NULL)
		{
			strcpy(mess, "ERROR: The Xray Tube class has not been instantiated before");
			return -1;
		}

		sprintf(mess,"Shutter 1:"); if(XrayTube->getShutter1()) strcat(mess,"ON  "); else strcat(mess,"OFF  ");
		return 0;
	}


	// --- if command is getshutter2---------------------------------------------------------------------------------
	else if(strcasecmp(args[0],"getshutter2")==0)
	{
		// if number of parameters are wrong
		if(argc!=1)
		{
			strcpy(mess, "ERROR: Required number of parameters: 1");
			return -1;
		}

		if(XrayTube==NULL)
		{
			strcpy(mess, "ERROR: The Xray Tube class has not been instantiated before");
			return -1;
		}

		sprintf(mess,"Shutter 2:"); if(XrayTube->getShutter2()) strcat(mess,"ON  "); else strcat(mess,"OFF  ");
		return 0;
	}



	// --- if command is getshutter3---------------------------------------------------------------------------------
	else if(strcasecmp(args[0],"getshutter3")==0)
	{
		// if number of parameters are wrong
		if(argc!=1)
		{
			strcpy(mess, "ERROR: Required number of parameters: 1");
			return -1;
		}

		if(XrayTube==NULL)
		{
			strcpy(mess, "ERROR: The Xray Tube class has not been instantiated before");
			return -1;
		}

		sprintf(mess,"Shutter 3:"); if(XrayTube->getShutter3()) strcat(mess,"ON  "); else strcat(mess,"OFF  ");
		return 0;
	}


	// --- if command is getshutter4---------------------------------------------------------------------------------
	else if(strcasecmp(args[0],"getshutter4")==0)
	{
		// if number of parameters are wrong
		if(argc!=1)
		{
			strcpy(mess, "ERROR: Required number of parameters: 1");
			return -1;
		}

		if(XrayTube==NULL)
		{
			strcpy(mess, "ERROR: The Xray Tube class has not been instantiated before");
			return -1;
		}

		sprintf(mess,"Shutter 4:"); if(XrayTube->getShutter4()) strcat(mess,"ON  "); else strcat(mess,"OFF  ");
		return 0;
	}



	// --- if command is getshutters---------------------------------------------------------------------------------
	else if(strcasecmp(args[0],"getshutters")==0)
	{
		// if number of parameters are wrong
		if(argc!=1)
		{
			strcpy(mess, "ERROR: Required number of parameters: 1");
			return -1;
		}

		if(XrayTube==NULL)
		{
			strcpy(mess, "ERROR: The Xray Tube class has not been instantiated before");
			return -1;
		}
		int s1,s2,s3,s4;
		XrayTube->getShutters(s1,s2,s3,s4);
		sprintf(mess,"Shutter 1:"); if(s1) strcat(mess,"ON  "); else strcat(mess,"OFF  ");
		strcat(mess,"Shutter 2:"); if(s2) strcat(mess,"ON  "); else strcat(mess,"OFF  ");
		strcat(mess,"Shutter 3:"); if(s3) strcat(mess,"ON  "); else strcat(mess,"OFF  ");
		strcat(mess,"Shutter 4:"); if(s4) strcat(mess,"ON  "); else strcat(mess,"OFF  ");
		return 0;
	}




	// --- if command is HV---------------------------------------------------------------------------------
	else if(strcasecmp(args[0],"HV")==0)
	{
		// if number of parameters are wrong
		if(argc!=2)
		{
			strcpy(mess, "ERROR: Required number of parameters: 2");
			return -1;
		}

		if(!( (!strcasecmp(args[1],"on"))||(!strcasecmp(args[1],"off")) ))
		{
			strcpy(mess, "ERROR: The values for HV should be either 'on' or 'off'");
			return -1;
		}
		if(XrayTube==NULL)
		{
			strcpy(mess, "ERROR: The Xray Tube class has not been instantiated before");
			return -1;
		}

		int value;
		if(!strcasecmp(args[1],"on"))
			value=1;
		else if(!strcasecmp(args[1],"off"))
			value=0;

		XrayTube->setHVSwitch(value);
		sprintf(mess,"The High Voltage has been switched %s",args[1]);
		return 0;
	}



	// --- if command is shutter---------------------------------------------------------------------------------
	else if(strcasecmp(args[0],"shutter")==0)
	{
		// if number of parameters are wrong
		if(argc!=3)
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
		if(XrayTube==NULL)
		{
			strcpy(mess, "ERROR: The Xray Tube class has not been instantiated before");
			return -1;
		}

		bool value;
		if(!strcasecmp(args[2],"on"))
			value=true;
		else if(!strcasecmp(args[2],"off"))
			value=false;


		XrayTube->setShutter(atoi(args[1]),value);
		sprintf(mess,"Shutter %s has been switched %s",args[1],args[2]);
		return 0;
	}



	// --- if command is setv---------------------------------------------------------------------------------
	else if(strcasecmp(args[0],"setv")==0)
	{
		// if number of parameters are wrong
		if(argc!=2)
		{
			strcpy(mess, "ERROR: Required number of parameters: 2");
			return -1;
		}

		if(XrayTube==NULL)
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

		if((voltage*0.001*XrayTube->getCurrent()) > maxTubePower)
		{
			sprintf(mess, "ERROR: This voltage %skV exceeds the max power of %dW since the current is set to %dmA",args[1],maxTubePower,(XrayTube->getCurrent()/1000));
			return -1;
		}
		XrayTube->setVoltage(voltage);
		sprintf(mess,"The voltage %skV has been set for the X-Ray Tube",args[1]);
		return 0;
	}


	// --- if command is setc---------------------------------------------------------------------------------
	else if(strcasecmp(args[0],"setc")==0)
	{
		// if number of parameters are wrong
		if(argc!=2)
		{
			strcpy(mess, "ERROR: Required number of parameters: 2");
			return -1;
		}

		if(XrayTube==NULL)
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

		if((current*0.001*XrayTube->getVoltage())>maxTubePower)
		{
			sprintf(mess, "ERROR: This current %smA exceeds the max power of %dW since the voltage is set to %dkV",args[1],maxTubePower,(XrayTube->getVoltage()/1000));

			return -1;
		}
		XrayTube->setCurrent(current);
		sprintf(mess,"The current %smA has been set for the X-Ray Tube",args[1]);
		return 0;
	}



	// --- if command is setvc---------------------------------------------------------------------------------
	else if(strcasecmp(args[0],"setvc")==0)
	{
		// if number of parameters are wrong
		if(argc!=3)
		{
			strcpy(mess, "ERROR: Required number of parameters: 3");
			return -1;
		}

		if(XrayTube==NULL)
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
		XrayTube->setVoltageAndCurrent(voltage,current);
		sprintf(mess,"The voltage %skV and current %smA has been set for the X-Ray Tube",args[1],args[2]);
		return 0;
	}




	// --- if command is warmup---------------------------------------------------------------------------------
	else if(strcasecmp(args[0],"warmup")==0)
	{
		// if number of parameters are wrong
		if(argc!=2)
		{
			strcpy(mess, "ERROR: Required number of parameters: 2");
			return -1;
		}

		if(XrayTube==NULL)
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
		if(XrayTube->isOnStandby()==1)
		{
			sprintf(mess, "ERROR: The X-Ray tube is on standby!");
			return -1;
		}
		if(XrayTube->isOnStandby()==-9999)
		{
			sprintf(mess, "ERROR: The X-Ray tube is switched off!");
			return -1;
		}

		XrayTube->startWarmup(voltage);
		XrayTube->getErrorCode();
		XrayTube->setHVSwitch(1);
		sprintf(mess,"The X-Ray tube has started warming up");
		return 0;
	}



	// --- if command is getwtime---------------------------------------------------------------------------------
	else if(strcasecmp(args[0],"getwtime")==0)
	{
		// if number of parameters are wrong
		if(argc!=1)
		{
			strcpy(mess, "ERROR: Required number of parameters: 1");
			return -1;
		}

		if(XrayTube==NULL)
		{
			strcpy(mess, "ERROR: The Xray Tube class has not been instantiated before");
			return -1;
		}

		sprintf(mess,"%d",XrayTube->getWarmupTimeRemaining());
		return 0;
	}



	// --- if command is issafe---------------------------------------------------------------------------------
	else if(strcasecmp(args[0],"issafe")==0)
	{
		// if number of parameters are wrong
		if(argc!=1)
		{
			strcpy(mess, "ERROR: Required number of parameters: 1");
			return -1;
		}

		if(XrayTube==NULL)
		{
			strcpy(mess, "ERROR: The Xray Tube class has not been instantiated before. Do that first");
			return -1;
		}
		if(XrayTube->isAccessPossible())
			sprintf(mess,"Yes, You may open the door now!");
		else
			sprintf(mess,"No, You may NOT open the door now! What were you thinking???");
		return 0;
	}
#endif
	//------------------------------------------ end of xray tube specific list ------------------------------------------------------------------





	// --- if command is setPos---------------------------------------
	else if(strcasecmp(args[0],"setPos")==0)
	{
		char positions[3][200],command[200];
		for(int i=0;i<3;i++) strcpy(positions[i],"0");

		// if number of parameters are wrong
		if(argc!=3)
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
	else if(strcasecmp(args[0],"setLower")==0)
	{
		// if number of parameters are wrong
		if(argc!=3)
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
	else if(strcasecmp(args[0],"setUpper")==0)
	{
		// if number of parameters are wrong
		if(argc!=3)
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
	else if(strcasecmp(args[0],"getSpeed")==0)
	{
		// if number of parameters are wrong
		if(argc!=2)
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
	else if(strcasecmp(args[0],"setSpeed")==0)
	{
		// if number of parameters are wrong
		if(argc!=3)
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

#ifndef LASERBOX

	// --- if command is getX1Limit---------------------------------------
	else if(strcasecmp(args[0],"getX1Limit")==0)
	{

		// if number of parameters are wrong
		if(argc!=1)
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
	else if(strcasecmp(args[0],"getX2Limit")==0)
	{

		// if number of parameters are wrong
		if(argc!=1)
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
#endif



	// --- if command is getUpperLimit---------------------------------------
	else if(strcasecmp(args[0],"getUpperLimit")==0)
	{

		// if number of parameters are wrong
		if(argc!=2)
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
	else if(strcasecmp(args[0],"getLowerLimit")==0)
	{

		// if number of parameters are wrong
		if(argc!=2)
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
	else if(strcasecmp(args[0],"cal")==0)
	{
		// if number of parameters are wrong
		if(argc!=2)
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



	// --- if command is save---------------------------------------
	else if(strcasecmp(args[0],"save")==0)
	{
		// if number of parameters are wrong
		if(argc!=1)
		{
			strcpy(mess, "ERROR: Required number of parameters: 1");
			return -1;
		}

		strcpy(mess,savePositions(positionsFileName));
		if(strstr (mess,"ERROR")!=NULL)
			return -1;

		strcpy(mess,saveToNewConfigFile(configFileName,configNewFileName));
		if(strstr (mess,"ERROR")!=NULL)
			return -1;

#ifndef LASERBOX
		WriteWarmupTimestamps(warmupTimeFileName);
#endif

		sprintf(mess,"Positions have been saved to %s and a new config file has been created as %s with updated values",positionsFileName.c_str(),configNewFileName.c_str());

		return 0;
	}




	// --- if command is list---------------------------------------
	else if(strcasecmp(args[0],"list")==0)
	{
		char *p= new char[1];
		// if number of parameters are wrong
		if(argc!=1)
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

#ifndef LASERBOX
	// --- if command is fllist---------------------------------------
	else if(strcasecmp(args[0],"fllist")==0)
	{
		// if number of parameters are wrong
		if(argc!=1)
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
#endif


	// --- if command is pos---------------------------------------
	else if(strcasecmp(args[0],"pos")==0)
	{
		// if number of parameters are wrong
		if(argc!=2)
		{
			strcpy(mess, "ERROR: Required number of parameters: 2");
			return -1;
		}
		// find motor name (case insensitive) and execute cmd
		for(int i=0;i<Motor::NumMotors;i++)
			if(strcasecmp(args[1],motor[i]->getName())==0)
			{
				sprintf(mess,"%f",motor[i]->getPosition());
				return 0;
			}

	}




	// --- if command is depos for debugging---------------------------
	else if(strcasecmp(args[0],"depos")==0)
	{
		// if number of parameters are wrong
		if(argc!=2)
		{
			strcpy(mess, "ERROR: Required number of parameters: 2");
			return -1;
		}
		// find motor name (case insensitive) and execute cmd
		for(int i=0;i<Motor::NumMotors;i++)
			if(strcasecmp(args[1],motor[i]->getName())==0)
			{
				sprintf(mess,"%f",motor[i]->debugPosition());
				return 0;
			}
	}



#ifndef LASERBOX
	// --- if command is getcenter---------------------------------------
	else if(strcasecmp(args[0],"getcenter")==0)
	{
		//if slits are not included in config file
		if(slit == NULL)
		{
			strcpy(mess, "ERROR: The slit motors do not exist in the config file.");
			return -1;
		}
		// if number of parameters are wrong
		if(argc!=1)
		{
			strcpy(mess, "ERROR: Required number of parameters: 1");
			return -1;
		}

		sprintf(mess,"%f",slit->getX1Center());
		return 0;
	}



	// --- if command is getslitwidth---------------------------------------
	else if(strcasecmp(args[0],"getslitwidth")==0)
	{
		//if slits are not included in config file
		if(slit == NULL)
		{
			strcpy(mess, "ERROR: The slit motors do not exist in the config file.");
			return -1;
		}
		// if number of parameters are wrong
		if(argc!=1)
		{
			strcpy(mess, "ERROR: Required number of parameters: 1");
			return -1;
		}

		sprintf(mess,"%f",slit->getSlitWidth());
		return 0;
	}






	// --- if command is widthrel---------------------------------------
	else if(strcasecmp(args[0],"widthrel")==0)
	{
		//if slits are not included in config file
		if(slit == NULL)
		{
			strcpy(mess, "ERROR: The slit motors do not exist in the config file.");
			return -1;
		}

		double halfWidth;
		// if number of parameters are wrong
		if(argc!=2)
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
	else if(strcasecmp(args[0],"widthabs")==0)
	{
		//if slits are not included in config file
		if(slit == NULL)
		{
			strcpy(mess, "ERROR: The slit motors do not exist in the config file.");
			return -1;
		}

		// if number of parameters are wrong
		if(argc!=2)
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
	else if(strcasecmp(args[0],"centerrel")==0)
	{
		//if slits are not included in config file
		if(slit == NULL)
		{
			strcpy(mess, "ERROR: The slit motors do not exist in the config file.");
			return -1;
		}

		// if number of parameters are wrong
		if(argc!=2)
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
	else if(strcasecmp(args[0],"centerabs")==0)
	{
		//if slits are not included in config file
		if(slit == NULL)
		{
			strcpy(mess, "ERROR: The slit motors do not exist in the config file.");
			return -1;
		}

		// if number of parameters are wrong
		if(argc!=2)
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
	else if(strcasecmp(args[0],"exactcenter")==0)
	{
		//if slits are not included in config file
		if(slit == NULL)
		{
			strcpy(mess, "ERROR: The slit motors do not exist in the config file.");
			return -1;
		}

		// if number of parameters are wrong
		if(argc!=1)
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
	else if(strcasecmp(args[0],"x1zerowidth")==0)
	{
		//if slits are not included in config file
		if(slit == NULL)
		{
			strcpy(mess, "ERROR: The slit motors do not exist in the config file.");
			return -1;
		}

		// if number of parameters are wrong
		if(argc!=1)
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




	/* --- if command is 'zerowidth' to move both slits simultaneously
     right next to each other to an absolute position of the slit specified--------------------------
	 */
	else if(strcasecmp(args[0],"zerowidth")==0)
	{
		//if slits are not included in config file
		if(slit == NULL)
		{
			strcpy(mess, "ERROR: The slit motors do not exist in the config file.");
			return -1;
		}

		// if number of parameters are wrong
		if(argc!=2)
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
#endif




	// --- if command is moverel------------------------------------
	else if(strcasecmp(args[0],"moverel")==0)
	{

		// if number of parameters are wrong
		if(argc!=3)
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
			if(strcasecmp(args[1],motor[i]->getName())==0)
			{
				newPosition=motor[i]->getPosition()+atof(args[2]);

#ifndef LASERBOX
				// if its a slit motor, checks if the slit motor can move to the new position without crashin into other one
				if(strcasecmp(args[1],"slit_x1")==0)
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
				else if(strcasecmp(args[1],"slit_x2")==0)
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
#endif

				if(!motor[i]->canMotorMove(newPosition))
				{
					sprintf(mess, "ERROR: Position given to move motor %s is beyond its limits: %f and %f",args[1],motor[i]->getLowerLimit(),motor[i]->getUpperLimit());
					return -1;
				}

				motor[i]->moveRel(atof(args[2]),0,0);

				//set position member of motor to the updated position
				motor[i]->setPosition(newPosition);
#ifndef LASERBOX
				//set slit positions and limits in slit class
				if(slitnum==1)
					slit->setX1pos(newPosition);
				else if(slitnum==2)
					slit->setX2pos(newPosition);
#endif
				sprintf(mess,"Moved %s by %s position",args[1],args[2]);
				return 0;
			}
	}






	// --- if command is simmoverel for simultaneous movt-----------------------------
	else if(strcasecmp(args[0],"simmoverel")==0)
	{
		num=2;
		// if number of parameters are wrong
		if(argc!=5)
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
			if(strcasecmp(args[1],motor[i]->getName())==0)
				// find second motor to move simultaneously
				for(int j=0;j<Motor::NumMotors;j++)
					if(strcasecmp(args[3],motor[j]->getName())==0)
					{
						// if both motors are the same, print error and exit
						if(strcasecmp(args[1],args[3])==0)
						{
							strcpy(mess,"ERROR: Both the motors to be moved simultaneously are the same");
							return -1;
						}

						//if they have the same controller, only then move
						if(strcmp(motor[i]->getController(),motor[j]->getController())==0)
						{
							newPosition=motor[i]->getPosition()+atof(args[2]);
							newPosition2=motor[j]->getPosition()+atof(args[4]);


#ifndef LASERBOX
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
									if(strcasecmp(args[1],"slit_x1")==0)
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
#endif

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

#ifndef LASERBOX
							if(  (!strcasecmp(args[1],"Huber")) ||(!strcasecmp(args[3],"Huber")) )
							{
								motor[i]->moveRel(atof(args[2]),0,0);
								motor[j]->moveRel(atof(args[4]),0,0);
							}
							else
#endif

								motor[i]->moveRel(atof(args[2]),motor[j]->getAxis(),atof(args[4]));
							//set position member of motors to the updated position
							motor[i]->setPosition(newPosition);
							motor[j]->setPosition(newPosition2);

#ifndef LASERBOX
							//setting both the positions and limits of the slit object if its the slit motors which moved
							if (slitnum==1)
								slit->setBothpos(newPosition,newPosition2);
							else if (slitnum==2)
								slit->setBothpos(newPosition2,newPosition);
#endif

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
	else if(strcasecmp(args[0],"moveabs")==0)
	{
		// if number of parameters are wrong
		if(argc!=3)
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
			if(strcasecmp(args[1],motor[i]->getName())==0)
			{

#ifndef LASERBOX
				// if its a slit motor, checks if the slit motor can move to the new position without crashin into other one
				if(strcasecmp(args[1],"slit_x1")==0)
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
				else if(strcasecmp(args[1],"slit_x2")==0)
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
#endif


				if(!motor[i]->canMotorMove(atof(args[2])))
				{
					sprintf(mess, "ERROR: Position given to move motor %s is beyond its limits: %f and %f",args[1],motor[i]->getLowerLimit(),motor[i]->getUpperLimit());
					return -1;
				}

				motor[i]->moveAbs(atof(args[2]),0,0,0);
				//set position member of motor to the updated position
				motor[i]->setPosition(atof(args[2]));

#ifndef LASERBOX
				//set slit positions and limits in slit class
				if(slitnum==1)
					slit->setX1pos(atof(args[2]));
				else if(slitnum==2)
					slit->setX2pos(atof(args[2]));
#endif


				sprintf(mess,"Moved %s to %s position",args[1],args[2]);
				return 0;
			}
	}





	// --- if command is sim2moveabs for simultaneous movt of 2 motors-----------------------------
	else if(strcasecmp(args[0],"sim2moveabs")==0)
	{
		num=2;
		// if number of parameters are wrong
		if(argc!=5)
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
			if(strcasecmp(args[1],motor[i]->getName())==0)
				// find second motor to move simultaneously
				for(int j=0;j<Motor::NumMotors;j++)
					if(strcasecmp(args[3],motor[j]->getName())==0)
					{
						// if both motors are the same, print error and exit
						if(strcasecmp(args[1],args[3])==0)
						{
							strcpy(mess,"ERROR: Both the motors to be moved simultaneously are the same");
							return -1;
						}
						//if they have the same controller, move
						if(strcmp(motor[i]->getController(),motor[j]->getController())==0)
						{


#ifndef LASERBOX
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
									if(strcasecmp(args[1],"slit_x1")==0)
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
#endif


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

#ifndef LASERBOX
							if(  (!strcasecmp(args[1],"Huber")) ||(!strcasecmp(args[3],"Huber")) )
							{
								motor[i]->moveAbs(atof(args[2]),0,0,0);
								motor[j]->moveAbs(atof(args[4]),0,0,0);
							}
							else
#endif

								motor[i]->moveAbs(atof(args[2]),motor[j]->getAxis(),atof(args[4]), motor[j]->getPosition());

							//set position member of motors to the updated position
							motor[i]->setPosition(atof(args[2]));
							motor[j]->setPosition(atof(args[4]));


#ifndef LASERBOX
							//setting both the positions and limits of the slit object if the motors to be moved were slits
							if(slitnum==1)
								slit->setBothpos(atof(args[2]),atof(args[4]));
							else if(slitnum==2)
								slit->setBothpos(atof(args[4]),atof(args[2]));
#endif


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
	else if(strcasecmp(args[0],"sim3moveabs")==0)
	{
		num=3;
#ifndef LASERBOX
		int x1=0,x2=0;
#endif
		double pos1,pos2,pos3;
		// if number of parameters are wrong
		if(argc!=7)
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
			if(strcasecmp(args[1],motor[i]->getName())==0)
				// find second motor
				for(int j=0;j<Motor::NumMotors;j++)
					if(strcasecmp(args[3],motor[j]->getName())==0)
						//find third motor
						for(int k=0;k<Motor::NumMotors;k++)
							if(strcasecmp(args[5],motor[k]->getName())==0)
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


#ifndef LASERBOX
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
#endif


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


#ifndef LASERBOX
									if(  (!strcasecmp(args[1],"Huber")) ||(!strcasecmp(args[3],"Huber")) || (!strcasecmp(args[5],"Huber")) )
									{
										motor[i]->moveAbs(atof(args[2]),0,0,0);
										motor[j]->moveAbs(atof(args[4]),0,0,0);
										motor[k]->moveAbs(atof(args[6]),0,0,0);
									}
									else
									{
#endif

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

#ifndef LASERBOX
									}
#endif

									//set position member of motors to the updated position
									motor[i]->setPosition(atof(args[2]));
									motor[j]->setPosition(atof(args[4]));
									motor[k]->setPosition(atof(args[6]));


#ifndef LASERBOX
									//setting both the positions and limits of the slit object if the motors to be moved were slits
									if(slitnum)
										slit->setBothpos(atof(args[x1+1]),atof(args[x2+1]));
#endif

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
	}

	// for wrong commands-----------------------------------------
	else
	{
		sprintf(mess,"ERROR: %s is not a Command",args[0]);
		return -1;
	}

	// if motor not found, print not found; if not simultaneous, args[1] is motor else args[1] and args[3]
#ifndef LASERBOX
	if (num==1) sprintf(mess,"ERROR:This motor %s is not listed in the config file.",args[1]);
#else
	if (num==4) sprintf(mess,"ERROR:This reference point %s is not listed in the config file.",args[1]);
	else if (num==0) sprintf(mess,"ERROR:This filter wheel %s is not listed in the config file.",args[1]);
	else if (num==1) sprintf(mess,"ERROR:This motor %s is not listed in the config file.",args[1]);
#endif

	else if(num==2)sprintf(mess,"ERROR: Either this motor %s or this motor %s is not listed in the config file.",args[1],args[3]);
	else sprintf(mess,"ERROR: Either this motor %s, this motor %s or this motor %s is not listed in the config file.",args[1],args[3],args[5]);
	return -1;


}

//-------------------------------------------------------------------------------------------------------------------------------------------------
/**  closes all the serialfds
 */
Initialize::~Initialize()
{
#ifdef LOCAL
	//closes all the serialfds
	for(int i=0;i<NUMBER_OF_CONTROLLERS;i++)
		interface[i]->close_serialfd();

#ifndef LASERBOX
	if(tubeInterface!=NULL)
		tubeInterface->close_serialfd();
#ifdef VACUUMBOX
	if(pressureInterface!=NULL)
		pressureInterface->close_serialfd();
#endif
#endif

#endif
}

//-------------------------------------------------------------------------------------------------------------------------------------------------
/**  reads the config file to create motor objects and store controller values
     and reading each line calls the init() function for further processing
 */

#ifndef LASERBOX
Initialize::Initialize(string const fName,string const fName2,string const fName3,string const fName4):slit1_exists(-1),slit2_exists(-1),maxTubePower(0),xrayStatus(-9), NUMBER_OF_CONTROLLERS(0)
#else
Initialize::Initialize(string const fName,string const fName2,string const fName3):NUMBER_OF_CONTROLLERS(0),NUMBER_OF_REFPOINTS(0)
#endif
{
#ifndef LASERBOX
	XrayTube = NULL;
	tubeInterface=NULL;
#ifdef VACUUMBOX
	pgauge = NULL;
	pressureInterface = NULL;
#endif
#endif

	configFileName = fName;
	positionsFileName = fName2;
	configNewFileName = fName3;
#ifndef LASERBOX
	warmupTimeFileName = fName4;
#endif

#ifdef LOCAL
//#ifndef DEBUG_SERVER

	//READ CONFIG FILE
	string sLine,sArgName;
	int iArgVal;
	ifstream inFile;

	// to store all the words in a line in the file
	char *args[10];
	for(int i=0;i<10;i++)
		args[i]=new char[1000];

#ifndef LASERBOX
	fluorList.clear();
	fluorListArray.clear();
#endif

	// opens file
	inFile.open(configFileName.c_str(), ifstream::in);
	if(inFile.is_open())
	{
		// repeats this for each line
		while(inFile.good())
		{
			// gets the line and saves it in sLine
			getline(inFile,sLine);

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
#ifndef LASERBOX
			else if (sLine.find("fluorescence")!=string::npos)
			{
				initFluorNames(sLine);
			}
#endif
			else
			{
				// divides the line into an arrray of strings, args[]
				istringstream sstr(sLine);
				iArgVal=0;
				while(sstr.good())
				{
					sstr>>sArgName;
					strcpy(args[iArgVal],sArgName.c_str());
					iArgVal++;
				}
				// calls init function for each line to do further processing,
				// sending number of arguments and array of arguments in 1 line as parameters

				init(iArgVal,args);
			}
		}
		// close file
		inFile.close();
#ifndef LASERBOX
		// After processing and the motor count is still 0, print error and exit
		if (Motor::NumMotors==0)
		{
			cout<<"ERROR:Config file not in proper format"<<endl<<endl;
			exit(-1);
		}
#else
		// After processing and the motor count and filter wheel count is still 0, print error and exit
		if ((Motor::NumMotors==0) && (Fwheel::NumFwheels))
		{
			cout<<"ERROR:Config file not in proper format"<<endl<<endl;
			exit(-1);
		}
#endif
	}
	// if file could not open, print error and exit
	else
	{
		cout<<"ERROR: Could not open file:"<<configFileName<<endl;
		exit(-1);
	}


	//VALIDATION
	//------------------------------------------
	// to ensure there is a required number of controllers for the motors
	if (((Motor::NumMotors>3)&&(NUMBER_OF_CONTROLLERS<2)) ||
			((Motor::NumMotors>6)&&(NUMBER_OF_CONTROLLERS<3)) )//took off the first condition repeated,maybe there's another condition missing
	{
		cout<<"\nERROR: The number of controllers is not sufficient for the motors. Add controller and fix config file."<<endl;
		exit(-1);
	}

	// to check theres no repetition for controllername in config file
	for(int i=0;i<Motor::NumMotors;i++)
		for(int j=0;j<Motor::NumMotors;j++)
		{
			if((i!=j)&&(!(strcasecmp(motor[i]->getName(),motor[j]->getName())) ||
					((motor[i]->getAxis()==motor[j]->getAxis()) &&
							(!(strcasecmp(motor[i]->getController(),motor[j]->getController()))))))
			{
				cout<<"\nERROR: In the config file,two motors should not have same name or connected to same axis of same controller."<<endl;
				exit(-1);
			}
		}

#ifndef LASERBOX
	//-----------------------------------
	// creates slit object if both slit_x1 and slit_x2
	if((slit1_exists < 0 && slit2_exists >= 0) || (slit1_exists >= 0 && slit2_exists < 0))
	{
		cout<<"\nERROR: Only one of the slit motors are connected or present in config file.\n\n";
		exit(-1);
	}
	else if (slit1_exists >= 0 && slit2_exists >= 0)
	{
		slit = new Slit(motor[slit1_exists]->getPosition(),motor[slit2_exists]->getPosition());
	}
	else // if both are zero, then they cant be positions in the motor array list
		slit = NULL;


	//----------------------------------
	//if fluorescence exists, all the validation requirements
	if(fluorListArray.size())
	{
		int numTargetHolders = fluorListArray.size();
		vector < vector < string> > lastTargetHolder = fluorListArray[numTargetHolders - 1];

		// there should be exactly maxfluor elements in each set, else error.
		if(lastTargetHolder.size() != maxfluorvalues)
		{
			cout<<"ERROR: There should be exactly " << maxfluorvalues << " targets in the fluorescence sets.\n";
			exit(-1);
		}

		// there should be atleast 3 strings for every target
		vector <string> lastTarget = lastTargetHolder[maxfluorvalues - 1];
		if (lastTarget.size() != FLUOR_PARA_NUM)
		{
			cout<<"ERROR: fluorescene list has inconsistent values. Not sufficient parameters for every fluorescence target." << endl;
			exit(-1);
		}

#ifdef XRAYBOX
		//rearranging the order..first to last

		for (size_t i = 0; i < fluorListArray.size(); ++i)
		{
			size_t first = 0;
			size_t last = maxfluorvalues - 1;
			while(first < last)
			{
				vector<string> temp = fluorListArray[i][first];
				fluorListArray[i][first] = fluorListArray[i][last];
				fluorListArray[i][last] = temp;
				++first;
				--last;
			}
		}
#endif
		//initially we take the first list for fluorescence
		fluorList = fluorListArray[0];

		// for debuging
		cout << "Fluoresence Targets" << endl;
		cout << "===================" << endl;
		// for each target holder
		for (size_t i = 0; i < fluorListArray.size(); ++i)
		{
			cout << "Target Holder " << i <<endl;
			// for each target
			for (size_t j = 0; j < maxfluorvalues; ++j)
			{
				cout << "\tTarget " << j << ": ";
				// for each target characteristic (name, energy, power)
				for (size_t k = 0; k < fluorListArray[i][j].size(); ++k)
				{
					cout << fluorListArray[i][j][k] << "\t";
				}
				cout << endl;
			}
			cout << endl;
		}

		cout << "Current Target Holder " <<endl;
		for(size_t i = 0; i < maxfluorvalues; ++i)
		{
			cout << "\tTarget " << i << ": ";
			for(size_t j = 0; j < fluorList[i].size(); ++j)
			{
				cout << fluorList[i][j] << "\t";
			}
			cout << endl;
		}
		cout << endl;
	}
	//--------------------------------
#endif

	// For debugging purposes: to print the motors and controllers read from file
	cout<<"Number of motors:"<<Motor::NumMotors<<endl;
	cout<<"Number of controllers:"<<NUMBER_OF_CONTROLLERS<<endl;
#ifdef LASERBOX
	cout<<"Number of filter wheels:"<<Fwheel::NumFwheels<<endl;
	cout<<"Number of refernce points:"<<NUMBER_OF_REFPOINTS<<endl;
#endif
	cout<<endl<<"Motors:"<<endl;
	cout<<"======="<<endl;
	for(int i=0;i<Motor::NumMotors;i++)
		motor[i]->print();
	cout<<endl;
    cout<<endl<<"Controllers:"<<endl;
    cout<<"============"<<endl;
	for(int i=0;i<NUMBER_OF_CONTROLLERS;i++)
	{
		cout<<"Name : "<<controller[i][0]<<endl;
		cout<<"Serial Number : "<<controller[i][1]<<endl;
		cout<<"Interface : "<<contInterface[i][1]<<endl<<endl;
	}
	cout<<endl;
#ifndef LASERBOX
	  //slit->print();
	//to put a default speed to all the controllers
	char changeVel[200];
	sprintf(changeVel,"%f setvel ",Motor::defaultSpeed);

	for(int k=0;k<NUMBER_OF_CONTROLLERS;k++)
		interface[k]->send_command(changeVel,0);
#else
	cout<<endl<<"Filter Wheels:"<<endl;
	cout<<"=============="<<endl;
	for(int i=0;i<Fwheel::NumFwheels;i++)
		fwheel[i]->print();
    cout<<endl<<"Reference Points:"<<endl;
    cout<<"================="<<endl;
	for(int i=0;i<NUMBER_OF_REFPOINTS;i++)
	{
		cout<<"Name \t\t: "<<referencePoints[i][0]<<endl;
		cout<<"Det x position\t: "<<referencePoints[i][1]<<endl;
		cout<<"Det y position\t: "<<referencePoints[i][2]<<endl;
		cout<<"Det z position\t: "<<referencePoints[i][3]<<endl<<endl;
	}
#endif
#ifndef LASERBOX
	if (!xrayStatus) {
	    cout<<endl<<"X-Ray Tube:"<<endl;
	    cout<<"==========="<<endl;
	    XrayTube->print();
	}
#ifdef VACUUMBOX
	if (pressureInterface != NULL) {
        cout<<endl<<"Pressure Gauge Controller:"<<endl;
        cout<<"=========================="<<endl;
	    pgauge->print();
	}
#endif
#endif

	// INTIALIZATION
	check_busy();
	setOrigPositions(positionsFileName);


#ifndef LASERBOX
	//to read the warmupTimeStamps
	warmupTimings.resize(61);
	for(int i=0;i<(int)warmupTimings.size();i++)
		warmupTimings[i].assign("unknown");
	ReadWarmupTimestamps(warmupTimeFileName);
#else
	//to make sure they work, move to pos 1
	for(int i=0;i<Fwheel::NumFwheels;i++)
	{
		fwheel[i]->setStartPosition();
		fwheel[i]->setStartPosition();//the second time it works
	}
#endif

//#endif  //debug_server
#endif  

}


#ifndef LASERBOX
//-------------------------------------------------------------------------------------------------------------------------------------------------
/**executes commands from the command line through both server and local
   @param sLine the fluorescence line read from the config file
 */
void Initialize::initFluorNames(string sLine)
{
	istringstream sstr(sLine);
	string sArgName;
	vector <string> args;

	for (int i = 0; i < FLUOR_PARA_NUM + 1; ++i) {
		if (!sstr.good()) {
			cout<<"ERROR: Could not scan fluorescence line " << sLine << endl;
			exit(-1);
		}
		sstr >> sArgName;
		args.push_back(sArgName);
	}

	// check where to insert the target
	int setIndex = fluorListArray.size() - 1;
	// first set (add an empty vector for the first set)
	if (setIndex < 0) {
		fluorListArray.push_back(std::vector < vector < string > > ());
		++setIndex;
	}
	// start next set (add an empty vector for the next set)
	if (fluorListArray[setIndex].size() == maxfluorvalues) {
		fluorListArray.push_back(std::vector < vector < string > > ());
		++setIndex;
	}
	//cout << "Set:" << setIndex <<endl;

	// adding target for each call to this method
	fluorListArray[setIndex].push_back(vector < string > ());

	size_t iPos = fluorListArray[setIndex].size() - 1;
	//cout << "Target pos: " << iPos << endl;

	for (size_t i = 0; i < args.size() - 1; i++) {
		fluorListArray[setIndex][iPos].push_back(args[i + 1]);
	}

}
#endif


//-------------------------------------------------------------------------------------------------------------------------------------------------
/** this function process current line read from config file, creates interface objects 
    after reading in all the controllers and then creates all the motor objects
    @param nArg number of arguments in current line read from file
    @param args array of arguments in current line read from file
 */
void Initialize::init(int nArg, char *args[])
{
	// the first argument is copied to mode, which is either 'controller' or 'motor'
	string mode=args[0];
	int temp;
	int exists=0;


#ifndef LASERBOX
	// if mode is POWER , set the max Power of tube------------------------------------------------------------------------------------
	if(mode=="power")
	{
		string temp;
		// if power is not a number
		temp.assign(args[1]);
		if(temp.find_first_not_of("0123456789")!=string::npos)
		{
			cout<<"ERROR:"<<args[1]<<"in the config file for max tube power should be a positive number"<<endl;
			exit(-1);
		}
		maxTubePower = atoi(args[1]);
		cout<<"\nTube power:"<<maxTubePower<<endl;
	}
#else
	// if mode is Fwheel, create fwheel objects and store its values-----------------------------------------------------------------
	if(mode=="fwheel")
	{
		vector<double>ValueList;
		// if number of arguments for the fwheel is incorrect, print error and exit
		if(nArg!=9)
		{
			cout<<"ERROR:In config file, there should be 9 arguments for fwheel mode. ";
			cout<<"Make sure there's no tab after each line."<<endl<<endl;
			exit(-1);
		}

		//check if the values are all numbers.
		string temp;
		for(int i=0;i<Fwheel::NumSlotsInWheel;i++)
		{
			temp.assign(args[i+3]);
			if(temp.find_first_not_of("0123456789.-")!=string::npos)
			{
				cout<<"ERROR: "<<args[i+3]<<" for absorption value for "<<args[1]<<" should be a number"<<endl<<endl;
				exit(-1);
			}
		}

		//saving the absorption values in a list
		ValueList.resize(Fwheel::NumSlotsInWheel);
		for(int i=0;i<Fwheel::NumSlotsInWheel;i++)
		{
			ValueList[i]=atof(args[i+3]);
			cout<<"valuelist:"<<i<<": "<<ValueList[i]<<" argsi+3:"<<args[i+3]<<endl;
		}


		//getting the interface port
		int usbNum=getFWInterface(args[2]);
		//if the serial number does not exist
		if(usbNum<0)
		{
			cout<<"ERROR: The filter wheel "<<args[1]<<" with serial number "<<args[2];
			cout<<" can't be found in the devices file. ";
			cout<<"Please verify the serial number in config file again"<<endl;
			exit(-1);
		}
		else
		{
			//creating the serial path
			char serial[200];
			strcpy(serial,"/dev/ttyUSBX");
			serial[11]=usbNum+48;

			//checking if the interfaces with same serial existed before
			if(Interface::NumFW_Interfaces>0)
			{
				for(int i=0;i<Interface::NumFW_Interfaces;i++)
				{
					//checking the last digit with usbNum
					if(fwInterface[i]->getSerial()[11]==usbNum+48)
					{
						cout<<"ERROR: The filter wheel "<<args[1]<<" "<<args[2]<<" has interface "<<serial<<" which seems to be";
						cout<<" already connected to another filter wheel/controller"<<endl;
						exit(-1);
					}
				}
			}

			//CREATE Interface
			bool success = false;
			fwInterface[Interface::NumFW_Interfaces] = new Interface(true,serial,&success);
			if(success){
				Interface::NumFW_Interfaces++;
				fwheel[Fwheel::NumFwheels] = new Fwheel(args[1], args[2],ValueList,fwInterface[Interface::NumFW_Interfaces-1]);
				Fwheel::NumFwheels++;
			}else
			{
				cout<<"ERROR:Was not able to create interface to filter wheel "<<args[1]<<" "<<args[2]<<endl;
				exit(-1);
			}

		}
	}

#endif








	// if mode is CONTROLLER, set the controller values for the 2-d arrays-------------------------------------------------------------
	else if(mode=="controller")
	{
		// if number of arguments for the controller is incorrect, print error and exit
		if(nArg!=3)
		{
			cout<<"ERROR:In config file, there should be 3 arguments for controller mode. Make sure there's no tab after each line."<<endl<<endl;
			exit(-1);
		}
		// resize 2-d arrays for each controller read from file
		NUMBER_OF_CONTROLLERS++;
		controller.resize(NUMBER_OF_CONTROLLERS);
		contInterface.resize(NUMBER_OF_CONTROLLERS);

		controller[NUMBER_OF_CONTROLLERS-1].resize(2);
		contInterface[NUMBER_OF_CONTROLLERS-1].resize(2);
		// copy controller names to 1st col in controller[][] and contInterface[][]
		controller[NUMBER_OF_CONTROLLERS-1][0]=args[1];
		contInterface[NUMBER_OF_CONTROLLERS-1][0]=args[1];
		// copy the serial numbers to 2nd col in controller[][]
		controller[NUMBER_OF_CONTROLLERS-1][1]=args[2];

	}

	// if mode is Motor, create motor objects and store its values-----------------------------------------------------------------
	else if(mode=="motor")
	{

		// to check theres no repetition for controllername in config file
		for(int i=0;i<NUMBER_OF_CONTROLLERS;i++)
			for(int j=0;j<NUMBER_OF_CONTROLLERS;j++)
				if((i!=j)&&((controller[i][0]==controller[j][0]) || (controller[i][1]==controller[j][1])))
				{
					cout<<"\nERROR: In the config file,two controllers should not be defined to have same name or same serial number."<<endl;
					exit(-1);
				}

		// if number of arguments for the motor is incorrect, print error and exit
		if(nArg!=7)
		{
			cout<<"ERROR:In config file, there should be 7 arguments in file for motor mode. Make sure there's no tab after each line."<<endl<<endl;
			exit(-1);
		}

		// the FIRST TIME  a motor line is read
		if(Motor::NumMotors==0)
		{
			// if no controllers are read upto now, print error and exit
			if (NUMBER_OF_CONTROLLERS==0)
			{
				cout<<"ERROR:Config file not in proper format. No controllers defined before all the motors in file."<<endl<<endl;
				exit(-1);
			}
			// if no controllers are read upto now, print error and exit
			else if (NUMBER_OF_CONTROLLERS>3)
			{
				cout<<"ERROR: Max of only 3 controllers allowed in config file"<<endl<<endl;
				exit(-1);
			}

			// before creating the first motor object,
			// creates interface objects acc to number of controllers


			char serial[200]="/dev/ttyUSBX";

			bool success=false;
			for(int usbNum=0;usbNum<10;usbNum++)
			{
				success=false;

				// for eg. Interface[0] will have serial as "/dev/ttyUSBX"
				serial[11]=usbNum+48;
				cout<<endl<<endl<<"******* " << serial << "******* " <<endl;

				interface[Interface::NumMotorcontroller_Interfaces]= new Interface(serial,&success);
				if(success)
					Interface::NumMotorcontroller_Interfaces++;
#ifndef LASERBOX
               if((!success)&&(tubeInterface==NULL)){
                    tubeInterface = new Interface(serial , &success, true);
                    if(success){
                        xrayStatus=0;
                        XrayTube = new Xray(tubeInterface);
                    }
                    else{
                        xrayStatus=-1;
                        tubeInterface = NULL;
                    }
                }

#ifdef VACUUMBOX
				if((!success)&&(pressureInterface==NULL)){
					pressureInterface = new Interface(serial, true, &success);
					if(success){
						pgauge = new Pgauge(pressureInterface);
					}
					else{
					    pgauge = NULL;
						pressureInterface = NULL;
					}
				}
#endif
#endif
			}

			// before creating first motor, after creating interface objects,
			// now set  2nd col of contInterface[][] with controller interface values
			//  by calling this function

			getContInterface();

			//disables all the axis of the controllers
			for(int i=0;i<NUMBER_OF_CONTROLLERS;i++)
				for(int j=1;j<4;j++)
				{
					char message[200];
					sprintf(message,"0 %d setaxis ",j);
					interface[i]->send_command(message,0);
				}

		}

		// for all motors including the first one, find the interface
		//  of the controller of the motor from 2nd col of contInterface[][] to give as argument in motor constructor

		for(int j=0;j<NUMBER_OF_CONTROLLERS;j++)
		{
			if(args[3]==contInterface[j][0])
			{
				char* p=new char[contInterface[j][1].length()+1];
				strcpy(p,contInterface[j][1].c_str());
				// get the last digit of the string, for eg. get 2 from '/dev/ttyUSB2'
				temp= atoi(p+11);

				//setting temp to point to the right interface[] element
				for(int i=0;i<NUMBER_OF_CONTROLLERS;i++)
					if(atoi(interface[i]->getSerial()+11)==temp)
					{
						temp=i;
						break;
					}

				// the controller name for motor section is defined under the controller section in config file
				exists=1;
				break;
			}
		}

		// check if controller name exists in controller[][], else print error and exit
		if(!exists)
		{
			cout<<"\nERROR in config file. A motor has a controller name which is not defined in config file under #controller.\n\n";
			exit(-1);
		}


#ifndef LASERBOX
		//if motor is slit_x1 or slit_x2,or fluorescence set the slit_exist variables or fluor_exists variable to the position in motor[]
		if(strcasecmp(args[1],"slit_x1")==0)
			slit1_exists=Motor::NumMotors;
		else if(strcasecmp(args[1],"slit_x2")==0)
			slit2_exists=Motor::NumMotors;
#endif

		//checks if axis values are either 1, 2 or 3
		if((atoi(args[2])>3)||(atoi(args[2])<0))
		{
			cout<<"Error in config file: Axis must be 1, 2 or 3"<<endl;
			exit(-1);
		}

		//checks if upper limit is greater than lower limit, else print error
		if(atoi(args[5])>atoi(args[6]))
		{
			cout<<"Error in config file: Upper limit must be greater than lower limit"<<endl;
			exit(-1);
		}


#ifndef LASERBOX
		//if motor is huber..set calspeed to huberCalSpeed defined in .h file & also find the default speed
		if(!strcasecmp(args[1],"Huber"))
		{
			char buffer[200];
			strcpy(buffer,interface[temp]->send_command((char*)"getvel ",1));
			//Motor::defaultSpeed = 30.00;//atof(buffer);
			sprintf(buffer,"%f 1 %s setncalvel ",Motor::huberCalSpeed,args[2]);  // [velocity][index=1 for 'to the limit switch'][axis] setncalvel
			interface[temp]->send_command(buffer,0);
		}
#endif

		//to enable this axis of the controller
		char message[200];
		sprintf(message,"1 %d setaxis ",atoi(args[2]));
		interface[temp]->send_command(message,0);


#ifdef LASERBOX
		//setting pitch
		if(!strcasecmp(args[1],"detector_x"))
		{
			sprintf(message,"2 %d setpitch ",atoi(args[2]));
			interface[temp]->send_command(message,0);
		}

		if(!strcasecmp(args[1],"detector_y"))
		{
			sprintf(message,"1 %d setpitch ",atoi(args[2]));
			interface[temp]->send_command(message,0);
		}

		if(!strcasecmp(args[1],"detector_z"))
		{
			sprintf(message,"2 %d setpitch ",atoi(args[2]));
			interface[temp]->send_command(message,0);
		}

#endif

		// creates motor object with an interface pointer. Motor[i] points to these motor objects
		motor[Motor::NumMotors]=new Motor(args[1],atoi(args[2]),args[3],atof(args[4]),atof(args[5]),atof(args[6]), 0.000, interface[temp]);
		// increases static memeber for number of motor objects
		Motor::NumMotors++;
	}


#ifdef LASERBOX
	else if(mode=="refpoint")
	{
		// if number of arguments for the refpoint is incorrect, print error and exit
		if(nArg!=5)
		{
			cout<<"ERROR:In config file, there should be 5 arguments for refpoint mode. Make sure there's no tab after each line."<<endl<<endl;
			exit(-1);
		}

		//check if the positions are all numbers
		string temp2,temp3,temp4;
		temp2.assign(args[2]);
		temp3.assign(args[3]);
		temp4.assign(args[4]);
		if((temp2.find_first_not_of("0123456789.-")!=string::npos)||
			(temp3.find_first_not_of("0123456789.-")!=string::npos)||
			(temp4.find_first_not_of("0123456789.-")!=string::npos))
		{
			cout<<"ERROR: Positions for detector_x, detector_y and detector_z should be numbers in the config file"<<endl<<endl;
			exit(-1);
		}

		// resize 2-d arrays for each ref points read from file
		NUMBER_OF_REFPOINTS++;
		referencePoints.resize(NUMBER_OF_REFPOINTS);
		referencePoints[NUMBER_OF_REFPOINTS-1].resize(4);
		// copy ref name
		referencePoints[NUMBER_OF_REFPOINTS-1][0].assign(args[1]);
		// copy pos of detx
		referencePoints[NUMBER_OF_REFPOINTS-1][1].assign(args[2]);
		// copy pos of dety
		referencePoints[NUMBER_OF_REFPOINTS-1][2].assign(args[3]);
		// copy pos of detz
		referencePoints[NUMBER_OF_REFPOINTS-1][3].assign(args[4]);
	}
#endif


	// if mode is not 'controller' or 'motor' or 'fluorescence' or power, print error and exit
	else
	{
#ifndef LASERBOX
		cout<<"ERROR: Mode in config file should be either controller, motor, fluorescence or power"<<endl;
#else
		cout<<"ERROR: Mode in config file should be either controller, motor ,fwheel or refpoint"<<endl;
#endif
		exit(-1);
	}

}


#ifdef LASERBOX
//------------------------------------------------------------------------------------------------------------------------------------------------

/**returns the interface number by specifying the serial number of the filter wheel
 */
int Initialize::getFWInterface(string serialNum)
{

#ifdef VERBOSE_MOTOR
	cout<<"\n-----Getting the filter wheel interface-----"<<endl<<endl;
#endif

	char output[255],systemCommand[200];
	string sOutput;
	int usbNum;
	bool found=false;

	//checking each ttyUSB for serial number
	for(usbNum=0;usbNum<10;usbNum++)
	{
		//reset output
		strcpy(output,"");
		//the command used to get the usb interface number depending on the serial number of filter wheel
		strcpy(systemCommand,"more /sys/class/tty/ttyUSBX/device/../../serial");
	    systemCommand[26]=usbNum+48;

		//using sys cmds to get output or str
		FILE* sysFile = popen(systemCommand, "r");
		fgets(output, sizeof(output), sysFile);
		pclose(sysFile);

		sOutput.assign(output);
		if(sOutput.find(serialNum)!=string::npos)
		{
			found=true;
			break;
		}
	}
#ifdef VERBOSE_MOTOR
	cout<<"\n-----Done getting the filter wheel interface-----"<<endl<<endl;
#endif
	if(found)
		return usbNum;
	else
		return -1;
}

#endif



//------------------------------------------------------------------------------------------------------------------------------------------------
/** gets the controller interface and fills the 2nd col of
    contInterface[][] with respective serial ports
 */
void Initialize::getContInterface()
{
	int temp=0,i,contNo;
	char buffer[255]="";
	int contFound=0;

#ifdef VERBOSE_MOTOR
	cout<<"\n-----Getting the controller interface-----"<<endl<<endl;
#endif

	// send getserialno commands to each Interface port, repeat this for each Interface port
	for(int k=0;k<Interface::NumMotorcontroller_Interfaces;k++)
	{
		if(contFound==NUMBER_OF_CONTROLLERS) break;
		strcpy(buffer,interface[k]->send_command((char*)"getserialno ",1));

		// convert serial number returned to integer
		contNo=atoi(buffer);
		cout<<"\ncontNo:"<<contNo;
		// fill the respective 2nd col of contInterface[][] with serial port
		for(i=0;i<NUMBER_OF_CONTROLLERS;i++)
		{
			temp = atoi(controller[i][1].c_str());

			// contNo= serial number returned by the particular port, temp= serial number in array
			if(contNo==temp)
			{
				// assigns 2nd col of contInterface[][] for the particular controller with the current serial port
				contInterface[i][1].assign(interface[k]->getSerial());
				contFound++;
				cout<<":"<<interface[k]->getSerial()<<":"<<contInterface[i][1];
				break;
			}
		}

	}
	cout<<endl<<endl;
#ifdef VERBOSE_MOTOR
	cout<<"\n-----Done getting the controller interface-----"<<endl<<endl;
#endif
}

//------------------------------------------------------------------------------------------------------------------------------------------------
/** save positions of all the motors to a Positions.txt file
 */
char* Initialize::savePositions(string const fName)
{
#ifdef VERBOSE_MOTOR
	cout<<"\n-----Saving the positions to file-----"<<endl<<endl;
#endif
	// opens file
	ofstream outFile;
	char message[200];
	char* p = message;
	char* name = new char[fName.length()+1];

	outFile.open(fName.c_str());
	if(outFile.is_open())
	{
		outFile<<setw(15)<<left<<"#Motor Name"<<"Positions\n\n";

		// prints motor name and the positions
		for(int i=0;i<Motor::NumMotors;i++)
		{
			outFile<<setw(15)<<left<<motor[i]->getName()<<setprecision(17)<<motor[i]->getPosition()<<endl;
			//cout<<setprecision(17)<<motor[i]->debugPosition()<<"\t";
		}
		// close file
		outFile.close();
#ifdef VERBOSE_MOTOR
		cout<<"\n-----Done saving positions to file-----"<<endl<<endl;
#endif
	}

	// if file does not exist, print error and exit
	else
		sprintf(message,"ERROR: Could not open %s",name);

	return p;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
/** save positions of all the motors to a Positions.txt file
 */
char* Initialize::saveToNewConfigFile(string const oldFName, string const fName)
{
#ifdef VERBOSE_MOTOR
	cout<<"\n-----Creating new config file to save changed data-----"<<endl<<endl;
#endif

	ofstream outFile;
	ifstream inFile;
	string sLine;
	char* cLine;
	char motorName[200];
	int loop;
	char message[200];
	char*p= message;
	char* name;

	//for debugging
	//motor[3]->setSpeed(45);

	outFile.open(fName.c_str());
	inFile.open(oldFName.c_str(), ifstream::in);

	if(outFile.is_open())
	{
		if(inFile.is_open())
		{
			while(inFile.good())
			{
				getline(inFile,sLine);

				//for comments, empty lines, fluorescence mode, controller mode
#ifndef LASERBOX
				if((sLine.find('#')!=string::npos) || (sLine.length()<2) || (sLine.find("controller")!=string::npos)|| (sLine.find("fluorescence")!=string::npos) )
#else
				if((sLine.find('#')!=string::npos) || (sLine.length()<2) || (sLine.find("controller")!=string::npos)|| (sLine.find("fwheel")!=string::npos))
#endif
					outFile<<sLine<<endl;

				//motor mode
				else if(sLine.find("motor")!=string::npos)
				{
					cLine = new char[sLine.length()+1];
					strcpy(cLine,sLine.c_str());
					istringstream sstr(cLine);

					//for mode "motor"
					if(sstr.good())
					{
						sstr>>cLine;
						outFile<<cLine<<"\t";
					}

					//for name
					if(sstr.good())
					{
						sstr>>motorName;
#ifndef LASERBOX
						if(!strcasecmp(motorName,"fluorescence"))
							outFile<<motorName<<"\t";
						else
#endif
							outFile<<motorName<<"\t\t";
					}

					//axis, controller
					for(int i=0;i<2;i++)
						if(sstr.good())
						{
							sstr>>cLine;
							outFile<<cLine<<"\t";
						}

					//find motor in Motor[] to compare speed and limits
					for(loop=0;loop<Motor::NumMotors;loop++)
					{
						if(!strcmp(motorName,motor[loop]->getName()))
							break;
					}

					//for speed
					if(sstr.good())
					{
						sstr>>cLine;
						if(motor[loop]->getSpeed()==atof(cLine))
							outFile<<cLine<<"\t";
						else
							outFile<<motor[loop]->getSpeed()<<"\t";
					}

					//for lower limit
					if(sstr.good())
					{
						sstr>>cLine;
						if(motor[loop]->getLowerLimit()==atof(cLine))
							outFile<<cLine<<"\t\t";
						else
							outFile<<motor[loop]->getLowerLimit()<<"\t\t";
					}

					//for upper limit
					if(sstr.good())
					{
						sstr>>cLine;
						if(motor[loop]->getUpperLimit()==atof(cLine))
							outFile<<cLine<<"\t";
						else
							outFile<<motor[loop]->getUpperLimit()<<"\t";
					}

					outFile<<endl;
				}
			}
			inFile.close();
		}
		else
		{
			name = new char[oldFName.length()+1];
			sprintf(message,"ERROR: Could not open %s",name);
		}

		outFile.close();
#ifdef VERBOSE_MOTOR
		cout<<"\n-----Done creating new config file to save changed data-----"<<endl<<endl;
#endif
	}
	else
	{
		name = new char[fName.length()+1];
		sprintf(message,"ERROR: Could not open %s",name);
	}
	return p;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
/** set the original positions if different from Positions.txt file
 */
void Initialize::setOrigPositions(string const fName)
{
	ifstream inFile;
	string sLine;
	char motorName[200], pos[200];
	bool found;
	int i;
	char positions[3][200];
	char command[200];

#ifdef VERBOSE_MOTOR
	cout<<"\n-----Setting original positions-----"<<endl<<endl;
#endif

	//oprning the positions file
	inFile.open(fName.c_str(), ifstream::in);
	if(inFile.is_open())
	{
		while(inFile.good())
		{
			getline(inFile,sLine);

			if(sLine.find('#')!=string::npos)
			{
				continue;
			}
			else if(sLine.length()<2)
			{
				continue;
			}
			else
			{
				//read motor name
				istringstream sstr(sLine);
				if(sstr.good())
					sstr>>motorName;

				found=false;
				//find the motor in the loop
				for(i=0;i<Motor::NumMotors;i++)
					if(!strcasecmp(motor[i]->getName(),motorName))
					{
						found=true;
						break;
					}
				//update the corresponding motor's position
				if(found)
					if(sstr.good())
					{
						sstr>>pos;
						if(found) motor[i]->setPosition(atof(pos));
					}
			}
		}
		inFile.close();
	}
	else
	{
		cerr<<"Error opening positions file.\n";
		exit(-1);
	}

	// for each controller
	for(int i=0;i<NUMBER_OF_CONTROLLERS;i++)
	{

		//resetting the values of positions[]
		for(int j=0;j<3;j++)
			strcpy(positions[j],"0");

		//for each motor, if the controller is the same as loop, copy its position to the rite position[axis -1]
		for(int j=0;j<Motor::NumMotors;j++)
			if(!strcmp(controller[i][0].c_str(),motor[j]->getController()))
				sprintf(positions[motor[j]->getAxis()-1],"%f",0-motor[j]->getPosition());


		strcpy(command,positions[0]);
		strcat(command," ");
		strcat(command,positions[1]);
		strcat(command," ");
		strcat(command,positions[2]);
		strcat(command," ");
		strcat(command,"setpos ");


		//find the interface and send this command to controller
		for(int k=0;k<NUMBER_OF_CONTROLLERS;k++)
			if(!strcmp(interface[k]->getSerial(),contInterface[i][1].c_str()))
			{
				interface[k]->send_command(command,0);
				break;
			}
	}

#ifndef LASERBOX
	if(slit!=NULL)// this is made null in Initialize constructor when slit1_exists and slit2_exists are both 0 and both cant b 0 for motor positions
	{
		slit->setX1pos(motor[slit1_exists]->getPosition());
		slit->setX2pos(motor[slit2_exists]->getPosition());
	}
#endif


#ifdef VERBOSE_MOTOR
	cout<<"\n-----Done setting original positions-----"<<endl<<endl;
#endif

}

//------------------------------------------------------------------------------------------------------------------------------------------------
/** waits till all the controllers are not busy
 */
void Initialize::check_busy()
{

	char buffer[200]="";
	int check;
#ifdef VERBOSE_MOTOR  
	cout<<"\n-----Checking if the controllers are busy-----"<<endl<<endl;
#endif 
	for(int i=0;i<NUMBER_OF_CONTROLLERS;i++)
	{
		// check=1 to keep checking till the controller is not busy
		check=1;
		while(check)
		{
			// send 'st ' command to check status of controller
			strcpy(buffer,interface[i]->send_command((char*)"st ",1));

			buffer[1]='\0';
			// if controller is not busy, check=0 and start checking next controller
			if(strcmp(buffer,"0")==0)
				check=0;
			// if busy, wait and try again
			else
				usleep(50000);
		}
	}
#ifdef VERBOSE_MOTOR
	cout<<"\n-----Done checking if the controllers are busy-----"<<endl<<endl;
#endif

}


#ifndef LASERBOX
//------------------------------------------------------------------------------------------------------------------------------------------------
/** reads warmuptimestamps from file
 */
int Initialize::ReadWarmupTimestamps(string const fName)
{
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
//------------------------------------------------------------------------------------------------------------------------------------------------
/** writes warmuptimestamps from file
 */
int Initialize::WriteWarmupTimestamps(string const fName)
{
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
//------------------------------------------------------------------------------------------------------------------------------------------------
#endif
