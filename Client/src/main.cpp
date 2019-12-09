/********************************************//**
 * @file main.cpp
 * @short Calls myClient to connect to server if remote, else executes command
 * @author Dhanya
 ***********************************************/

#include "Client.h"
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
string getServerHostname(string const fName);  

//-------------------------------------------------------------------------------------------------------------------------------------------------
 /**the main function
   @param argc number of command line arguments
   @param argv array of command line arguments
   @returns 0 to OS
 */
int main(int argc, char* argv[])
{

#ifdef XRAYBOX
	cout << " This is XRay Box Client" << endl;
#elif LASERBOX
	cout << " This is Laser Box Client" << endl;
#else
	cout << " This is Vacuum Box Client" << endl;
#endif


  char args[255], serverHostname[1000],userName[255], currTime[255], pcName[255];
  int serverOptions=0;
  bool serverOption=false;

  //read server.txt file if it exists to read the default server(eg.beamline server)
  strcpy(serverHostname,getServerHostname("init.txt").c_str());
  strcpy(serverHostname,getServerHostname("init.txt").c_str()); 
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
  Client client = Client();
  if(client.Connect(serverHostname))
    {
      cout<<"\n--- Rxd data:"<<client.SendCommand(argc-serverOptions*2,args);
      cout<<"\n---------------------------------------------------------------------------------\n";
      client.Disconnect();
    }
  else
    cout<<"\n ERROR: Could not connect to server\n\n";
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

string getServerHostname(string const fName)
{
  //default server hostname
  strcpy(output,Server_Hostname);

  ifstream inFile;
  inFile.open(fName.c_str(), ifstream::in);                             
  if(inFile.is_open()) {
    while(inFile.good()) {
      string sLine;
      getline(inFile,sLine);
      // delete lines after comments
      if (sLine.find('#') != string::npos) {
          sLine.erase(sLine.find('#'));
      }
      // scan arguments
      istringstream iss(sLine);
      vector<string> args = vector<string>(istream_iterator<string>(iss), istream_iterator<string>());
      // blank lines
      if (args.size() < 2 || args[0] != "server") {
        continue;
      }
      inFile.close();
      return args[1];
    }
  }
  return string(Server_Hostname);
}
//-------------------------------------------------------------------------------------------------------------------------------------------------
