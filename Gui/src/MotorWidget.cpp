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
    if (result.first.empty()) {
        GetPosition();
    }
    if (result.second) {
        emit UpdateSignal();
    }
}

void MotorWidget::Update() {
    GetPosition();
}
