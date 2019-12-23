#include "MotorWidget.h"
#include "GuiDefs.h"

#include <sstream>

MotorWidget::MotorWidget(QWidget *parent, std::string name, std::string hostname)
    : QWidget(parent), name(name), hostname(hostname) {
    setupUi(this);
    LayoutWindow();
    Initialization();
}

MotorWidget::~MotorWidget() {}

std::string MotorWidget::GetName() {
    return name;
}

void MotorWidget::LayoutWindow() {
    groupbox->setTitle(name.c_str());
}

void MotorWidget::Initialization() {
    connect(spinAbsolute, SIGNAL(valueChanged(double)), this, SLOT(MoveAbsolute(double)));
    connect(pushLeft, SIGNAL(clicked()), this, SLOT(MoveLeft()));
    connect(pushRight, SIGNAL(clicked()), this, SLOT(MoveRight()));
    connect(pushCal, SIGNAL(clicked()), this, SLOT(Calibrate()));
}

void MotorWidget::GetPosition() {
    disconnect(spinAbsolute, SIGNAL(valueChanged(double)), this, SLOT(MoveAbsolute(double)));
    std::pair <std::string, int> result = SendCommand(hostname, 2, "pos " + name, "MotorWidget::GetPosition");
    if (!result.first.empty()) {
        try {
            double position = getDouble(result.first);
            spinAbsolute->setValue(position);
        } catch (const std::exception& e) {
            Message(WARNING, e.what(), "MotorWidget::GetPosition");
        }
    }
    connect(spinAbsolute, SIGNAL(valueChanged(double)), this, SLOT(MoveAbsolute(double)));
    if (result.second) {
        emit UpdateSignal();
    }  
}

void MotorWidget::MoveAbsolute(double value) {
    FILE_LOG(logINFO) << "Moving " << name << " to " << value;
    std::ostringstream oss;
    oss << "moveabs " << name << ' ' << value;
    std::pair <std::string, int> result = SendCommand(hostname, 3, oss.str(), "MotorWidget::MoveAbsolute");
    if (result.first.empty()) {
        GetPosition();
    }
    if (result.second) {
        emit UpdateSignal();
    } else {
        emit MotorMovedSignal();
    }
}

void MotorWidget::MoveLeft() {
    MoveRelative(-1 * spinRelative->value());
}

void MotorWidget::MoveRight() {
    MoveRelative(spinRelative->value());
}

void MotorWidget::MoveRelative(double value) {
    FILE_LOG(logINFO) << "Moving " << name << " by " << value;
    std::ostringstream oss;
    oss << "moverel " << name << ' ' << value;
    std::pair <std::string, int> result = SendCommand(hostname, 3, oss.str(), "MotorWidget::MoveRelative");
    GetPosition();
    if (result.second) {
        emit UpdateSignal();
    } else {
        emit MotorMovedSignal();
    }
}

void MotorWidget::Calibrate() {
    if (Message(QUESTION, "Are you sure you want to calibrate the motor?", "MotorWidget::Calibrate", "Calibrate?") == FAIL) {
        return;
    }
    pushCal->setChecked(true);
    lblPosition->setEnabled(false);
    spinAbsolute->setEnabled(false);
    spinRelative->setEnabled(false);
    FILE_LOG(logINFO) << "Calibrating " << name;
    std::ostringstream oss;
    oss << "cal " << name;
    std::pair <std::string, int> result = SendCommand(hostname, 2, oss.str(), "MotorWidget::Calibrate");
    Message(INFORMATION, "Calibration Completed", "MotorWidget::Calibrate");
    GetPosition();
    if (result.second) {
        emit UpdateSignal();
    } else {
        emit MotorMovedSignal();
    }
    pushCal->setChecked(false);
    lblPosition->setEnabled(true);
    spinAbsolute->setEnabled(true);
    spinRelative->setEnabled(true);
}


void MotorWidget::Update() {
    GetPosition();
}
