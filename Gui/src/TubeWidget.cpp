#include "TubeWidget.h"
#include "GuiDefs.h"

#include <QCheckBox>
#include <QStatusBar>
#include <QTimer>

#include <sstream>

#define TUBE_ERROR_INITIAL_WARM_UP  (70)
#define TUBE_ERROR_WARM_UP  (106)
#define TUBE_ERROR_WARM_UP2  (109)

TubeWidget::TubeWidget(QWidget *parent, std::string hostname, QStatusBar* statusBar)
    :   QWidget(parent), hostname(hostname), statusBar(statusBar) {
	setupUi(this);
    timerActual = new QTimer(this);
    timerActual->setSingleShot(true);
    timerWarmup = new QTimer(this);
    timerWarmup->setSingleShot(true);
    LayoutWindow();
    Initialization();
}

TubeWidget::~TubeWidget() {}

void TubeWidget::LayoutWindow() {
    GetConnectedShutters();
    spinActualVoltage->setStyleSheet("color: darkBlue");
    spinActualCurrent->setStyleSheet("color: darkBlue");
}

void TubeWidget::Initialization() {
    connect(pushError, SIGNAL(clicked()), this, SLOT(DisplayTubeError()));
    connect(pushClear, SIGNAL(clicked()), this, SLOT(ClearError()));
    connect(pushHighVoltage, SIGNAL(toggled(bool)), this, SLOT(SetHighVoltage(bool)));
    for (unsigned int i = 0; i < chkShutters.size(); ++i) {
        connect(chkShutters[i], SIGNAL(toggled(bool)), this, SLOT(SetShutters()));
    }
    connect(spinVoltage, SIGNAL(valueChanged(int)), this, SLOT(SetVoltage(int)));
    connect(spinCurrent, SIGNAL(valueChanged(int)), this, SLOT(SetCurrent(int)));
    connect(timerActual, SIGNAL(timeout()), this, SLOT(UpdateActualValues()));
    connect(timerWarmup, SIGNAL(timeout()), this, SLOT(UpdateWarmupTiming()));
}

void TubeWidget::GetConnectedShutters() {
    std::string result = SendTubeCommand(hostname, 1, "connectedshutters ", "TubeWidget::GetConnectedShutters");
    if (result.empty()) {
        return;
    }
    std::istringstream iss(result);
    std::vector<std::string> list = std::vector<std::string>(std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>());
    if (list.size() < 4) {
        Message(WARNING, "Could not get connected shutters list", "TubeWidget::GetConnectedShutters");
        return;
    }
    chkShutters.resize(4);
    chkShutters[0] = chkShutter1;
    chkShutters[1] = chkShutter2;
    chkShutters[2] = chkShutter3;
    chkShutters[3] = chkShutter4;

    for (unsigned int i = 0; i < chkShutters.size(); ++i) {
        if (list[i] == "nc") {
            chkShutters[i]->setText("-");
            chkShutters[i]->setEnabled(false);
        }
    }
}

void TubeWidget::DisplayTubeError() {
    // get error code
    int errorCode = -1;
    std::string result = SendTubeCommand(hostname, 1, "geterr", "TubeWidget::DisplayTubeError");
    if (result.empty()) {
        return;
    }
    try {
        errorCode = getInteger(result);
    } catch (const std::exception& e) {
        Message(WARNING, e.what(), "TubeWidget::DisplayTubeError");
    }
    result = SendTubeCommand(hostname, 1, "getemess", "TubeWidget::DisplayTubeError");
    if (result.empty()) {
        return;
    }
    std::ostringstream oss;
    oss << errorCode << ": " << result;
    dispError->setText(oss.str().c_str());    
}

void TubeWidget::ClearError(bool wait) {
    FILE_LOG(logINFO) << "Clearing Error in Tube";
    std::string result = SendTubeCommand(hostname, 1, "clear", "TubeWidget::ClearError");
    statusBar->showMessage("Updating ...");
    // wait required when clearing 109 error (warm up necessary, when user declines)
    if (wait) {
        usleep (1 * 1000 * 1000);
    }
    DisplayTubeError();
}

