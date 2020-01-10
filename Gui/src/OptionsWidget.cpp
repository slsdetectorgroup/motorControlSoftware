#include "OptionsWidget.h"
#include "GuiDefs.h"

#include <QCloseEvent>

#include <iterator>

OptionsWidget::OptionsWidget(QWidget *parent, std::string hostname, 
    std::vector <std::string> motors, 
    std::vector <std::string> refpoints,
    std::vector <std::string> fluor)
    :QMainWindow(parent), hostname(hostname) {
    setupUi(this);
	LayoutWindow(motors, refpoints, fluor);
	Initialization();    
}

OptionsWidget::~OptionsWidget() {}

void OptionsWidget::LayoutWindow(
    std::vector <std::string> motors, 
    std::vector <std::string> refpoints,
    std::vector <std::string> fluor) {

    // motors
    for (size_t i = 0; i < motors.size(); ++i) {
        comboMotor->addItem(motors[i].c_str());
    }
    // ref points
    for (size_t i = 0; i < refpoints.size(); ++i) {
        comboReferencePoint->addItem(refpoints[i].c_str());
    } 
    if (comboReferencePoint->count() == 0) {
        groupReferencePoints->setEnabled(false);
    }    
    // fluor widgets
    for (size_t i = 0; i < fluor.size(); ++i) {
        comboFluor->addItem(fluor[i].c_str());
    } 
    if (comboFluor->count() == 0) {
        groupFluor->setEnabled(false);
    }  
    targets.push_back(dispTarget1);
    targets.push_back(dispTarget2);
    targets.push_back(dispTarget3);
    targets.push_back(dispTarget4);
    targets.push_back(dispTarget5);
    targets.push_back(dispTarget6);
    targets.push_back(dispTarget7);
    targets.push_back(dispTarget8);
    energy.push_back(spinEnergy1);
    energy.push_back(spinEnergy2);
    energy.push_back(spinEnergy3);
    energy.push_back(spinEnergy4);
    energy.push_back(spinEnergy5);
    energy.push_back(spinEnergy6);
    energy.push_back(spinEnergy7);
    energy.push_back(spinEnergy8);
}

void OptionsWidget::Initialization() {
    connect(comboMotor, SIGNAL(currentIndexChanged(int)), this, SLOT(SetMotor(int)));
    connect(spinPosition, SIGNAL(valueChanged(double)), this, SLOT(SetPosition(double)));
    connect(spinUpper, SIGNAL(valueChanged(double)), this, SLOT(SetUpperLimit(double)));
    connect(spinLower, SIGNAL(valueChanged(double)), this, SLOT(SetLowerLimit(double)));
    connect(comboReferencePoint, SIGNAL(currentIndexChanged(int)), this, SLOT(SetRefPoint(int)));
    connect(comboFluor, SIGNAL(currentIndexChanged(int)), this, SLOT(SetFluor(int)));
    connect(pushDeleteHolder, SIGNAL(clicked()), this, SLOT(DeleteHolder()));
    connect(pushAddHolder, SIGNAL(clicked()), this, SLOT(AddHolder()));
    connect(pushClear, SIGNAL(clicked()), this, SLOT(ClearHolderContents()));
}

void OptionsWidget::SetMotor(int index) {
    GetMotorController();
    GetPosition();
    GetUpperLimit();
    GetLowerLimit();
    std::string name = std::string(comboMotor->currentText().toAscii().data());
    bool allowed = true;
    if ((name.find("Slit") != std::string::npos) || (name.find("Fluorescence") != std::string::npos)) {
        allowed = false;
    }
    spinPosition->setEnabled(allowed);
    spinUpper->setEnabled(allowed);
    spinLower->setEnabled(allowed); 
}

void OptionsWidget::GetMotorController() {
    std::ostringstream oss;
    oss << "getcontroller " << comboMotor->currentText().toAscii().data();
    std::string result = SendCommand(hostname, 2, oss.str(), "OptionsWidget::GetMotorController");
    lblMotorControllerValue->setText(result.c_str());
}

void OptionsWidget::GetPosition() {
    std::string name = std::string(comboMotor->currentText().toAscii().data());
    disconnect(spinPosition, SIGNAL(valueChanged(double)), this, SLOT(SetPosition(double)));
    std::string result = SendCommand(hostname, 2, "pos " + name, "OptionsWidget::GetPosition");
    if (!result.empty()) {
        try {
            double position = getDouble(result);
            spinPosition->setValue(position);
        } catch (const std::exception& e) {
            Message(WARNING, e.what(), "OptionsWidget::GetPosition");
        }
    }
    connect(spinPosition, SIGNAL(valueChanged(double)), this, SLOT(SetPosition(double)));
}

void OptionsWidget::SetPosition(double value) {
    if (Message(QUESTION, "Are you sure you want to overwrite current position?", "OptionsWidget::SetPosition") == FAIL) {
        GetPosition();
        return;
    }  
    std::string name = std::string(comboMotor->currentText().toAscii().data());
    FILE_LOG(logINFO) << "Overwriting current position of " << name << " to " << value;
    std::ostringstream oss;
    oss << "setpos " << name << ' ' << value;
    std::string result = SendCommand(hostname, 3, oss.str(), "OptionsWidget::SetPosition");
    if (!result.empty()) {
        GetPosition();
    }
}

