/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include "MySocketTCP.h"
#include "Interface.h"
#include "Initialize.h"
#include "commonDefs.h"

#include <stdlib.h>
#include <sstream>
#include <cstring>
#include <stdio.h>
#include <signal.h>
#include <iterator>

#define TIME_BUFFER_LENGTH	(30)


int main(int argc, char *argv[]) {
#ifdef XRAYBOX
	FILE_LOG(logINFOBLUE) << "XRay Box Server";
#elif LASERBOX
	FILE_LOG(logINFOBLUE) << "Laser Box Server";
#else
	FILE_LOG(logINFOBLUE) << "Vacuum Box Server";
#endif

    // if socket crash, ignores SISPIPE, prevents global signal handler
    // subsequent read/write to socket gives error - must handle locally
    signal(SIGPIPE, SIG_IGN);

	MySocketTCP *sock = new MySocketTCP(PORT_NO);
	if (sock->getErrorStatus()) {
		throw RuntimeError("Could not create server socket");
	}

	Initialize init = Initialize();
	FILE_LOG(logINFO) << "Ready for commands...";


	std::string lockedPid;
	std::string lockedPcName;
	std::string lockedUserName;
	std::string lockedTimestamp;
	bool locked = false;
	bool proceed = true;
	bool lastCmdUnlock = false;


	int ret = OK;
	while (ret != GOODBYE) {

		sock->Connect();

		// get number of arguments
		int nArg = 0;
		sock->ReceiveDataOnly(&nArg, sizeof(nArg));

		// get all arguments
		char args[TCP_PACKET_LENGTH];
		memset(args, 0, sizeof(args));
		sock->ReceiveDataOnly(args, sizeof(args));
		std::cout << std::endl;
		FILE_LOG(logINFO) << "Server Received: " << args;

		// extract time
		std::string timestamp(args + (strlen(args) - TIME_BUFFER_LENGTH), TIME_BUFFER_LENGTH - 1);
		memset(args + strlen(args) - TIME_BUFFER_LENGTH, 0, TIME_BUFFER_LENGTH);
		--nArg;
		FILE_LOG(logDEBUG) << "timestamp:[" << timestamp << ']';

		// scan arguments
		istringstream iss(args);
		std::vector<string> command = std::vector<std::string>(istream_iterator<std::string>(iss), istream_iterator<std::string>());

		// extract username, pcname, pid
		std::string pid = command[command.size() - 1];
		std::string pcName = command[command.size() - 2];
		std::string userName = command[command.size() - 3];
		command.erase(command.begin() + command.size() - 3, command.begin() + command.size() - 1);
		nArg -= 3;
		FILE_LOG(logDEBUG) << "pid:[" << pid << "], pcName" << pcName << "], userName" << userName << ']';

		// parse commands
		char mess[TCP_PACKET_LENGTH];
		memset(mess, 0, sizeof(mess));

		// unlock command
		if (command[1] == "unlock") {
			sprintf(mess,"You have successfully unlocked %s from the server.", lockedUserName.c_str());
			locked = false;
			proceed = false;
			lastCmdUnlock = true;
		} 
		
		// unlocked server (lock it by default and save username, pcname, pid etc)
		else if (!locked)  {
			lockedUserName = userName;
			lockedPcName = pcName;
			lockedPid = pid;
			lockedTimestamp = timestamp;
			locked = true;
			lastCmdUnlock = false;
			proceed = true;
			// if the last command was unlock and its a gui command,
			// the first command should be "list" 		
			if (lastCmdUnlock && command[0] == "gui" && command[1] != "list") {
				proceed = false;
				strcpy(mess,"ERROR: Another user had updated the server. So, your GUI is not updated.\nInitiating Update..");
			}
		}

		// locked server (validate user)
		else {
			proceed = true;
			if (userName != lockedUserName || pcName != lockedPcName) {
				sprintf(mess,"ERROR:The "
#ifdef XRAYBOX
						"x-ray"
#elif LASERBOX
						"laser"
#else
						"vacuum"
#endif
						" box is in use by \n\nUser Name\t%s: %s\nPC Name\t\t: %s\nLast Command at\t: %s\n\n"
						"Please check with this person before you use \"unlock\".\n", 
						(command[0] == "gui" ? "\t" : ""), 
						lockedUserName.c_str(), lockedPcName.c_str(), lockedTimestamp.c_str());	
				proceed = false;
			}
		}

		// proceed to execute commands
		if (proceed) {
			// no commands
			if (nArg <= 1) {
				strcpy(mess,"No move commands given in the command line");	
			} 
			// execute command
			else {
				if (command[1] == "close") {
					ret = GOODBYE;
					strcpy(mess,"Closing server");
				} else {
					command.erase(command.begin());
					ret = init.executeCommand(command, mess);
				}
			}
			// update timestamp
			lockedTimestamp = timestamp;
		}

		// send return value
		sock->SendDataOnly(&ret, sizeof(ret));

		// send message
		sock->SendDataOnly(mess, sizeof(mess));

		sock->Disconnect();
	}

	delete sock;
	FILE_LOG(logINFO) << "Exiting Server!";

	return 0;
}