void TubeWidget::ClearError() {
    ClearError(false);
}

void TubeWidget::GetHighVoltage() {
    disconnect(pushHighVoltage, SIGNAL(toggled(bool)), this, SLOT(SetHighVoltage(bool)));
    std::string result = SendTubeCommand(hostname, 1, "gethv ", "TubeWidget::GetHighVoltage");
    if (!result.empty()) {
        pushHighVoltage->setChecked(result == "on");
    }
    connect(pushHighVoltage, SIGNAL(toggled(bool)), this, SLOT(SetHighVoltage(bool)));
    bool hv = pushHighVoltage->isChecked();
    frameShutters->setEnabled(hv);
    CheckWarning();
}

void TubeWidget::SetHighVoltage(bool enable) {
    FILE_LOG(logINFO) << "Setting High Voltage: " << enable;
    frameShutters->setEnabled(enable);
    CheckWarning();
    std::ostringstream oss;
    oss << "hv " << (enable ? "on" : "off");
    std::string result = SendTubeCommand(hostname, 2, oss.str(), "TubeWidget::SetHighVoltage");
    if (result.empty()) {
        GetHighVoltage();
        CheckWarmup();
    }
    GetHighVoltage();
    if (pushHighVoltage->isChecked()) {
        UpdateActualValues();
        statusBar->showMessage("Update completed", 2 * 1000);
    } else {
        GetShutters();
        GetActualVoltage();
        GetActualCurrent();        
    }
}

void TubeWidget::GetShutters() {
    for (unsigned int i = 0; i < chkShutters.size(); ++i) {
        disconnect(chkShutters[i], SIGNAL(toggled(bool)), this, SLOT(SetShutters()));
    }
    std::string result = SendTubeCommand(hostname, 1, "getshutters ", "TubeWidget::GetShutters");
    if (!result.empty()) {
        std::istringstream iss(result);
        std::vector<std::string> list = std::vector<std::string>(std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>());
        if (list.size() < 4) {
            Message(WARNING, "Could not get shutters state", "TubeWidget::GetShutters");
        } else {
            for (unsigned int i = 0; i < chkShutters.size(); ++i) {
                chkShutters[i]->setChecked(list[i] == "on");
            }
        }   
    }
    for (unsigned int i = 0; i < chkShutters.size(); ++i) {
        connect(chkShutters[i], SIGNAL(toggled(bool)), this, SLOT(SetShutters()));
    }
    CheckWarning();
}

void TubeWidget::SetShutters() {
    CheckWarning();
    QCheckBox *checkedBox = qobject_cast<QCheckBox *>(sender());
    for (unsigned int i = 0; i < chkShutters.size(); ++i) {
        if (checkedBox == chkShutters[i]) {
            FILE_LOG(logINFO) << "Setting Shutter " << i << ": " << (chkShutters[i]->isChecked() ? "on" : "off");
            std::ostringstream oss;
            oss << "shutter " << i + 1 << ' ' << (chkShutters[i]->isChecked() ? "on" : "off");
            std::string result = SendTubeCommand(hostname, 3, oss.str(), "TubeWidget::SetShutters");
            if (result.empty()) {
                GetShutters();
            }
            continue;
        }
    }
}

void TubeWidget::CheckWarning() {
    bool shutters = false;
    for (unsigned int i = 0; i < chkShutters.size(); ++i) {
        if (chkShutters[i]->isChecked()) {
            shutters = true;
        }
    }
    frameWarning->setEnabled(pushHighVoltage->isChecked() && shutters);
}

void TubeWidget::GetVoltage() {
    disconnect(spinVoltage, SIGNAL(valueChanged(int)), this, SLOT(SetVoltage(int)));
    std::string result = SendTubeCommand(hostname, 1, "getv ", "TubeWidget::GetVoltage");
    if (!result.empty()) {
        try {
            int value = getInteger(result);
            spinVoltage->setValue(value);
        } catch (const std::exception& e) {
            Message(WARNING, e.what(), "TubeWidget::GetVoltage");
        } 
    }
    connect(spinVoltage, SIGNAL(valueChanged(int)), this, SLOT(SetVoltage(int)));
}

