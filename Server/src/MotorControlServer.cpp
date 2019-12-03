/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include "MySocketTCP.h"
#include "INTERFACE.h"
#include "INITIALIZE.h"
#include "commonDefs.h"
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <cstring>
#include <stdio.h>
#include <signal.h>
using namespace std;


int main(int argc, char *argv[])
{
cout<<"test"<<endl;
#ifdef XRAYBOX
	cout << " This is XRay Box Server" << endl;
#elif LASERBOX
	cout << " This is Laser Box Server" << endl;
#else
	cout << " This is Vacuum Box Server" << endl;
#endif

    // if socket crash, ignores SISPIPE, prevents global signal handler
    // subsequent read/write to socket gives error - must handle locally
    signal(SIGPIPE, SIG_IGN);


	int  retval=OK,ret=0,n;
	char mess[255],args[255],userName[255],pcName[255],pid[255],currTime[30];
	char lockeduserName[255],lockedpcName[255],lockedpid[255],lockedcurrTime[30];
	bool trueUser=true,lock=false,lastCmdUnlock=false;

	char *command[15];
	for(int i=0;i<15;i++)
		command[i]=new char[1000];


	// creates the socet from the server side
	MySocketTCP *sock=new MySocketTCP(PORT_NO);
	if ( sock->getErrorStatus())
		return -1;


	// reads config file to create motor objects and store controller values
#ifdef XRAYBOX
	INITIALIZE init=INITIALIZE("config.txt", "Positions.txt","configNew.txt","warmupTimestamps.txt");
#elif LASERBOX
	INITIALIZE init=INITIALIZE("config.txt", "Positions.txt","configNew.txt");
#else
	INITIALIZE init=INITIALIZE("config.txt", "Positions.txt","configNew.txt","warmupTimestamps.txt");
#endif


	cout<<"\nReady for commands..."<<endl;

	/* waits for connection */
	while(retval!=GOODBYE) {

		//connect socket
		sock->Connect();
#ifdef VERY_VERBOSE
		cout<<"*** connected"<<endl;
#endif	
		// Receiving the number of arguments in command line
		sock->ReceiveDataOnly(&argc,sizeof(argc));
#ifdef VERY_VERBOSE
		cout<<"*** r "<<argc<<endl;
#endif
		// Receiving arguments in command line
		sock->ReceiveDataOnly(args, sizeof(args));
		//#ifdef VERY_VERBOSE
		cout<<"*** server received: "<<args<<"."<<endl;;
		//#endif


		//to copy the time from args(fixed)
		strncpy(currTime,args+(strlen(args)-30),29);
		currTime[29] = '\0';

		// place the commands in command[][]
		istringstream sstr(args);
		n=0;
		while(sstr.good())
		{
			if(n==(argc-4))
			{ //get the userDetails
				sstr>>userName;
				if(sstr.good())  sstr>>pcName;
				if(sstr.good())  sstr>>pid;
				argc-=4;
				break;
			}
			else
			{ //get the commands
				sstr>>args;
				strcpy(command[n],args);
				n++;
			}
		}

		//  debug
		//  for(int i=0;i<n;i++)
		//      cout<<i<<":"<<command[i]<<endl;
		//     cout<<"userName:"<<userName<<" pcName:"<<pcName<<" pid:"<<pid<<" time:"<<currTime<<"."<<endl;
		//     cout<<"\nlockedcurrTime:"<<lockedcurrTime<<"."<<endl;


		//if command is unlock
		if(!strcasecmp(command[1],"unlock"))
		{
			sprintf(mess,"You have successfully unlocked %s from the server.",lockeduserName);
			trueUser=false;
			lock=false;
			lastCmdUnlock=true;
		}
		//if not locked, then save userdetails and lock
		else if(!lock)
		{
			strcpy(lockeduserName,userName);
			strcpy(lockedpcName,pcName);
			strcpy(lockedpid,pid);
			strcpy(lockedcurrTime,currTime);
			lock=true;
			trueUser=true;
			//if the last command was unlock and its a gui command,
			//the first command should be "list" for new gui or "save" to close old gui
			if((lastCmdUnlock)&&(!strcmp(command[0],"gui"))
					&&(strcasecmp(command[1],"list"))
#ifndef LASERBOX
					&&(strcasecmp(command[1],"whichflist"))
#endif
					&&(strcasecmp(command[1],"save")))
			{
				trueUser=false;
				strcpy(mess,"ERROR:Another user had updated the server. So, your GUI is not updated.\nInitiating Update..");
			}

			lastCmdUnlock=false;
		}
		else
		{
			//if locked, always check if it is the right user
			if((strcmp(userName,lockeduserName))||(strcmp(pcName,lockedpcName)))
			{
				cout<<"userName:"<<userName<<":lockedUserNAme:"<<lockeduserName<<":pcName:"<<pcName<<":lockedpcName:"<<lockedpcName<<":"<<endl;
				char tabspace[5] ="";
				if(!strcmp(command[0],"gui"))
					strcpy(tabspace,"\t");
				sprintf(mess,"ERROR:The "
#ifdef XRAYBOX
						"x-ray"
#elif LASERBOX
						"laser"
#else
						"vacuum"
#endif
						" box is in use by \n\nUser Name\t%s: %s\nPC Name\t\t: %s\nLast Command at\t: ",tabspace,lockeduserName,lockedpcName);
				strcat(mess,lockedcurrTime);
				strcat(mess,"\n\nPlease check with this person before you use \"unlock\".\n");

				trueUser=false;
			}
			else
				trueUser=true;
		}

		//only execute commands if its a right user
		if(trueUser)
		{
			// if no commands given
			if (argc<2)
				strcpy(mess,"No move commands given in the command line");
			else
			{
				// if command is to close server, return 9 and break
				if(strcasecmp(command[1],"close")==0)
				{
					ret=9;
					strcpy(mess,"Closing server");
				}
				// call the executeCommand function
				else
					ret=init.executeCommand(argc-1,command+1,mess);
			}
			//save the last timestamp of true user
			strncpy(lockedcurrTime,currTime,29);
			lockedcurrTime[29] = '\0';

		}



		// sends ret back to client
		sock->SendDataOnly(&ret,sizeof(ret));
#ifdef VERY_VERBOSE
		cout<<"*** s "<<ret<<endl;
#endif

		// sends message received back to client
		sock->SendDataOnly(mess,sizeof(mess));
		//#ifdef VERY_VERBOSE
		cout<<"*** server sends: "<<mess<<endl;
		strcpy(mess,"");
		//#endif

		//disconnect socket
		sock->Disconnect();
#ifdef VERY_VERBOSE
		cout<<"*** disconnected"<<endl;
#endif

		if(ret==9)
			break;

	}

	delete sock;
	cout<<"Goodbye!"<<endl;

	return 0;
}

