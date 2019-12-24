#include "TubeWidget.h"
#include "GuiDefs.h"

#include <QCheckBox>

#include <sstream>

#define TUBE_ERROR_INITIAL_WARM_UP  (70)
#define TUBE_ERROR_WARM_UP  (106)
#define TUBE_ERROR_WARM_UP2  (109)

TubeWidget::TubeWidget(QWidget *parent, std::string hostname)
    :   QWidget(parent), hostname(hostname) {
	setupUi(this);
    LayoutWindow();
    Initialization();
}

TubeWidget::~TubeWidget() {}

void TubeWidget::LayoutWindow() {
    GetConnectedShutters();
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

void TubeWidget::ClearError() {
    FILE_LOG(logINFO) << "Clearing Error in Tube";
    std::string result = SendTubeCommand(hostname, 1, "clear", "TubeWidget::ClearError");
    DisplayTubeError();
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
    GetActualVoltage();
}

void TubeWidget::SetVoltage(int value) {
    FILE_LOG(logINFO) << "Setting voltage: " << value << " kV";
    std::ostringstream oss;
    oss << "setv " << value;
    std::string result = SendTubeCommand(hostname, 2, oss.str(), "TubeWidget::SetVoltage");
    if (result.empty()) {
        GetVoltage();
        CheckWarmup();
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
        GetCurrent();
        CheckWarmup();
    } else {
        GetActualCurrent();
    }
}

void TubeWidget::GetActualVoltage() {
    std::string result = SendTubeCommand(hostname, 1, "getactualv ", "TubeWidget::GetActualVoltage");
    if (!result.empty()) {
        dispVoltage->setText(result.c_str());
    }
}

void TubeWidget::GetActualCurrent() {
    std::string result = SendTubeCommand(hostname, 1, "getactualv ", "TubeWidget::GetActualVoltage");
    if (!result.empty()) {
        dispCurrent->setText(result.c_str()); 
    }
}

void TubeWidget::CheckWarmup() {
    int errorCode = -1;
    std::string result = SendTubeCommand(hostname, 1, "geterr", "TubeWidget::CheckWarmup");
    if (result.empty()) {
        DisplayTubeError();
        return;
    }
    try {
        errorCode = getInteger(result);
    } catch (const std::exception& e) {
        Message(WARNING, e.what(), "TubeWidget::CheckWarmup");
    }

    // error code is warmup
    bool warmup = false;
    if (errorCode == TUBE_ERROR_INITIAL_WARM_UP || errorCode == TUBE_ERROR_WARM_UP || errorCode == TUBE_ERROR_WARM_UP2) {
        if (Message(QUESTION, "Do you want to warm up?", "TubeWidget::CheckWarmup", "Warm-up?") == OK) {
            warmup = true;
        }
    }
    std::cout << "Warmup:" << warmup << std::endl;
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
    GetHighVoltage();
    GetShutters();
    GetVoltage();
    GetCurrent();
    GetActualVoltage();
    GetActualCurrent();
    CheckWarmup();
    DisplayTubeError();
}