void TubeWidget::SetVoltage(int value) {
    FILE_LOG(logINFO) << "Setting voltage: " << value << " kV";
    std::ostringstream oss;
    oss << "setv " << value;
    std::string result = SendTubeCommand(hostname, 2, oss.str(), "TubeWidget::SetVoltage");
    if (result.empty()) {
        CheckWarmup();
    } 
    GetHighVoltage();
    GetVoltage();
    if (pushHighVoltage->isChecked()) {
        UpdateActualValues();
    } else {
        GetActualVoltage();
    }
}

void TubeWidget::GetCurrent() {
    disconnect(spinCurrent, SIGNAL(valueChanged(int)), this, SLOT(SetCurrent(int)));
    std::string result = SendTubeCommand(hostname, 1, "getc ", "TubeWidget::GetCurrent");
    if (!result.empty()) {
        try {
            int value = getInteger(result);
            spinCurrent->setValue(value);
        } catch (const std::exception& e) {
            Message(WARNING, e.what(), "TubeWidget::GetCurrent");
        } 
    }
    connect(spinCurrent, SIGNAL(valueChanged(int)), this, SLOT(SetCurrent(int)));
    GetActualCurrent();
}

void TubeWidget::SetCurrent(int value) {
    FILE_LOG(logINFO) << "Setting current: " << value << " mA";
    std::ostringstream oss;
    oss << "setc " << value;
    std::string result = SendTubeCommand(hostname, 2, oss.str(), "TubeWidget::SetCurrent");
    if (result.empty()) {
        CheckWarmup();
    } 
    GetHighVoltage();
    GetCurrent();
    if (pushHighVoltage->isChecked()) {
        UpdateActualValues();
    } else {
        GetActualCurrent();
    }
}

void TubeWidget::GetActualVoltage() {
    std::string result = SendTubeCommand(hostname, 1, "getactualv ", "TubeWidget::GetActualVoltage");
    if (!result.empty()) {
        try {
            int value = getInteger(result);
            spinActualVoltage->setValue(value);
        } catch (const std::exception& e) {
            Message(WARNING, e.what(), "TubeWidget::GetActualVoltage");
        } 
    }
}

void TubeWidget::GetActualCurrent() {
    std::string result = SendTubeCommand(hostname, 1, "getactualc ", "TubeWidget::GetActualCurrent");
    if (!result.empty()) {
        try {
            int value = getInteger(result);
            spinActualCurrent->setValue(value);
        } catch (const std::exception& e) {
            Message(WARNING, e.what(), "TubeWidget::GetActualCurrent");
        } 
    }
}

int TubeWidget::CheckWarmup() {
    FILE_LOG(logINFO) << "Checking warm up required";
    int errorCode = -1;
    std::string result = SendTubeCommand(hostname, 1, "geterr", "TubeWidget::CheckWarmup");
    if (result.empty()) {
        Message(WARNING, "Could not figure out if warm up required.", "TubeWidget::CheckWarmup");
        DisplayTubeError();
        return 0;
    }
    try {
        errorCode = getInteger(result);

    } catch (const std::exception& e) {
        Message(WARNING, "Could not warm up. " + std::string(e.what()), "TubeWidget::CheckWarmup");
        return 0;
    }
    // no error code
    if (errorCode == 0) {
        return 0;
    }
    // initial warm up (cleared via warmupcheck from  Update())
    if (errorCode == TUBE_ERROR_INITIAL_WARM_UP) {
        ClearError();
        return 0;
    }
    // error code is warmup
    if (errorCode == TUBE_ERROR_WARM_UP || errorCode == TUBE_ERROR_WARM_UP2) {
        int voltage = spinVoltage->value();
        // get last warm up timing
        std::string lastWarmupTimeStamp;
        {
            std::ostringstream oss;
            oss << "readwarmuptiming " << voltage;
            std::string result = SendTubeCommand(hostname, 2, oss.str(), "TubeWidget::CheckWarmup");
            if (result.empty()) {
                Message(WARNING, "Could not warm up.", "TubeWidget::CheckWarmup");
                DisplayTubeError();
                return 0;
            }    
            lastWarmupTimeStamp = result;    
        }

        // warm up question
        std::ostringstream oss;
        oss << "Initiate warm up? \nLast warm-up at " << voltage << " kV at " << lastWarmupTimeStamp;
        if (Message(QUESTION, oss.str(), "TubeWidget::CheckWarmup", "Warm-up?") != OK) {
            DeclineWarmup();
        } else {
            AcceptWarmup();
        }
    } 
    // unknown error code
    else {
        Message(WARNING, "Unknown Error. Please check error message field.", "TubeWidget::UpdateWarmupTiming");
        UpdateValues();
    }
    return 1;
}

