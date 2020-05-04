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

#define MIN_ARGUMENTS	(6)



int main(int argc, char *argv[]) {
#ifdef XRAYBOX
	FILE_LOG(logINFOBLUE) << "XRay Box Server";
#elif LASERBOX
	FILE_LOG(logINFOBLUE) << "Laser Box Server";
#elif VACUUMBOX
	FILE_LOG(logINFOBLUE) << "Vacuum Box Server";
#else
	FILE_LOG(logINFOBLUE) << "Motor Control Server";
#endif
#ifdef VIRTUAL
	FILE_LOG(logINFOBLUE) << "Virtual Server";
#endif
    // if socket crash, ignores SISPIPE, prevents global signal handler
    // subsequent read/write to socket gives error - must handle locally
    signal(SIGPIPE, SIG_IGN);

	int portno = PORT_NO;
	if (argc == 1) {
		FILE_LOG(logINFO) << "Control Server";
	} else {
		FILE_LOG(logINFO) << "Stop Server";
		portno = PORT_NO + 1;
	}

	MySocketTCP *sock = new MySocketTCP(portno);
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
		ret = OK;
		sock->Connect();

		// get number of arguments
		int nArg = 0;
		if (sock->ReceiveDataOnly(&nArg, sizeof(nArg)) < 0 ) {
			FILE_LOG(logERROR) << "Error reading from socket. Possible socket crash.";
			sock->Disconnect();
			continue;
		}

		// get all arguments
		char args[TCP_PACKET_LENGTH];
		memset(args, 0, sizeof(args));
		if (sock->ReceiveDataOnly(args, sizeof(args)) < 0 ) {
			FILE_LOG(logERROR) << "Error reading from socket. Possible socket crash.";
			sock->Disconnect();
			continue;		
		}
		std::cout << std::endl;
		FILE_LOG(logINFO) << "Server Received: " << args;

		char mess[TCP_PACKET_LENGTH];
		memset(mess, 0, sizeof(mess));


		if (nArg >= MIN_ARGUMENTS) {
			// extract time
			std::string timestamp(args + (strlen(args) - TIME_BUFFER_LENGTH), TIME_BUFFER_LENGTH);
			memset(args + strlen(args) - TIME_BUFFER_LENGTH, 0, TIME_BUFFER_LENGTH);
			--nArg;

			// scan remaining arguments
			std::istringstream iss(args);
			std::vector<std::string> command = std::vector<std::string>(std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>());
			std::string clientType = command[0];
			std::string commandName = command[1];

			std::string pid = command[command.size() - 1];
			command.pop_back();
			--nArg;
			std::string pcName = command[command.size() - 1];
			command.pop_back();
			--nArg;
			std::string userName = command[command.size() - 1];
			command.pop_back();
			--nArg;
			FILE_LOG(logDEBUG) << "clienttype:[" << clientType << "], command:[" << commandName << "], pid:[" << pid << "], pcName:[" << pcName << "], userName:[" << userName << "], timestamp:[" << timestamp << ']';

			// unlock command
			if (commandName == "unlock") {
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
				if (lastCmdUnlock && clientType == "gui" && commandName != "list") {
					proceed = false;
					ret = FAIL;
					sprintf(mess,"%s.\n\nYour previous command was unsuccessful. Please update your gui!",
					ANOTHER_USER_ERROR_PHRASE);
					FILE_LOG(logERROR) << mess;
				}
			}

			// locked server (validate user)
			else {
				proceed = true;
				if (userName != lockedUserName || pcName != lockedPcName) {
					proceed = false;
					ret = FAIL;
					sprintf(mess,"The "
#ifdef XRAYBOX
							"x-ray"
#elif LASERBOX
							"laser"
#elif VACUUMBOX
							"vacuum"
#else
							"motorControlServer"
#endif
							" %s \n\nUser Name\t%s: %s\nPC Name\t\t: %s\nLast Command at\t: %s\n\n"
							"Please check with this person before you use \"unlock\".\n", 
							BOX_IN_USE_ERROR_PHRASE,
							(clientType == "gui" ? "\t" : ""), 
							lockedUserName.c_str(), lockedPcName.c_str(), lockedTimestamp.c_str());	
					FILE_LOG(logERROR) << mess;		
				}
			}
			if (proceed) {
				// close server
				if (commandName == "close") {
					ret = GOODBYE;
					FILE_LOG(logINFO) << "Closing Server!";
					strcpy(mess,"Closing server");
				} else {
					command.erase(command.begin());
					try {
						std::string result = init.executeCommand(command);
						strcpy(mess, result.c_str());
					} catch (const std::exception &e) {
						ret = FAIL;
						strcpy(mess, e.what());
					}
					// update timestamp
					lockedTimestamp = timestamp;
				}
			}
		} 
		
		// invalid number of arguments to parse command
		else {
			ret = FAIL;
			sprintf(mess, "Invalid number of arguments %d. Require atleast %d\n.", nArg, MIN_ARGUMENTS);
			FILE_LOG(logERROR) << mess;
		}

		if (sock->SendDataOnly(&ret, sizeof(ret)) < 0) {
			sock->Disconnect();
			continue;
		}
		FILE_LOG(logINFO) << "Server Sent: " << mess;
		sock->SendDataOnly(mess, sizeof(mess));
		sock->Disconnect();
	}

	delete sock;
	FILE_LOG(logINFO) << "Exiting Server!";

	return 0;
}

