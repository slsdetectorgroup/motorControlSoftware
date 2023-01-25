#include "Client.h"
#include "sls/ClientSocket.h"
#include "commonDefs.h"

#include <cstring>
#include <iterator>
#include <sstream>
#include <vector>
#include <unistd.h>

Client::Client(std::string hostname) : hostname(hostname) {}

std::string Client::SendCommand(int nCommand, std::string command) {
    uint16_t portno = PORT_NO;
    if (command.find("stop") != std::string::npos) {
        portno = PORT_NO + 1;
    }
    if (command.find("gui ") == std::string::npos) {
        std::cout << ":" << portno << "]\n";
    }

    // connect
    sls::ClientSocket client( hostname, portno);

    // copy arguments
    char args[TCP_PACKET_LENGTH];
    memset(args, 0, sizeof(args));
    // ensure command doesnt exceed size
    size_t sz = command.length();
    if (sz >= TCP_PACKET_LENGTH) {
        sz = TCP_PACKET_LENGTH - 1;
    }
    strncpy(args, command.c_str(), sz);

    // add additional parameters
    nCommand = AddOtherParameters(nCommand, args);
    LOG(logDEBUG) << "Sending Command: " << nCommand << " [" << args << ']';

    // send command
    client.Send(nCommand);
    client.Send(args);

    // get results
    int ret = OK;
    client.Receive(ret);
    char message[TCP_PACKET_LENGTH];
    memset(message, 0, sizeof(message));
    client.Receive(message);

    // throw exceptions
    if (ret == FAIL) {
        if (strstr(message, TUBE_STANDBY_ERROR_PHRASE) != NULL) {
            throw TubeStandbyError(message);
        } else if (strstr(message, TUBE_OFF_ERROR_PHRASE) != NULL) {
            throw TubeOffError(message);
        } else if (strstr(message, PRESSUR_OFF_ERROR_PHRASE) != NULL) {
            throw PressureOffError(message);
        } else if (strstr(message, BOX_IN_USE_ERROR_PHRASE) != NULL) {
            throw BoxInUseError(message);
        } else if (strstr(message, ANOTHER_USER_ERROR_PHRASE) != NULL) {
            throw AnotherUserError(message);
        }
        throw std::runtime_error(message);
    }
    return std::string(message);
}

int Client::AddOtherParameters(int nArg, char args[]) {
    strcat(args, " ");

    strcat(args, useSystemCommand("whoami").c_str());
    strcat(args, " ");
    ++nArg;
    strcat(args, useSystemCommand("uname -n").c_str());
    strcat(args, " ");
    ++nArg;
    // get pid
    {
        std::stringstream oss;
        oss << getpid();
        std::string pid = oss.str();
        strcat(args, pid.c_str());
        strcat(args, " ");
        ++nArg;
    }
    strcat(args, useSystemCommand("date").c_str());
    ++nArg;
    return nArg;
}

std::string Client::useSystemCommand(std::string command) {
    FILE *sysFile = popen(command.c_str(), "r");
    char output[COMMAND_BUFFER_LENGTH];
    memset(output, 0, sizeof(output));
    fgets(output, sizeof(output), sysFile);
    pclose(sysFile);

    if (command == "date") {
        std::string dateresult(output, TIME_BUFFER_LENGTH);
        LOG(logDEBUG) << "Date:[" << dateresult << ']';
        return dateresult;
    }

    std::istringstream iss(output);
    std::vector<std::string> result =
        std::vector<std::string>(std::istream_iterator<std::string>(iss),
                                 std::istream_iterator<std::string>());
    return result[0];
}