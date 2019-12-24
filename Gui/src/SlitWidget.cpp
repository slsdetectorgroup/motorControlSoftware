#include "SlitWidget.h"
#include "MotorWidget.h"
#include "GuiDefs.h"

SlitWidget::SlitWidget(QWidget *parent, std::string hostname)
    : QWidget(parent), hostname(hostname), motorx1(NULL), motorx2(NULL) {
    setupUi(this);
    LayoutWindow();
    Initialization();
}

SlitWidget::~SlitWidget() {}

void SlitWidget::LayoutWindow() {
    motorx1 = new MotorWidget(this, "Slit_x1", hostname);
    gridSlitList->addWidget(motorx1, 0, 0);

    motorx2 = new MotorWidget(this, "Slit_x2", hostname);
    gridSlitList->addWidget(motorx2, 1, 0);
}

void SlitWidget::Initialization() {
	connect(spinSlitWidth, SIGNAL(valueChanged(double)), this, SLOT(SetSlitWidth(double)));
	connect(spinCenter, SIGNAL(valueChanged(double)), this, SLOT(SetCenter(double)));
	connect(motorx1, SIGNAL(MotorMovedSignal()), this, SLOT(Update()));
	connect(motorx2, SIGNAL(MotorMovedSignal()), this, SLOT(Update()));
}

void SlitWidget::GetSlitWidth() {
	disconnect(spinSlitWidth, SIGNAL(valueChanged(double)), this, SLOT(SetSlitWidth(double)));
    std::string result = SendCommand(hostname, 1, "getslitwidth", "SlitWidget::GetSlitWidth");
    if (!result.empty()) {
        try {
            double value = getDouble(result); 
            spinSlitWidth->setValue(value);
        } catch (const std::exception& e) {
            Message(WARNING, e.what(), "SlitWidget::GetSlitWidth");
        }
    }
	connect(spinSlitWidth, SIGNAL(valueChanged(double)), this, SLOT(SetSlitWidth(double)));
}

void SlitWidget::SetSlitWidth(double value) {
    FILE_LOG(logINFO) << "Setting slit width to " << value;
    std::ostringstream oss;
    oss << "setslitwidth " << value;
    std::string result = SendCommand(hostname, 2, oss.str(), "SlitWidget::SetSlitWidth");
    if (result.empty()) {
        GetSlitWidth();
    } else {
        motorx1->Update();
        motorx2->Update();        
    }
}

void SlitWidget::GetCenter() {
	disconnect(spinCenter, SIGNAL(valueChanged(double)), this, SLOT(SetCenter(double)));
    std::string result = SendCommand(hostname, 1, "getcenter", "SlitWidget::GetCenter");
    if (!result.empty()) {
        try {
            double value = getDouble(result); 
            spinCenter->setValue(value);
        } catch (const std::exception& e) {
            Message(WARNING, e.what(), "SlitWidget::GetSlitWidth");
        }
    } 
	connect(spinCenter, SIGNAL(valueChanged(double)), this, SLOT(SetCenter(double)));
}

void SlitWidget::SetCenter(double value) {
    FILE_LOG(logINFO) << "Setting center to " << value;
    std::ostringstream oss;
    oss << "setcenter " << value;
    std::string result = SendCommand(hostname, 2, oss.str(), "SlitWidget::SetCenter");
    if (result.empty()) {
        GetCenter();
    } else {
        motorx1->Update();
        motorx2->Update();        
    }
}

void SlitWidget::Update() {
    motorx1->Update();
    motorx2->Update();    
    GetSlitWidth();
    GetCenter();
}
