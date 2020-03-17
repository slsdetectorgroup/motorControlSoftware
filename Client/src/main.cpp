#include "Client.h"
#include "commonDefs.h"

#include <string>
#include <cstring>

int main(int argc, char* argv[]) {
#ifdef XRAYBOX
	  std::cout << "XRay Box Client";
#elif LASERBOX
	  std::cout << "Laser Box Client";
#else
	  std::cout << "Vacuum Box Client";
#endif

    char args[TCP_PACKET_LENGTH];
    memset(args, 0, sizeof(args));
    std::string serverHostname(DEFAULT_SERVER_HOSTNAME);

	// scan arguments for server
    strcat(args, "client ");
    int nArg = 1;
    for (int i = 1; i < argc; ++i) {
        if (strncmp(argv[i], "-server", 8) == 0) {
            if (i + 1 == argc) {
                throw RuntimeError("Cannot scan server hostname option");    
            }
            serverHostname = argv[++i];
        }else{
            strcat(args, argv[i]);
            strcat(args, " ");
            ++nArg;
        }  
    }
    std::cout << ' [' << serverHostname;

    Client client = Client(serverHostname);
    try {
        std::string result = client.SendCommand(nArg, args);
        std::cout << result << std::endl;
    } catch (const TubeStandbyError &e) {
        ;
    } catch (const TubeOffError &e) {
        ;
    } catch (const PressureOffError &e) {
        ;
    } catch (const BoxInUseError &e) {
        ;
    } catch (const AnotherUserError &e) {
        ;
    } catch (const std::exception &e) {
        FILE_LOG(logERROR) << e.what();
    }

    return 0;
}


