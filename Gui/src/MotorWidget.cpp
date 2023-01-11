#include "MotorWidget.h"
#include "GuiDefs.h"

#include <QStatusBar>

#include <sstream>

MotorWidget::MotorWidget(QWidget *parent, std::string name,
                         std::string hostname, QStatusBar *statusBar)
    : QWidget(parent), name(name), hostname(hostname), statusBar(statusBar) {
    setupUi(this);
    LayoutWindow();
    Initialization();
}

MotorWidget::~MotorWidget() {}

std::string MotorWidget::GetName() { return name; }

double MotorWidget::GetPositionFromWidget() { return spinAbsolute->value(); }

void MotorWidget::LayoutWindow() { lblMotor->setText(name.c_str()); }

void MotorWidget::Initialization() {
    connect(spinAbsolute, SIGNAL(valueChanged(double)), this,
            SLOT(MoveAbsolute(double)));
    connect(pushLeft, SIGNAL(clicked()), this, SLOT(MoveLeft()));
    connect(pushRight, SIGNAL(clicked()), this, SLOT(MoveRight()));
    connect(pushCal, SIGNAL(clicked()), this, SLOT(Calibrate()));
}

void MotorWidget::GetPosition() {
    disconnect(spinAbsolute, SIGNAL(valueChanged(double)), this,
               SLOT(MoveAbsolute(double)));
    std::string result =
        SendCommand(hostname, 2, "pos " + name, "MotorWidget::GetPosition");
    if (!result.empty()) {
        try {
            double position = getDouble(result);
            spinAbsolute->setValue(position);
        } catch (const std::exception &e) {
            Message(WARNING, e.what(), "MotorWidget::GetPosition");
        }
    }
    connect(spinAbsolute, SIGNAL(valueChanged(double)), this,
            SLOT(MoveAbsolute(double)));
}

void MotorWidget::MoveAbsolute(double value) {
    LOG(logINFO) << "Moving " << name << " to " << value;
    statusBar->showMessage("Moving ...");
    statusBar->showMessage("Moving ...");
    std::ostringstream oss;
    oss << "moveabs " << name << ' ' << value;
    std::string result =
        SendCommand(hostname, 3, oss.str(), "MotorWidget::MoveAbsolute");
    statusBar->clearMessage();
    if (result.empty()) {
        GetPosition();
    } else {
        emit MotorMovedSignal();
    }
}

void MotorWidget::MoveLeft() { MoveRelative(-1 * spinRelative->value()); }

void MotorWidget::MoveRight() { MoveRelative(spinRelative->value()); }

void MotorWidget::MoveRelative(double value) {
    LOG(logINFO) << "Moving " << name << " by " << value;
    statusBar->showMessage("Moving ...");
    statusBar->showMessage("Moving ...");
    std::ostringstream oss;
    oss << "moverel " << name << ' ' << value;
    std::string result =
        SendCommand(hostname, 3, oss.str(), "MotorWidget::MoveRelative");
    statusBar->clearMessage();
    GetPosition();
    if (!result.empty()) {
        emit MotorMovedSignal();
    }
}

void MotorWidget::Calibrate() {
    if (Message(QUESTION, "Are you sure you want to calibrate the motor?",
                "MotorWidget::Calibrate", "Calibrate?") == FAIL) {
        return;
    }
    statusBar->showMessage("Calibrating ...");
    pushCal->setChecked(true);
    lblPosition->setEnabled(false);
    spinAbsolute->setEnabled(false);
    spinRelative->setEnabled(false);
    LOG(logINFO) << "Calibrating " << name;
    statusBar->showMessage("Calibrating ...");
    std::ostringstream oss;
    oss << "cal " << name;
    std::string result =
        SendCommand(hostname, 2, oss.str(), "MotorWidget::Calibrate");
    statusBar->clearMessage();
    GetPosition();
    if (!result.empty()) {
        emit MotorMovedSignal();
    }
    pushCal->setChecked(false);
    lblPosition->setEnabled(true);
    spinAbsolute->setEnabled(true);
    spinRelative->setEnabled(true);
}

void MotorWidget::Update() { GetPosition(); }
