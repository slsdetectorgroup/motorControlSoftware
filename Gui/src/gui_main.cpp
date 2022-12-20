#include "Gui.h"
#include "GuiDefs.h"

#include <QApplication>

int main(int argc, char **argv) {
    // find server hostname
    std::string serverHostname = DEFAULT_SERVER_HOSTNAME;
    if (argc > 2) {
        for (int i = 1; i < argc; ++i) {
            if (strncmp(argv[i], "-server", 8) == 0) {
                if (i + 1 == argc) {
                    throw RuntimeError("Cannot scan server hostname option");
                }
                serverHostname.assign(argv[i + 1]);
                break;
            }
        }
    }

    QApplication app(argc, argv);

    // app.setWindowIcon(QIcon(":/icons/images/motorIcon.png"));
    try {
        Gui gui(serverHostname);
        gui.show();
        app.exec();
    } catch (const std::exception &e) {
        Message(CRITICAL, std::string(e.what()) + "\nExiting Gui :'( ", "main");
    }
    return 0;
}
