#include "Gui.h"
#include "FluorWidget.h"
#include "FwheelWidget.h"
#include "GuiDefs.h"
#include "MotorWidget.h"
#include "OptionsWidget.h"
#include "PGaugeWidget.h"
#include "ReferencePointsWidget.h"
#include "SlitWidget.h"
#include "TubeWidget.h"

#include <QCloseEvent>

#include <iterator>

Gui::Gui(std::string hostname)
    : QMainWindow(0), hostname(hostname), refpoints(NULL), slits(NULL),
      pgauge(NULL), tube(NULL), optionsWidget(NULL) {
    setupUi(this);
    LayoutWindow();
    Initialization();
    Update();
}

Gui::~Gui() {}

void Gui::LayoutWindow() {
    layoutDone = false;

    std::ostringstream oss;
#ifdef XRAYBOX
    oss << "XRay Box Gui";
#elif LASERBOX
    oss << "Laser Box Gui";
#elif VACUUMBOX
    oss << "Vacuum Box Gui";
#else
    oss << "Motor Control Gui";
#endif
    oss << " - Motor Control Software (" << hostname << ")";
    std::string title = oss.str();
    FILE_LOG(logINFOBLUE) << title;
    setWindowTitle(title.c_str());

    // hide all widgets
    groupMotors->hide();
    groupReferencePoints->hide();
    groupFluorescence->hide();
    groupSlits->hide();
    groupFilterWheels->hide();
#if !defined(XRAYBOX) && !defined(VACUUMBOX)
    groupPressure->hide();
    widgetTube->hide();
    resize(WINDOW_WIDTH_NO_TUBE, WINDOW_HEIGHT_REFERENCE);
#elif XRAYBOX
    groupPressure->hide();
    resize(WINDOW_WIDTH_UNCHECK_TUBE, WINDOW_HEIGHT_REFERENCE);
#elif VACUUMBOX
    resize(WINDOW_WIDTH_UNCHECK_TUBE,
           WINDOW_HEIGHT_REFERENCE + WINDOW_HEIGHT_UNCHECK_PRESSURE);
#endif
    LoadMotorWidgets();
    LoadReferencePointsWidget();
    LoadFwheelWidgets();
    LoadOptionsWidget();

//#ifdef VACUUMBOX
// one can check this later if they want, takes up too much time
// groupPressure->setChecked(true);
//#endif
#if defined(XRAYBOX) || defined(VACUUMBOX)
    groupTube->setChecked(true);
#endif
    layoutDone = true;
}

void Gui::LoadMotorWidgets() {

    std::string result;
    while (result.empty()) {
        result = SendCommand(hostname, 1, "motorlist", "Gui::LoadMotorWidgets");
    }
    FILE_LOG(logDEBUG) << "motorlist:" << result;

    // parse motor names
    std::istringstream iss(result);
    std::vector<std::string> args =
        std::vector<std::string>(std::istream_iterator<std::string>(iss),
                                 std::istream_iterator<std::string>());

    for (size_t i = 0; i < args.size(); ++i) {
        std::string motorName = args[i];

        // create slit widget
        if (motorName.find("Slit") != std::string::npos) {
            if (slits == NULL) {
                groupSlits->show();
                groupSlits->setEnabled(true);
                lblSlits->hide();
                slits = new SlitWidget(this, hostname, statusbar);
                gridSlits->addWidget(slits, 0, 0);
            }
            continue;
        }

        // create fluorescence widget
        if (motorName.find("Fluorescence") != std::string::npos) {
            if (fluorWidgets.size() == 0) {
                groupFluorescence->show();
                groupFluorescence->setEnabled(true);
                lblFluors->hide();
            }
            fluorWidgets.push_back(
                new FluorWidget(this, motorName, hostname, statusbar));
            int currentIndex = fluorWidgets.size() - 1;
            gridFluorescence->addWidget(fluorWidgets[currentIndex],
                                        currentIndex, 0);
            continue;
        }

        // create motor widget
        if (motorWidgets.size() == 0) {
            groupMotors->show();
            groupMotors->setEnabled(true);
            lblMotors->hide();
        }
        motorWidgets.push_back(
            new MotorWidget(this, motorName, hostname, statusbar));
        int currentIndex = motorWidgets.size() - 1;
        gridMotors->addWidget(motorWidgets[currentIndex], currentIndex, 0);
    }

    int mainWindowHeight =
        height() + ((motorWidgets.size() == 0) ? 0 : WINDOW_HEIGHT_MOTOR_REF) +
        motorWidgets.size() * WINDOW_HEIGHT_MOTOR +
        ((fluorWidgets.size() == 0) ? 0 : WINDOW_HEIGHT_FLUOR_REF) +
        fluorWidgets.size() * WINDOW_HEIGHT_FLUOR +
        (slits == NULL ? 0 : WINDOW_HEIGHT_SLITS);
    resize(width(), mainWindowHeight);
}