void TubeWidget::AcceptWarmup() {
    int voltage = spinVoltage->value();
    int current = spinCurrent->value();

    // switch off voltage and set voltage and current again
    pushHighVoltage->setChecked(false);
    GetVoltage();
    GetCurrent();
    spinVoltage->setValue(voltage);
    spinCurrent->setValue(current);

    // start warm up
    std::ostringstream oss;
    oss << "warmup " << voltage;
    std::string result = SendTubeCommand(hostname, 2, oss.str(), "TubeWidget::CheckWarmup");
    if (result.empty()) {
        Message(WARNING, "Could not warm up.", "TubeWidget::CheckWarmup");
        DisplayTubeError();
        return;
    }   
    // check error code
    int errorCode = -1;
    result = SendTubeCommand(hostname, 1, "geterr", "TubeWidget::CheckWarmup");
    if (result.empty()) {
        DisplayTubeError();
        return;
    }
    try {
        errorCode = getInteger(result);
    } catch (const std::exception& e) {
        Message(WARNING, "Warm up failed. " + std::string(e.what()), "TubeWidget::CheckWarmup");
        return;
    }

    // warm up failed
    if (errorCode != 0) {
        std::ostringstream oss;
        oss << "Warm up failed.  Error Code: " << errorCode << ". Check error message field.";
        Message(WARNING, oss.str(), "TubeWidget::CheckWarmup");    
        UpdateValues();
    } 
    // warm up successful
    else {
        UpdateWarmupTiming();
    }
}

void TubeWidget::DeclineWarmup() {
    Message(INFORMATION, "You have chosen not to proceed with warming up. \nClearing error message and updating values ...", "TubeWidget::CheckWarmup");
    statusBar->showMessage("Updating ...");
    // clear the warm up necessary error with time to recover
    ClearError(true);
    statusBar->showMessage("Updating ...");
    // clear also the initial warm up error (clearing 109, will set errorcode to 70 again)
    ClearError();
    statusBar->showMessage("Updating ...");
    UpdateValues();
}

int TubeWidget::GetWarmupTimeRemaining() {
    int warmupTimeRemaining = -1;
    std::string result;
    while (result.empty()) {
        result = SendTubeCommand(hostname, 1, "getwtime", "TubeWidget::UpdateWarmupTiming");
        try {
            warmupTimeRemaining = getInteger(result);
        } catch (const std::exception& e) {
            FILE_LOG(logERROR) << "Could not get warm up time remaining. " << e.what();
            result = "";
        }
    }
    return warmupTimeRemaining;
}

