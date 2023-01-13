/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include "Initialize.h"
#include "Interface.h"
#include "sls/ServerSocket.h"
#include "sls/DataSocket.h"
#include "sls/string_utils.h"
#include "commonDefs.h"

#include <cstring>
#include <iterator>
#include <signal.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>

#define MIN_ARGUMENTS (6)

int main(int argc, char *[]) {
#ifdef VIRTUAL
    LOG(logINFOBLUE) << "Virtual Server";
#endif
#ifdef XRAYBOX
    LOG(logINFOBLUE) << "XRay Box Server";
#elif LASERBOX
    LOG(logINFOBLUE) << "Laser Box Server";
#elif VACUUMBOX
    LOG(logINFOBLUE) << "Vacuum Box Server";
#elif GENERIC
    LOG(logINFOBLUE) << "Motor Control Server";
#endif

    // if socket crash, ignores SISPIPE, prevents global signal handler
    // subsequent read/write to socket gives error - must handle locally
    signal(SIGPIPE, SIG_IGN);

    int portno = PORT_NO;
    if (argc == 1) {
        LOG(logINFO) << "Control Server";
    } else {
        LOG(logINFO) << "Stop Server";
        portno = PORT_NO + 1;
    }

    sls::ServerSocket server(portno);
    Initialize init = Initialize();
    LOG(logINFO) << "Ready for commands...";

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
        try {
            auto socket = server.accept();

            try {

            // get number of arguments
            int nArg = 0;
            socket.Receive(nArg);

            // get all arguments
            char args[TCP_PACKET_LENGTH];
            socket.Receive(args);
            std::cout << std::endl;
            LOG(logINFO) << "Server Received: " << args;


            char mess[TCP_PACKET_LENGTH];
            memset(mess, 0, sizeof(mess));

            if (nArg >= MIN_ARGUMENTS) {
                // extract time
                std::string timestamp(args + (strlen(args) - TIME_BUFFER_LENGTH),
                                    TIME_BUFFER_LENGTH);
                memset(args + strlen(args) - TIME_BUFFER_LENGTH, 0,
                    TIME_BUFFER_LENGTH);
                --nArg;

                // scan remaining arguments
                std::istringstream iss(args);
                std::vector<std::string> command = std::vector<std::string>(
                    std::istream_iterator<std::string>(iss),
                    std::istream_iterator<std::string>());
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
                LOG(logDEBUG) << "clienttype:[" << clientType << "], command:["
                            << commandName << "], pid:[" << pid << "], pcName:["
                            << pcName << "], userName:[" << userName
                            << "], timestamp:[" << timestamp << ']';

                // unlock command
                if (commandName == "unlock") {
                    sprintf(mess,
                            "You have successfully unlocked %s from the server.",
                            lockedUserName.c_str());
                    locked = false;
                    proceed = false;
                    lastCmdUnlock = true;
                }

                // unlocked server (lock it by default and save username, pcname,
                // pid etc)
                else if (!locked) {
                    lockedUserName = userName;
                    lockedPcName = pcName;
                    lockedPid = pid;
                    lockedTimestamp = timestamp;
                    locked = true;
                    lastCmdUnlock = false;
                    proceed = true;
                    // if the last command was unlock and its a gui command,
                    // the first command should be "list"
                    if (lastCmdUnlock && clientType == "gui" &&
                        commandName != "list") {
                        proceed = false;
                        ret = FAIL;
                        sprintf(
                            mess,
                            "%s.\n\nYour previous command was unsuccessful. Please "
                            "update your gui!",
                            ANOTHER_USER_ERROR_PHRASE);
                        LOG(logERROR) << mess;
                    }
                }

                // locked server (validate user)
                else {
                    proceed = true;
                    if (userName != lockedUserName || pcName != lockedPcName) {
                        proceed = false;
                        ret = FAIL;
                        sprintf(mess,
                                "The "
    #ifdef XRAYBOX
                                "x-ray"
    #elif LASERBOX
                                "laser"
    #elif VACUUMBOX
                                "vacuum"
    #else
                                "motorControlServer"
    #endif
                                " %s \n\nUser Name\t%s: %s\nPC Name\t\t: %s\nLast "
                                "Command "
                                "at\t: %s\n\n"
                                "Please check with this person before you use "
                                "\"unlock\".\n",
                                BOX_IN_USE_ERROR_PHRASE,
                                (clientType == "gui" ? "\t" : ""),
                                lockedUserName.c_str(), lockedPcName.c_str(),
                                lockedTimestamp.c_str());
                        LOG(logERROR) << mess;
                    }
                }
                if (proceed) {
                    // close server
                    if (commandName == "close") {
                        ret = GOODBYE;
                        LOG(logINFO) << "Closing Server!";
                        strcpy(mess, "Closing server");
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
                sprintf(mess,
                        "Invalid number of arguments %d. Require atleast %d\n.",
                        nArg, MIN_ARGUMENTS);
                LOG(logERROR) << mess;
            }


            socket.Send(ret);

            LOG(logINFO) << "Server Sent: " << mess;
            socket.Send(mess);


            } catch(const RuntimeError &e) {
                //"Error reading from socket. Possible socket crash.";
                // We had an error needs to be sent to client
                char mess[MAX_STR_LENGTH]{};
                sls::strcpy_safe(mess, e.what());
                socket.Send(FAIL);
                socket.Send(mess); 
                continue;//disconnect???
            }
        } catch (const RuntimeError &e) {
            LOG(logERROR) << "Accept failed";
        }

    }

    LOG(logINFO) << "Exiting Server!";

    return 0;
}