void Gui::LoadReferencePointsWidget() {
    // get number of reference points
    std::string result =
        SendCommand(hostname, 1, "nref", "Gui::LoadReferencePointsWidget");
    if (result.empty()) {
        return;
    }
    try {
        // if no refernce points, return
        int nref = getInteger(result);
        if (nref == 0) {
            return;
        }
    } catch (const std::exception &e) {
        Message(WARNING, e.what(), "Gui::LoadReferencePointsWidget");
        return;
    }
    MotorWidget *x = NULL;
    MotorWidget *y = NULL;
    MotorWidget *z = NULL;
    for (unsigned int i = 0; i < motorWidgets.size(); ++i) {
        if (motorWidgets[i]->GetName() == "Detector_x") {
            x = motorWidgets[i];
            continue;
        }
        if (motorWidgets[i]->GetName() == "Detector_y") {
            y = motorWidgets[i];
            continue;
        }
        if (motorWidgets[i]->GetName() == "Detector_z") {
            z = motorWidgets[i];
            continue;
        }
    }
    if (x == NULL || y == NULL || z == NULL) {
        Message(
            WARNING,
            "No reference points added as Detector_x, Detector_y or Detector_z "
            "is missing",
            "Gui::LoadReferencePointsWidget");
        return;
    }
    groupReferencePoints->show();
    groupReferencePoints->setEnabled(true);
    lblReferencePoints->hide();
    refpoints = new ReferencePointsWidget(this, hostname, x, y, z, statusbar);
    gridReferencePoints->addWidget(refpoints, 0, 0);

    resize(width(), height() + WINDOW_HEIGHT_REF_POINTS);
}

void Gui::LoadFwheelWidgets() {
    // get number of filter wheels
    std::string result =
        SendCommand(hostname, 1, "nfw", "Gui::LoadFwheelWidgets");
    if (result.empty()) {
        return;
    }
    try {
        // if no filter wheels, return
        int nfwheel = getInteger(result);
        if (nfwheel == 0) {
            return;
        }
    } catch (const std::exception &e) {
        Message(WARNING, e.what(), "Gui::LoadFwheelWidgets");
        return;
    }

    // get list of filter wheel names
    result = SendCommand(hostname, 1, "fwlist", "Gui::LoadFwheelWidgets");
    if (result.empty()) {
        return;
    }
    FILE_LOG(logDEBUG) << "fwlist:" << result;

    // parse filter wheel names
    std::istringstream iss(result);
    while (iss.good()) {
        std::string name;
        iss >> name;
        if (name.empty()) {
            continue;
        }

        // create filter wheel widget
        if (fwheelWidgets.size() == 0) {
            groupFilterWheels->show();
            groupFilterWheels->setEnabled(true);
            lblFilterWheels->hide();
        }

        fwheelWidgets.push_back(
            new FwheelWidget(this, name, hostname, statusbar));
        int currentIndex = fwheelWidgets.size() - 1;
        gridFilterWheels->addWidget(fwheelWidgets[currentIndex], currentIndex,
                                    0);
    }

    int mainWindowHeight =
        height() +
        ((fwheelWidgets.size() == 0) ? 0 : WINDOW_HEIGHT_FWHEEL_REF) +
        fwheelWidgets.size() * WINDOW_HEIGHT_FWHEEL;
    resize(width(), mainWindowHeight);
}