void OptionsWidget::GetUpperLimit() {
    std::string name = std::string(comboMotor->currentText().toAscii().data());
    disconnect(spinUpper, SIGNAL(valueChanged(double)), this, SLOT(SetUpperLimit(double)));
    std::string result = SendCommand(hostname, 2, "getupper " + name, "OptionsWidget::GetUpperLimit");
    if (!result.empty()) {
        try {
            double position = getDouble(result);
            spinUpper->setValue(position);
        } catch (const std::exception& e) {
            Message(WARNING, e.what(), "OptionsWidget::GetUpperLimit");
        }
    }    
    connect(spinUpper, SIGNAL(valueChanged(double)), this, SLOT(SetUpperLimit(double)));
}

void OptionsWidget::SetUpperLimit(double value) {
    std::string name = std::string(comboMotor->currentText().toAscii().data());
    FILE_LOG(logINFO) << "Setting " << name << "'s upper limit to " << value;
    std::ostringstream oss;
    oss << "setupper " << name << ' ' << value;
    std::string result = SendCommand(hostname, 3, oss.str(), "OptionsWidget::SetUpperLimit");
    if (result.empty()) {
        GetUpperLimit();
    }
}

void OptionsWidget::GetLowerLimit() {
    std::string name = std::string(comboMotor->currentText().toAscii().data());
    disconnect(spinLower, SIGNAL(valueChanged(double)), this, SLOT(SetLowerLimit(double)));
    std::string result = SendCommand(hostname, 2, "getlower " + name, "OptionsWidget::GetLowerLimit");
    if (!result.empty()) {
        try {
            double position = getDouble(result);
            spinLower->setValue(position);
        } catch (const std::exception& e) {
            Message(WARNING, e.what(), "OptionsWidget::GetLowerLimit");
        }
    }    
    connect(spinLower, SIGNAL(valueChanged(double)), this, SLOT(SetLowerLimit(double)));
}

void OptionsWidget::SetLowerLimit(double value) {
    std::string name = std::string(comboMotor->currentText().toAscii().data());
    FILE_LOG(logINFO) << "Setting " << name << "'s lower limit to " << value;
    std::ostringstream oss;
    oss << "setlower " << name << ' ' << value;
    std::string result = SendCommand(hostname, 3, oss.str(), "OptionsWidget::SetLowerLimit");
    if (result.empty()) {
        GetLowerLimit();
    }
}

void OptionsWidget::SetRefPoint(int index) {
    std::string name = std::string(comboReferencePoint->currentText().toAscii().data());
    std::string result = SendCommand(hostname, 2, "refvals " + name, "OptionsWidget::SetRefPoint");
    if (!result.empty()) {
        std::istringstream iss(result);
        std::vector<std::string> args = std::vector<std::string>(std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>());
        if (args.size() != 3) {
            Message(WARNING, "Could not get positions for reference point " + name, "OptionsWidget::SetRefPoint");
        } else {
            lblDetXPositionValue->setText(args[0].c_str());
            lblDetYPositionValue->setText(args[1].c_str());
            lblDetZPositionValue->setText(args[2].c_str());
        }
    }    
}

void OptionsWidget::SetFluor(int index) {
    std::string name = std::string(comboFluor->currentText().toAscii().data());
    bool enableHolderEdit = (name == "Fluorescence_wheel" ? false : true);
    pushDeleteHolder->setEnabled(enableHolderEdit);
    groupHolder->setEnabled(enableHolderEdit);
    GetNumFluorHolders();
    ClearHolderContents();
}

void OptionsWidget::GetNumFluorHolders() {
    std::string name = std::string(comboFluor->currentText().toAscii().data());
    std::string result = SendCommand(hostname, 2, "numflist " + name, "OptionsWidget::GetNumFluorHolders");
    if (!result.empty()) {
        lblFluorNumHoldersValue->setText(result.c_str());
    }    
}

void OptionsWidget::DeleteHolder() {
    if (Message(QUESTION, "Are you sure you want to delete the last holder?", "OptionsWidget::DeleteHolder") == FAIL) {
        return;
    }    
    std::string name = std::string(comboFluor->currentText().toAscii().data());
    std::string result = SendCommand(hostname, 2, "dellastholder " + name, "OptionsWidget::DeletHolder");
    GetNumFluorHolders();     
}

void OptionsWidget::AddHolder() {
    std::string name = std::string(comboFluor->currentText().toAscii().data());
    std::ostringstream oss;
    oss << "addholder " << name << ' ';
    for (size_t i = 0; i < targets.size(); ++i) {
        oss << targets[i]->text().toAscii().data() << ' ';
        oss << energy[i]->value() << ' ';
    }
    std::string result = SendCommand(hostname, 2 + 2 * targets.size(), oss.str(), "OptionsWidget::AddHolder");
    if (!result.empty()) {
        ClearHolderContents();   
    }
    GetNumFluorHolders();     
}

void OptionsWidget::ClearHolderContents() {
    for (size_t i = 0; i < targets.size(); ++i) {
        targets[i]->setText("");
        energy[i]->setValue(0);
    }  
}

void OptionsWidget::Update() {
    SetMotor(comboMotor->currentIndex());
    if (groupReferencePoints->isEnabled()) {
        SetRefPoint(comboReferencePoint->currentIndex());
    }
    if (groupFluor->isEnabled()) {
        SetFluor(comboFluor->currentIndex());
    }
}

void OptionsWidget::closeEvent(QCloseEvent* event) {
    emit ClosedSignal();
	event->accept();
}
