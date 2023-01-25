/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include "Initialize.h"
#include "Interface.h"
#include "sls/ServerSocket.h"
#include "sls/DataSocket.h"
#include "commonDefs.h"

#include <cstring>
#include <iterator>
#include <signal.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>

#ifdef XRAYBOX
#define softwareType "X-Ray"
#elif LASERBOX
#define softwareType "Laser"
#elif VACUUMBOX
#define softwareType "Vacuum"
#else
#define softwareType "Motor Control Server"
#endif

struct lockAttributes {
    std::string pid;
    std::string pcName;
    std::string username;
    std::string timestamp;
    bool status {false};
    bool proceed {true};
    bool lastCmdUnlock{false};
};


int DecodeFunction(char* mess, int nArg, char* args, lockAttributes& lock, Initialize& init) {
    const int minArguments = 6;
    if (nArg < minArguments) {
        sprintf(mess,
                "Invalid number of arguments %d. Require atleast %d\n.",
                nArg, minArguments);
        LOG(logERROR) << mess;
        return FAIL;
    } 
    
    // extract arguments
    std::string timestamp(args + (strlen(args) - TIME_BUFFER_LENGTH),
                        TIME_BUFFER_LENGTH);
    memset(args + strlen(args) - TIME_BUFFER_LENGTH, 0,
        TIME_BUFFER_LENGTH);
    --nArg;
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
                lock.username.c_str());
        lock.status = false;
        lock.proceed = false;
        lock.lastCmdUnlock = true;
        return OK;
    }

    // already unlocked server (lock it by default and save username, pcname,
    // pid etc)
    if (!lock.status) {
        lock.username = userName;
        lock.pcName = pcName;
        lock.pid = pid;
        lock.timestamp = timestamp;
        lock.status = true;
        lock.lastCmdUnlock = false;
        lock.proceed = true;

        // confirm if the last command was unlock and its a gui command,
        // the first command should be "list"
        if (lock.lastCmdUnlock && clientType == "gui" &&
            commandName != "list") {
            lock.proceed = false;
            sprintf(
                mess,
                "%s.\n\nYour previous command was unsuccessful. Please "
                "update your gui!",
                ANOTHER_USER_ERROR_PHRASE);
            LOG(logERROR) << mess;
            return FAIL;
        }
    }

    // locked server (validate user)
    else {
        lock.proceed = true;
        if (userName != lock.username || pcName != lock.pcName) {
            lock.proceed = false;
            sprintf(mess,
                    "The %s %s \n\nUser Name\t%s: %s\nPC Name\t\t: %s\nLast "
                    "Command at\t: %s\n\nPlease check with this person before you use "
                    "\"unlock\".\n",
                    softwareType,
                    BOX_IN_USE_ERROR_PHRASE,
                    (clientType == "gui" ? "\t" : ""),
                    lock.username.c_str(), lock.pcName.c_str(),
                    lock.timestamp.c_str());
            LOG(logERROR) << mess;
            return FAIL;
        }
    }

    if (lock.proceed) {
        
        // close server
        if (commandName == "close") {
            LOG(logINFO) << "Closing Server!";
            strcpy(mess, "Closing server");
            return GOODBYE;
        } 

        // other commands
        command.erase(command.begin());
        try {
            std::string result = init.executeCommand(command);
            strcpy(mess, result.c_str());
        } catch (const std::exception &e) {
            strcpy(mess, e.what());
            lock.timestamp = timestamp;
            return FAIL;
        }
        // update timestamp
        lock.timestamp = timestamp;
    }
    return OK;
}



int main(int argc, char *[]) {
#ifdef VIRTUAL
    LOG(logINFOBLUE) << "Virtual Server";
#endif
    LOG(logINFOBLUE) << softwareType << " Box Server";


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
    lockAttributes lock;
    LOG(logINFO) << "Ready for commands...";

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

            // return success/error message
            char mess[TCP_PACKET_LENGTH];
            memset(mess, 0, sizeof(mess));
            ret = DecodeFunction(mess, nArg, args, lock, init);      
            socket.Send(ret);
            LOG(logINFO) << "Server Sent: " << mess;
            socket.Send(mess);


            } catch(const RuntimeError &e) {
                //"Error reading from socket. Possible socket crash.";
                // We had an error needs to be sent to client
                //TODO! Assert?
                char mess[TCP_PACKET_LENGTH]{};
                strncpy(mess, e.what(), TCP_PACKET_LENGTH-1);
                mess[TCP_PACKET_LENGTH-1] = '\0'; //Just to be safe
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
