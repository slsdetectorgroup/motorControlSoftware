#include "Client.h"
#include "commonDefs.h"

#include <string>
#include <sstream>
#include <vector>
#include <cstring>
#include <iterator>

std::string useSystemCommand(std::string command) {
    FILE* sysFile = popen(command.c_str(), "r");
	  char output[COMMAND_BUFFER_LENGTH];
	  memset(output, 0, sizeof(output));
	  fgets(output, sizeof(output), sysFile);
	  pclose(sysFile);

    std::istringstream iss(output);
		std::vector<std::string> result = std::vector<std::string>(std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>());

    if (command == "date") {
        std::string result(output, TIME_BUFFER_LENGTH);
        FILE_LOG(logDEBUG) << "Date:[" << result << ']';
        return result;
    }
    return result[0];
}


int main(int argc, char* argv[]) {
#ifdef XRAYBOX
	  std::cout << "XRay Box Client" << std::endl;
#elif LASERBOX
	  std::cout << "Laser Box Client" << std::endl;
#else
	  std::cout << "Vacuum Box Client" << std::endl;
#endif

    char args[TCP_PACKET_LENGTH];
    memset(args, 0, sizeof(args));
    std::string serverHostname(DEFAULT_SERVER_HOSTNAME);

	// scan arguments
    strcat(args, "client ");
    int nArg = 1;
    for (int i = 1; i < argc; ++i) {
        if (argv[i] == "-server") {
            if (i + 1 == argc) {
                throw RuntimeError("Cannot scan server hostname option");    
            }
            serverHostname.assign(argv[i + 1]);
            ++i;
            continue;
        }
        strcat(args, argv[i]);
        strcat(args, " ");
        ++nArg;
    }

    // add other parameters
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


    std::cout << "serverHostname: [" << serverHostname << ']' << std::endl;
    FILE_LOG(logDEBUG) << "Sending Command: " << nArg << " [" << args << ']';

    Client client = Client();
    client.Connect(serverHostname);
    try {
        std::string result = client.SendCommand(nArg, args);
        std::cout << result << std::endl;
    } catch (const std::exception &e) {
        FILE_LOG(logERROR) << e.what();
    }
    client.Disconnect();

    return 0;
}


