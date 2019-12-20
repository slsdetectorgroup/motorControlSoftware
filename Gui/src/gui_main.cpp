#include "GuiDefs.h"
#include "Gui.h"

#include <QApplication>
#include <QPlastiqueStyle>

int main(int argc, char **argv) {
#ifdef XRAYBOX
	FILE_LOG(logINFOBLUE) << "XRay Box Gui";
#elif LASERBOX
	FILE_LOG(logINFOBLUE) << "Laser Box Gui";
#else
	FILE_LOG(logINFOBLUE) << "Vacuum Box Gui";
#endif

    // find server hostname
    std::string serverHostname = DEFAULT_SERVER_HOSTNAME;
    if (argc > 2) {
        for (int i = 1; i < argc; ++i) {
            if (argv[i] == "-server") {
                if (i + 1 == argc) {
                    throw RuntimeError("Cannot scan server hostname option");    
                }
                serverHostname.assign(argv[i + 1]);
            break;
            }
        }
    }

    QApplication app(argc, argv);
    app.setStyle(new QPlastiqueStyle);
    //app.setWindowIcon(QIcon(":/icons/images/motorIcon.png"));
    try {
        Gui gui(serverHostname);
        gui.show();
        app.exec();
    } catch (const std::exception &e) {
        Message(CRITICAL, std::string(e.what()) + "\nExiting Gui :'( ", "main");
    }
    return 0;
}
