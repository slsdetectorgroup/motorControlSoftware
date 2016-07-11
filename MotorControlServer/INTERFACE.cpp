/********************************************//**
 * @file INTERFACE.cpp
 * @short Defines the interface objects
 * @author Dhanya
 ***********************************************/
#include "INTERFACE.h"
#include <fcntl.h>  /* File control definitions */
#include <iostream>
#include <cstring>
#include <cstdlib> //exit
#include <unistd.h> //usleep in raspberrypi
using namespace std;


	int INTERFACE::NumInterfaces(0);
#ifdef LASERBOX
	int INTERFACE::NumFW_Interfaces(0);
#endif

INTERFACE::INTERFACE(char* serial, bool* success)
{
	/* serial port is copied to serial*/
	strcpy(this->serial,serial);
	cout<<"Checking serial:"<<serial<<endl;

	serialfd= open (serial, O_RDWR | O_NOCTTY | O_NDELAY);

	/* exits program if port cant be opened */
	if (serialfd==-1)
	{
		cout<<"WARNING: Unable to open port "<<serial<<", check permissions to use it next time"<<endl;
		//exit(-1);
	}
	else
	{
		/* control options */
		new_serial_conf.c_cflag = B57600 | CS8 | CREAD | CLOCAL;
		/* input options */
		new_serial_conf.c_iflag = 0;
		/* output options */
		new_serial_conf.c_oflag = 0;
		/* line options */
		new_serial_conf.c_lflag = ICANON;
		/* flush input */
		tcflush(serialfd, TCIFLUSH);
		tcsetattr(serialfd, TCSANOW, &new_serial_conf);

		/*
		for serial programming see:  http://www.easysw.com/~mike/serial/serial.html
		 */


		//sending a command st to controller to check if the usb is  really connected and can read data back from controller
		char command[200]="st ";
		char buffer[200]="";

		//so that the error doesnt print a million times
		int writeValue=-1, attempt=0;
		while(writeValue==-1)  //while (write (serialfd,command,strlen(command))==-1)
		{
			writeValue = write (serialfd,command,strlen(command));
			attempt++;
			if(attempt==10)
				cout<<"error (attempt number10) sending the command "<<command<<" to port"<<serial<<" trying again"<<endl;
		}
		if(attempt>10)
			cout<<"succesful in sending the command,"<<command<<" to port"<<serial<<endl;



		buffer[0]='\0';
		usleep(50000);

		for(int i=0;i<100;i++)
			if(read(serialfd, buffer, 255)!=-1)
			{
				*success = true;
				break;
			}

		if(*success==false){
			cout << "**************************Not successfull in reading from buffer" << endl;
			close_serialfd();
		}
	}  
}


#ifndef LASERBOX
//-------------------------------------------------------------------------------------------------------------------------------------------------
INTERFACE::INTERFACE(char* serial, bool* success, bool xray)
{
	/* serial port is copied to serial*/
	strcpy(this->serial,serial);

	//O_NOCTTY flag tells UNIX that this program doesn't want to be the "controlling terminal" for that port.
	//O_NDELAY flag tells UNIX that this program doesn't care what state the DCD signal line is in - whether the other end of the port is up and running. If you do not specify this flag, your process will be put to sleep until the DCD signal line is the space voltage.

	serialfd= open (serial, O_RDWR | O_NOCTTY | O_NDELAY);

	/* exits program if port cant be opened */
	if (serialfd==-1)
	{
		cout<<"ERROR: Unable to open port "<<serial<<" for xray tube, check permissions to use it\n";
		//exit(-1);
	}
	else
	{
		/* control options */
		//B9600	9600 baud
		//CS8	8 data bits
		//CSTOPB	2 stop bits (1 otherwise)
		//CLOCAL	Local line - do not change "owner" of port
		//CREAD	Enable receiver
#ifdef XRAYBOX
		new_serial_conf.c_cflag = B9600 | CS8 | CSTOPB | CLOCAL | CREAD ;
#else
		new_serial_conf.c_cflag = B9600 | CS8 | CLOCAL | CREAD ;
#endif
		/* input options */
		//IGNPAR	Ignore parity errors
		new_serial_conf.c_iflag = IGNPAR;

		/* output options */
		new_serial_conf.c_oflag = 0;

		/* line options */
		new_serial_conf.c_lflag = 0; //ICANON;

		/* flush input */
		tcflush(serialfd, TCIFLUSH);
		tcsetattr(serialfd, TCSANOW, &new_serial_conf);

		int a,b;
		send_command_to_tube((char*)"sr:12 ", 1, a,b);
		send_command_to_tube((char*)"sr:12 ", 1, a,b);
		if(a==-9999){
			close_serialfd();
			*success = false;
		}
		else
			*success = true;
	}

}
#else
//-------------------------------------------------------------------------------------------------------------------------------------------------