void Gui::LoadOptionsWidget() {
    std::vector<std::string> motorlist;
    for (size_t i = 0; i < motorWidgets.size(); ++i) {
        motorlist.push_back(motorWidgets[i]->GetName());
    }
    std::vector<std::string> reflist;
    if (refpoints != NULL) {
        reflist = refpoints->GetList();
    }
    std::vector<std::string> fluorlist;
    for (size_t i = 0; i < fluorWidgets.size(); ++i) {
        fluorlist.push_back(fluorWidgets[i]->GetName());
        motorlist.push_back(fluorWidgets[i]->GetName());
    }
    if (slits != NULL) {
        motorlist.push_back("Slit_x1");
        motorlist.push_back("Slit_x2");
    }
    std::vector<std::string> fwheellist;
    for (size_t i = 0; i < fwheelWidgets.size(); ++i) {
        fwheellist.push_back(fwheelWidgets[i]->GetName());
    }
    optionsWidget = new OptionsWidget(this, hostname, motorlist, reflist,
                                      fluorlist, fwheellist);
}

void Gui::Initialization() {
    connect(pushUpdate, SIGNAL(clicked()), this, SLOT(Update()));
    connect(groupPressure, SIGNAL(toggled(bool)), this,
            SLOT(LoadPressureWidget()));
    connect(groupTube, SIGNAL(toggled(bool)), this, SLOT(LoadTubeWidget()));
    // options
    connect(pushOptions, SIGNAL(clicked()), this, SLOT(ShowOptions()));
    connect(optionsWidget, SIGNAL(ClosedSignal()), this, SLOT(OptionsClosed()));
}

void Gui::LoadPressureWidget(bool userClick) {
    try {
        statusbar->showMessage("Checking. Please be patient ...");
        statusbar->showMessage("Checking. Please be patient ...");
        std::string result =
            SendCommand(hostname, 1, "ispressure", "Gui::LoadPressureWidget");
        if (!result.empty()) {
            if (result == "on") {
                EnablePressureWidget(true);
            } else {
                if (userClick) {
                    Message(WARNING, "Vacuum pump is switched off.",
                            "Gui::LoadPressureWidget");
                }
                EnablePressureWidget(false);
            }
        }
    } catch (const PressureOffError &e) {
        Message(WARNING, e.what(), "Gui::LoadPressureWidget");
        EnablePressureWidget(false);
    }
    statusbar->clearMessage();
}

void Gui::EnablePressureWidget(bool enable) {
    bool wasEnabled = (pgauge != NULL);
    disconnect(groupPressure, SIGNAL(toggled(bool)), this,
               SLOT(LoadPressureWidget()));
    if (pgauge != NULL) {
        disconnect(pgauge, SIGNAL(SwitchedOffSignal(bool)), this,
                   SLOT(EnablePressureWidget(bool)));
    }
    if (!enable) {
        if (pgauge != NULL) {
            gridPressure->removeWidget(pgauge);
            delete pgauge;
            pgauge = NULL;
            lblPressure->show();
        }
        groupPressure->setChecked(false);

    } else {
        if (pgauge == NULL) {
            lblPressure->hide();
            pgauge = new PGaugeWidget(this, hostname);
            gridPressure->addWidget(pgauge, 0, 0);
        }
        pgauge->Update();
        groupPressure->setChecked(true);
        if (!wasEnabled) {
            resize(width(), height() + WINDOW_HEIGHT_PRESSURE);
        }
    }
    connect(groupPressure, SIGNAL(toggled(bool)), this,
            SLOT(LoadPressureWidget()));
    if (pgauge != NULL) {
        connect(pgauge, SIGNAL(SwitchedOffSignal(bool)), this,
                SLOT(EnablePressureWidget(bool)));
    }
    optionsWidget->EnablePressure(enable);
}

void Gui::LoadTubeWidget(bool userClick) {
    try {
        statusbar->showMessage("Checking. Please be patient ...");
        statusbar->showMessage("Checking. Please be patient ...");
        std::string result =
            SendCommand(hostname, 1, "istube", "Gui::LoadTubeWidget");
        if (!result.empty()) {
            if (result == "on") {
                EnableTubeWidget(true);
            } else {
                if (userClick) {
                    if (result == "standby") {
                        Message(WARNING, "Xray Tube is on Stand-by.",
                                "Gui::LoadTubeWidget");
                    } else {
                        Message(WARNING, "Xray Tube is switched off.",
                                "Gui::LoadTubeWidget");
                    }
                }
                EnableTubeWidget(false);
            }
        }
    } catch (const TubeOffError &e) {
        Message(WARNING, e.what(), "Gui::LoadTubeWidget");
        EnableTubeWidget(false);
    }
    statusbar->clearMessage();
}