void TubeWidget::UpdateWarmupTiming() {
    FILE_LOG(logDEBUG) << "timer warmup timeout";

    // check if there is warm up time remaining
    int wt = GetWarmupTimeRemaining();
    std::ostringstream oss;
    oss << wt << " s";
    if (wt > 60) {
        oss << " (" << (wt / 60) << " min)";
    }
    std::string wtime = oss.str();

    // updating begins
    if (!groupWarmup->isEnabled()) {
        FILE_LOG(logDEBUG) << "Begin Updating warm up timing";
        if (wt > 0) {
            Message(INFORMATION, "Warm-up of tube is in progress. It takes " + wtime, 
            "TubeWidget::UpdateWarmupTiming");
        }
        groupWarmup->setEnabled(true);
        frameHighVoltage->setEnabled(false);
        frameShutters->setEnabled(false);
        frameVoltage->setEnabled(false);
        frameWarning->setEnabled(false);
        spinActualVoltage->setStyleSheet("color: gray");
        spinActualCurrent->setStyleSheet("color: gray");
    }

    // continue timed update     
    if (wt > 0) {
        dispTime->setText(wtime.c_str());  
        timerWarmup->start(WARMUP_TIME_OUT_MS);
        statusBar->showMessage("Tube Warming up ...");
    }
    // update finished
    else {
        dispTime->setText("");

        groupWarmup->setEnabled(false);
        frameHighVoltage->setEnabled(true);
        frameShutters->setEnabled(true);
        frameVoltage->setEnabled(true);

        statusBar->showMessage("Tube warm up completed", 2 * 1000);

        UpdateValues(); 
    }
}

void TubeWidget::UpdateValues() {
    FILE_LOG(logINFO) << "Updating Tube Values";

    statusBar->showMessage("Updating ...");
    DisplayTubeError();
    statusBar->showMessage("Updating ...");
    GetHighVoltage();
    statusBar->showMessage("Updating ...");
    GetShutters();
    statusBar->showMessage("Updating ...");
    GetVoltage();
    statusBar->showMessage("Updating ...");
    GetCurrent();
    statusBar->showMessage("Updating ...");
    if (pushHighVoltage->isChecked()) {
        UpdateActualValues();
        statusBar->showMessage("Updating ...");
    } else {
        GetActualVoltage();
        statusBar->showMessage("Updating ...");
        GetActualCurrent();
    }
	statusBar->showMessage("Update completed", 2 * 1000);
}

void TubeWidget::UpdateActualValues() {
    FILE_LOG(logDEBUG) << "timer actual timeout";

    // update actual values
    GetHighVoltage(); // if off, stop updating
    GetActualVoltage();
    GetActualCurrent();
    int actualVoltage = spinActualVoltage->value();
    int actualCurrent = spinActualCurrent->value(); 
    int voltage = spinVoltage->value();
    int current = spinCurrent->value();     

    // updating begins
    if (frameVoltage->isEnabled()) {
        FILE_LOG(logDEBUG) << "Begin Updating Tube Actual Values";

        frameHighVoltage->setEnabled(false);
        frameShutters->setEnabled(false);
        frameVoltage->setEnabled(false);
        spinActualVoltage->setStyleSheet("color: red");
        spinActualCurrent->setStyleSheet("color: red");
    }
  
    // continue timed update     
    if (pushHighVoltage->isChecked() && (voltage != actualVoltage || current != actualCurrent)) {
        timerActual->start(ACTUAL_TIME_OUT_MS);
        statusBar->showMessage("Updating ...");
    } 
    // update finished
    else {
        spinActualVoltage->setStyleSheet("color: darkBlue");
        spinActualCurrent->setStyleSheet("color: darkBlue");
        frameHighVoltage->setEnabled(true);
        frameShutters->setEnabled(true);
        frameVoltage->setEnabled(true);
    }
}

std::string TubeWidget::SendTubeCommand(std::string hostname, int nCommand, std::string command, std::string source) {
    try {
        return SendCommand(hostname, nCommand, command, source);
    } catch (const TubeStandbyError& e) {
        Message(WARNING, "Tube on stand by.", "TubeWidget::GetPressure"); 
        emit SwitchedOffSignal(false);
    } catch (const TubeOffError& e) {
        Message(WARNING, "Tube switched off.", "TubeWidget::GetPressure"); 
        emit SwitchedOffSignal(false);
    }
    return "";
}

void TubeWidget::Update() {
    statusBar->showMessage("Updating ...");
    GetVoltage();
    statusBar->showMessage("Updating ...");
    GetCurrent();
    statusBar->showMessage("Updating ...");
    if (GetWarmupTimeRemaining() > 0) {
        UpdateWarmupTiming();
    } else {
        if (!CheckWarmup()) {
            UpdateValues();
        }
    }
}