INTERFACE::INTERFACE(bool fw, char* serial, bool* success)
{
	/* serial port is copied to serial*/
	strcpy(this->serial,serial);

	//O_NOCTTY flag tells UNIX that this program doesn't want to be the "controlling terminal" for that port.
	//O_NDELAY flag tells UNIX that this program doesn't care what state the DCD signal line is in - whether the other end of the port is up and running. If you do not specify this flag, your process will be put to sleep until the DCD signal line is the space voltage.

	serialfd= open (serial, O_RDWR | O_NOCTTY | O_NDELAY);

	/* exits program if port cant be opened */
	if (serialfd==-1)
	{
		cout<<"ERROR: Unable to open port "<<serial<<" for filter wheel, check permissions to use it\n";
		//exit(-1);
	}
	else
	{
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
		new_serial_conf.c_lflag = 0; //ICANON;

		/* flush input */
		tcflush(serialfd, TCIFLUSH);
		tcsetattr(serialfd, TCSANOW, &new_serial_conf);

		*success = true;

		if(*success==false){
			cout << "**************************Not successfull in reading from buffer for Filter wheel" << endl;
			close_serialfd();
		}
	}

}
#endif
//-------------------------------------------------------------------------------------------------------------------------------------------------

char* INTERFACE::getSerial()
{
	return serial;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void INTERFACE::close_serialfd()
{
	close(serialfd);
	delete this;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void INTERFACE::print()
{
	cout<<"\nPort:"<<serial<<", Serialfd:"<<serialfd<<endl;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

char* INTERFACE::send_command(char* c, int rb)  
{

#ifndef LASERBOX
	//for safety, exit if by any chance this command is called by the tube serial object
	if(!strcasecmp(serial,"/dev/ttyS0"))
		exit(-1);
#endif

	/* while checking if controller busy check=1, else check=0 while sending command */
	int check=1,busy=1,count;
	char buffer[255]="", command[200]="st ";
	char* p=buffer;

	/* this loop executes once to check if controller is busy & then to send command */
	for(int i=0;i<2;i++)
	{

		/* in not check mode: 'c' passed as argument is copied to 'command' to be sent */
		if(check==0)
		{
			/* if c is also 'st', then exit as check mode is already over */
			if(strcmp(c,"st ")==0) return p;

			strcpy(command,c);

#ifdef VERBOSE_MOTOR
			cout<<"Sending command: "<<command<<" with rb: "<<rb<<" to port "<<serial<<endl;
#endif
		}


		/*busy is always 1.
		 for check=1: keeps executing till st=0 and then breaks out of this while loop to send command
		 for check=0: sends the command and breaks out of loop by returning something
		 */
		count=1;
		while(busy)
		{
			/* keeps sending command till no error */
			//while (write (serialfd,command,strlen(command))==-1)
			//	cout<<"error sending the command,"<<command<<" to port"<<serial<<" trying again"<<endl;

			//so that the error doesnt print a million times
			int writeValue=-1, attempt=0;
			while(writeValue==-1)
			{
				writeValue = write (serialfd,command,strlen(command));
				attempt++;
				if(attempt==10)
					cout<<"error (attempt number10) sending the command "<<command<<" to port"<<serial<<" trying again"<<endl;
			}
			if(attempt>10)
				cout<<"succesful in sending the command,"<<command<<" to port"<<serial<<endl;



			/* for check=1: ignores
			  for check=0: if read back from controller not required, returns a useless variable to exit the function
			 */
			if(!rb&&!check) return p;

			/* For read back from controller, wait 50 microsec before reading buffer*/
			buffer[0]='\0';
			usleep(50000);
			int repeat = 0;
			/* keeps reading buffer till no error ,&
			 dont print error if command is 0 0 0 r which is just to wait for controller not to be busy*/
			while ((read(serialfd, buffer, 255)==-1) )
			{
				//error while reading 'pos' for 'm' movts and for 'ncal' movts..so don print this error for 'pos'
				if(strcmp(c,"pos "))
					repeat++;

				if(repeat==100)
				{
					cout<<"\nERROR:Error receiving data back, reading again: command sent is '"<<command<<"' to port '"<<serial<<"'\n";
					cout<<"The original command being:"<<c<<endl;
					cout<<"ERROR: The controllers might be busy with something else. \n";
					cout<<"Maybe you're trying to read from the buffer when a relative movement command is going on. \n";
					cout<<"Please check and resume.\n\n";
					//exit(-1);
				}			
			}

			/* returns buffer for read back commands */
			if(check==0)
			{
#ifdef VERBOSE_MOTOR
				cout<<"Received data:*"<<buffer<<"*\n";
#endif 
				return p;
			}

			/* in check mode */
			else
			{
				buffer[1]='\0';

				/* if 'st' reads back '0', signifies not busy, sets check=0 &
				  breaks out of busy loop to send command in next for loop
				 */
				if((strcmp(command,"st ")==0)&&(strcmp(buffer,"0")==0))
				{
					check=0;
					break;
				}

				/* 'st' did not read back '0',signifies busy, wait for 50 microsec
				  and repeat the busy loop to send 'st' command again
				 */
				usleep(50000);
			}

			//checking if the controllers are busy with something else, \
			//present command sent:st, cmd required to be sent shoudl not be st
			count++;
			if((strcmp(c,"pos ")) &&(count==10) && (!strcmp(command,"st ")))
			{
				cout<<"\n\nERROR: The controllers are busy with something else as the status does not show '0' each time.\n";
				cout<<"Maybe you're trying to read from the buffer when a relative movement command is going on.\nPlease check and resume.\n\n";
				cout<<"command:"<<c<<" rb:"<<rb<<endl;
				cout<<"check if joystick is being used or the controller is stuck\n";
				//exit(-1);
			}
		}
	}

	/* returning a useless variable to avoid warning */
	return p;
}

#ifndef LASERBOX
//-------------------------------------------------------------------------------------------------------------------------------------------------

char* INTERFACE::send_command_to_tube(char* c, int rb, int &value, int &value2)  
{
	//for safety,if a usb serial port calls this function, it exits
	//if(strstr (serial,"USB")!=NULL) exit(-1);


	char buffer[255]="", command[200]="", binaryNumber[9]="00000000";
	char* p = binaryNumber;
	int temp,temp2;

	strcpy(command,c);

	//#ifdef VERBOSE_MOTOR
	cout<<"Sending command:"<<command<<endl;
	//#endif

	if (write (serialfd,command,strlen(command))==-1)
		cout<<"error sending the command \n";

	if (rb)
	{
		buffer[0]='\0';
		int count = 0;
		while(!strcmp(buffer,""))
		{
			usleep(200000);

			if ( read (serialfd, buffer, 255)==-1)
				cout<<"error receiving data back \n";
			count++;
			if(count==5)//if((count==5)&&(!strcmp(c,"sr:12 ")))
			{
				cout<<"ERROR:The tube is probably switched off. Received no output from Tube for command:"<<c<<endl;
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
				cout<<"ERROR: the data received for command '"<<command<<"' is :"<<temp<<endl;
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
	cout<<"INTERFACE: Command:"<<c<<"  Received value:"<<p<<" : value:"<<value<<endl;
#endif 
	return p;
}


#else
//-------------------------------------------------------------------------------------------------------------------------------------------------

char* INTERFACE::send_command_to_fw(char* c, int rb)
{
	char buffer[255]="", command[200]="";
	char* p = buffer;

	strcpy(command,c);
	strcat(command,"\r");

#ifdef VERBOSE_MOTOR
	cout<<"Sending command: "<<command<<" with rb: "<<rb<<" to port "<<serial<<endl;
#endif

	if (write (serialfd,command,strlen(command))==-1)
		cout<<"error sending the command \n";

	if (rb)
	{
		buffer[0]='\0';
		while(!strcmp(buffer,""))
		{
			usleep(200000);
			if ( read (serialfd, buffer, 255)==-1)
				cout<<"error receiving data back \n";
		}

#ifdef VERBOSE_MOTOR
		cout<<"Received data:*"<<buffer<<"*\n";
#endif
	}
		return p;
}


#endif
//-------------------------------------------------------------------------------------------------------------------------------------------------
