/********************************************//**
 * @file main.cpp
 * @short Calls myClient to connect to server if remote, else executes command
 * @author Dhanya
 ***********************************************/

#ifdef REMOTE
#include "MotorControlClient.h"
#endif

#ifdef LOCAL
#include "INITIALIZE.h"
#endif

#include "commonDefs.h"
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <unistd.h>
#include <cstring>
using namespace std;

//using systemcommand to get pcname,username etc
char* useSystemCommand(char* systemCommand);

//read server.txt file if it exists to read the default server(eg.beamline server)
char* readFile(string const fName);  

//-------------------------------------------------------------------------------------------------------------------------------------------------
 /**the main function
   @param argc number of command line arguments
   @param argv array of command line arguments
   @returns 0 to OS
 */
int main(int argc, char* argv[])
{

#ifndef XRAYBOX
	cout << " This is XRay Box Client" << endl;
#elif LASERBOX
	cout << " This is Laser Box Client" << endl;
#else
	cout << " This is Vacuum Box Client" << endl;
#endif


#ifdef REMOTE
  char args[255], serverHostname[1000],userName[255], currTime[255], pcName[255];
  int serverOptions=0;
  bool serverOption=false;

  //read server.txt file if it exists to read the default server(eg.beamline server)
  strcpy(serverHostname,readFile("init.txt"));
  strcpy(serverHostname,readFile("init.txt")); 
  //cout<<"serverHostname:"<<serverHostname<<"."<<endl;

  //include first argument for server to differentiate between gui and socketclient
  strcpy(args,argv[0]); strcat(args," ");

  //loop through each command line argument
  for(int i=1;i<argc;i++)
    {
      //to skip the value of Server option
      if(serverOption)
	{
	  serverOption=false;
	  continue;
	}
       //check if a server options exist
      if(!strcasecmp(argv[i],"-server"))
	{
	  serverOptions++;
	  serverOption=true;  
	  strcpy(serverHostname,argv[i+1]);
	}
      //concatenates all the commands (if not server option) into a string args
      else
	{
	  strcat(args, argv[i]);
	  strcat(args, " ");
	}
    }

  //getting username,pcname,pid for each command to send to server
  strcpy(userName,useSystemCommand((char*)"whoami"));
  strcpy(pcName,useSystemCommand((char*)"uname -n"));
  strcpy(currTime,useSystemCommand((char*)"date"));
  sprintf(args,"%s %s %s %d %s ",args,userName,pcName,getpid(),currTime);
  argc+=4;
  /*DEBUGGING
  cout<<"hostname:"<<serverHostname<<" argc:"<<argc-serverOptions*2<<" args:"<<args<<"."<<endl;
  */
  cout<<"serverHostname:"<<serverHostname<<"."<<endl;
  //connect socket and send command
  MotorControlClient client = MotorControlClient();
  if(client.Connect(serverHostname))
    {
      cout<<"\n--- Rxd data:"<<client.SendCommand(argc-serverOptions*2,args);
      cout<<"\n---------------------------------------------------------------------------------\n";
      client.Disconnect();
    }
  else
    cout<<"\n ERROR: Could not connect to server\n\n";

#endif


#ifdef LOCAL
  char mess[255];

  // reads config file to create motor objects and store controller values
#ifdef XRAYBOX
  INITIALIZE init=INITIALIZE("config.txt", "Positions.txt","configNew.txt","warmupTimestamps.txt");
#elif LASERBOX
  INITIALIZE init=INITIALIZE("config.txt", "Positions.txt","configNew.txt");
#else
  INITIALIZE init=INITIALIZE("config.txt", "Positions.txt","configNew.txt","warmupTimestamps.txt");
#endif
  if (argc<2)
	strcpy(mess,"No move commands given in the command line");
  else
	init.executeCommand(argc-1,argv+1,mess);

  cout<<"\n\nMessage:\n"<<mess<<endl<<endl;

  //save positions of all motors in a Positions.txt file 
  init.savePositions("Positions.txt");

  //save to new config file
  init.saveToNewConfigFile("config.txt","configNew.txt");
  
#ifndef LASERBOX
  //write the warmuptimestamps file
  init.WriteWarmupTimestamps("warmupTimestamps.txt");
#endif

  cout<<"\n---------------------------------------------------------------------------------\n";

#endif

  return 0;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

char* useSystemCommand(char* systemCommand)
{
  const char *WHITESPACE=" \t\n\r";
  char *result;
  char output[255];

  //using sys cmds to get output or str
  FILE* sysFile = popen(systemCommand, "r");
  fgets(output, sizeof(output), sysFile);
  pclose(sysFile);
  
  if(!strcmp(systemCommand,"date"))
    {
      result = output + 0;
      result[29]=0;
    }
  else
    {
      //remove trailing spaces
      int spacesAtStart = strspn(output, WHITESPACE);//length of initial portion of str1 which matches str2 ,here 0
      result = output + spacesAtStart;
      int lengthOfNonSpace = strcspn(result, WHITESPACE);//position of where str1 does not match str2
      result[lengthOfNonSpace] = 0;
    }
  //  cout<<endl;
  return result;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

char* readFile(string const fName)
{
  char output[255],arg[255];
  char *result=output+0;
  ifstream inFile;
  string sLine;

  //default server hostname
  strcpy(output,Server_Hostname);

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
		sstr>>arg;
	      if(!strcasecmp(arg,"server"))
		{
		sstr>>output;
		//cout<<"From file, using server "<<output<<"."<<endl;
		}
	    }
	}
      inFile.close();
    }
  else
    { 
      //default server hostname
      ;// cerr<<"Using default server \n";
    }
  return result;
}
//-------------------------------------------------------------------------------------------------------------------------------------------------
