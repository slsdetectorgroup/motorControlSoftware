#include "ReferencePointsWidget.h"
#include "MotorWidget.h"
#include "GuiDefs.h"

#include <QStatusBar>

#include <sstream>

ReferencePointsWidget::ReferencePointsWidget(QWidget *parent, std::string hostname, MotorWidget* x, MotorWidget* y, MotorWidget* z, QStatusBar* statusBar)
    : QWidget(parent), hostname(hostname), x(x), y(y), z(z), statusBar(statusBar)  {
    setupUi(this);
    LayoutWindow();
    Initialization();
}

ReferencePointsWidget::~ReferencePointsWidget() {}

std::vector <std::string> ReferencePointsWidget::GetList() {
    std::vector <std::string> result;
    for (int i = 0; i < comboReference->count(); ++i) {
        std::string name = comboReference->itemText(i).toAscii().data();
        if (name != "None") {
            result.push_back(name);
        }
    }
    return result;
}
void ReferencePointsWidget::LayoutWindow() {
    LoadReferencePoints();
}

void ReferencePointsWidget::LoadReferencePoints() {
	std::string result = SendCommand(hostname, 1, "reflist ", "FwheelWidget::LoadReferencePoints");
	if (result.empty()) {
		return;
	}
	FILE_LOG(logDEBUG) << "reflist:" << result;
    // parse values
    std::istringstream iss(result);
    std::vector<std::string> list = std::vector<std::string>(std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>());
    if (list.size() == 0) {
        Message(CRITICAL, "Could not get reference points list", "ReferencePointsWidget::LoadReferencePoints");
        return;
    }    
    // add reference points to list
    for (unsigned int i = 0; i < list.size(); ++i) {
        comboReference->addItem(list[i].c_str());
    }    
    //add None
    comboReference->addItem("None");
    // disable none
    QModelIndex index = comboReference->model()->index(comboReference->count() - 1, 0); 
    QVariant v(0);
    comboReference->model()->setData(index, v, Qt::UserRole - 1);    
}

void ReferencePointsWidget::Initialization() {
    connect(comboReference, SIGNAL(currentIndexChanged(int)), this, SLOT(SetReferencePoint(int)));
	connect(x, SIGNAL(MotorMovedSignal()), this, SLOT(GetReferencePoint()));
	connect(y, SIGNAL(MotorMovedSignal()), this, SLOT(GetReferencePoint()));
	connect(z, SIGNAL(MotorMovedSignal()), this, SLOT(GetReferencePoint()));
}

void ReferencePointsWidget::GetReferencePoint() {
    disconnect(comboReference, SIGNAL(currentIndexChanged(int)), this, SLOT(SetReferencePoint(int)));
    std::string result = SendCommand(hostname, 1, "getref ", "ReferencePointsWidget::GetReferencePoint");
    if (!result.empty()) {
		// loop through all the combo list items to find a match
		bool found = false;
		for (int i = 0; i < comboReference->count(); ++i) {
			std::string text = std::string(comboReference->itemText(i).toAscii().data());
			// found match
			if (text == result) {
				comboReference->setCurrentIndex(i);
				found = true;
			}
		}
		// error in matching
		if (!found) {
			std::ostringstream oss;
			oss << "Could not match reference point " << result << " to any in combo list.";
			Message(WARNING, oss.str(), "ReferencePointsWidget::GetReferencePoint");
		}
    }
    connect(comboReference, SIGNAL(currentIndexChanged(int)), this, SLOT(SetReferencePoint(int)));
}

void ReferencePointsWidget::SetReferencePoint(int index) {
    std::string ref = std::string(comboReference->currentText().toAscii().data());
    FILE_LOG(logINFO) << "Moving to reference point " << ref;
    statusBar->showMessage("Moving ...");
    statusBar->showMessage("Moving ...");
    std::ostringstream oss;
    oss << "setref " << ref;
    std::string result = SendCommand(hostname, 2, oss.str(), "ReferencePointsWidget::SetReferencePoint");
    statusBar->clearMessage();
    if (result.empty()) {
        GetReferencePoint();
    } else {
        x->Update();
        y->Update();
        z->Update();
    }
}

void ReferencePointsWidget::Update() {
    x->Update();
    y->Update();
    z->Update();
    GetReferencePoint();
}