void Gui::EnableTubeWidget(bool enable) {
    disconnect(groupTube, SIGNAL(toggled(bool)), this, SLOT(LoadTubeWidget()));
    if (tube != NULL) {
        disconnect(tube, SIGNAL(SwitchedOffSignal(bool)), this,
                   SLOT(EnableTubeWidget(bool)));
    }
    if (!enable) {
        if (tube != NULL) {
            gridTube->removeWidget(tube);
            delete tube;
            tube = NULL;
            lblTube->show();
        }
        groupTube->setChecked(false);
    }
    // enable tube
    else {
        if (tube == NULL) {
            lblTube->hide();
            tube = new TubeWidget(this, hostname, statusbar);
            gridTube->addWidget(tube, 0, 0);
        }
        tube->Update();
        groupTube->setChecked(true);
        // resize width (maybe height)
        if (height() < WINDOW_HEIGHT_TUBE) {
            resize(WINDOW_WIDTH_TUBE, WINDOW_HEIGHT_TUBE);
        } else {
            resize(WINDOW_WIDTH_TUBE, height());
        }
    }
    connect(groupTube, SIGNAL(toggled(bool)), this, SLOT(LoadTubeWidget()));
    if (tube != NULL) {
        connect(tube, SIGNAL(SwitchedOffSignal(bool)), this,
                SLOT(EnableTubeWidget(bool)));
    }
    optionsWidget->EnableTube(enable);
}

void Gui::ShowOptions() {
    optionsWidget->show();
    optionsWidget->Update();
}

void Gui::OptionsClosed() {
    optionsWidget->hide();
    for (size_t i = 0; i < fluorWidgets.size(); ++i) {
        if (fluorWidgets[i]->GetName() != "Fluorescence_wheel") {
            fluorWidgets[i]->UpdateHolderList();
        }
    }
    Update();
}

void Gui::Update() {
    if (!layoutDone) {
        return;
    }
    statusbar->showMessage(
        "Updating ..."); // gets replaced by temp explanations
                         // of tool tip texts (not permanent)

    // update motors
    for (unsigned int i = 0; i < motorWidgets.size(); ++i) {
        motorWidgets[i]->Update();
        statusbar->showMessage("Updating ...");
    }
    // udpate reference points
    if (refpoints != NULL) {
        refpoints->Update();
        statusbar->showMessage("Updating ...");
    }
    // udpate slits
    if (slits != NULL) {
        slits->Update();
        statusbar->showMessage("Updating ...");
    }
    // update fluorescence
    for (unsigned int i = 0; i < fluorWidgets.size(); ++i) {
        fluorWidgets[i]->Update();
        statusbar->showMessage("Updating ...");
    }
    // update fwheels
    for (unsigned int i = 0; i < fwheelWidgets.size(); ++i) {
        fwheelWidgets[i]->Update();
        statusbar->showMessage("Updating ...");
    }
    // udpate pressure gauge (only if enabled)
    if (groupPressure->isChecked()) {
        LoadPressureWidget(false);
        statusbar->showMessage("Updating ...");
    }
    // udpate pressure gauge (only if enabled)
    if (groupTube->isChecked()) {
        LoadTubeWidget(false);
        statusbar->showMessage("Updating ...");
    }

    statusbar->showMessage("Update completed", 2 * 1000);
}

// void Gui::Stop() {
//     pushStop->setChecked(true);
//     FILE_LOG(logINFO) << "Stopping all motors";
//     std::string result = SendCommand(hostname, 1, "stopall", "Gui::Stop");
//     pushStop->setChecked(false);
// 	Update();
// }

void Gui::closeEvent(QCloseEvent *event) {
    if (FAIL == Message(QUESTION, "Are you sure you want to quit?",
                        "Gui::closeEvent", "Quit Motor Control GUI")) {
        event->ignore();
    } else {
        SendCommand(hostname, 1, "unlock", "Gui::closeEvent");
        event->accept();
    }
}
