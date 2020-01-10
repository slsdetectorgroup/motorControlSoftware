#include "FluorWidget.h"
#include "MotorWidget.h"
#include "GuiDefs.h"

#include <QStatusBar>

#include <sstream>

FluorWidget::FluorWidget(QWidget *parent, std::string name, std::string hostname, QStatusBar* statusBar)
    : QWidget(parent), name(name), hostname(hostname), statusBar(statusBar)  {
    setupUi(this);
    LayoutWindow();
    Initialization();
}

FluorWidget::~FluorWidget() {}

std::string FluorWidget::GetName() {
    return name;
}

void FluorWidget::UpdateHolderList() {
    disconnect(comboHolder, SIGNAL(currentIndexChanged(int)), this, SLOT(SetHolder(int)));
    comboHolder->clear();
    LoadTargetHolders();
    connect(comboHolder, SIGNAL(currentIndexChanged(int)), this, SLOT(SetHolder(int)));
}

void FluorWidget::LayoutWindow() {
    groupbox->setTitle(name.c_str());
    LoadTargetHolders();
    motor = new MotorWidget(this, name, hostname, statusBar);
    gridPageMotor->addWidget(motor, 0, 0);
    stackedWidget->setCurrentIndex(0);
}

void FluorWidget::LoadTargetHolders() {
	std::string result = SendCommand(hostname, 2, "numflist " + name, "FwheelWidget::LoadTargetHolders");
	if (result.empty()) {
		return;
	}
	FILE_LOG(logDEBUG) << "numfl:" << result;
    try {
        int numHolders = getInteger(result);
        if (numHolders < 1) {
            std::ostringstream oss;
            oss << "Invalid number of target holders: " << numHolders;
            throw RuntimeError (oss.str());
        }
        for (unsigned int i = 0; i < numHolders; ++i) {
            std::ostringstream oss;
            oss << "Holder " << i;
            comboHolder->addItem(oss.str().c_str());
        }
        comboHolder->setCurrentIndex(0);
    } catch (const std::exception& e) {
        Message(WARNING, e.what(), "FluorWidget::LoadTargetHolders");
    }
}

void FluorWidget::Initialization() {
    connect(comboHolder, SIGNAL(currentIndexChanged(int)), this, SLOT(SetHolder(int)));
    connect(comboTarget, SIGNAL(currentIndexChanged(int)), this, SLOT(SetTarget(int)));
    connect(pushLeft, SIGNAL(clicked()), this, SLOT(SetStackedWidget()));
    connect(pushRight, SIGNAL(clicked()), this, SLOT(SetStackedWidget()));
	connect(motor, SIGNAL(MotorMovedSignal()), this, SLOT(GetTarget()));

}

void FluorWidget::SetStackedWidget() {
    int index = stackedWidget->currentIndex() == 1 ? 0 : 1;
    stackedWidget->setCurrentIndex(index);
}

void FluorWidget::GetHolder() {
    disconnect(comboHolder, SIGNAL(currentIndexChanged(int)), this, SLOT(SetHolder(int)));
    std::string result = SendCommand(hostname, 2, "getholder " + name, "FluorWidget::GetHolder");
    if (!result.empty()) {
        try {
            int index = getInteger(result);
            comboHolder->setCurrentIndex(index);
        } catch (const std::exception& e) {
            Message(WARNING, e.what(), "FluorWidget::GetHolder");
        }
    }
    connect(comboHolder, SIGNAL(currentIndexChanged(int)), this, SLOT(SetHolder(int)));
    if (!result.empty()) {
        GetTargetList();
    }
}

void FluorWidget::GetTargetList() {
    disconnect(comboTarget, SIGNAL(currentIndexChanged(int)), this, SLOT(SetTarget(int)));
    std::ostringstream oss;
    oss << "fllist " << name << " " << comboHolder->currentIndex();
    std::string result = SendCommand(hostname, 3, oss.str(), "FwheelWidget::GetTargetList");
	if (!result.empty()) {
        FILE_LOG(logDEBUG) << "fllist:" << result;
        // parse values
        std::istringstream iss(result);
        std::vector<std::string> list = std::vector<std::string>(std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>());
        if (list.size() == 0) {
            Message(WARNING, "Could not get target list for this holder", "FluorWidget::GetTargetList");
        } else {
            // clear list first
            energy.clear();
            comboTarget->clear();

            // add targets to list
            for (unsigned int i = 0; i < list.size() / 2; ++i) {
                comboTarget->addItem(list[i * 2].c_str());
                energy.push_back(list[i * 2 + 1]);
            }
            //add None
            comboTarget->addItem("None");
            energy.push_back("-");  
            // disable none
            QModelIndex index = comboTarget->model()->index(comboTarget->count() - 1, 0); 
            QVariant v(0);
            comboTarget->model()->setData(index, v, Qt::UserRole - 1);
        }
    }
    connect(comboTarget, SIGNAL(currentIndexChanged(int)), this, SLOT(SetTarget(int)));
    if (!result.empty()) {
        GetTarget();
    }    
}

void FluorWidget::SetHolder(int index) {
    FILE_LOG(logINFO) << "Setting " << name << "'s target holder to " << index;
    std::ostringstream oss;
    oss << "setholder " << name << ' ' << index;
    std::string result = SendCommand(hostname, 3, oss.str(), "FluorWidget::SetHolder");
    if (result.empty()) {
        GetHolder();
    }
    if (!result.empty()) {
        GetTargetList();
    }
}

void FluorWidget::GetTarget() {
    disconnect(comboTarget, SIGNAL(currentIndexChanged(int)), this, SLOT(SetTarget(int)));
    std::string result = SendCommand(hostname, 2, "getfl " + name, "FluorWidget::GetTarget");
    if (!result.empty()) {
		// loop through all the combo list items to find a match
		bool found = false;
		for (unsigned int i = 0; i < comboTarget->count(); ++i) {
			std::string text = std::string(comboTarget->itemText(i).toAscii().data());
			// found match
			if (text == result) {
				comboTarget->setCurrentIndex(i);
                dispEnergy->setText(std::string(energy[i] + " KeV").c_str());
				found = true;
			}
		}
		// error in matching
		if (!found) {
			std::ostringstream oss;
			oss << "Could not match target " << result << " to any in combo list for " << name;
			Message(WARNING, oss.str(), "FluorWidget::GetTarget");
		}
    }
    connect(comboTarget, SIGNAL(currentIndexChanged(int)), this, SLOT(SetTarget(int)));
}

void FluorWidget::SetTarget(int index) {
    std::string target = std::string(comboTarget->currentText().toAscii().data());
    FILE_LOG(logINFO) << "Moving " << name << " to target " << target;
    // display energy
    dispEnergy->setText(std::string(energy[index] + " KeV").c_str());
    statusBar->showMessage("Moving ...");
    statusBar->showMessage("Moving ...");
    // move to target
    std::ostringstream oss;
    oss << "setfl " << name << ' ' << target;
    std::string result = SendCommand(hostname, 3, oss.str(), "FluorWidget::SetTarget");
    statusBar->clearMessage();
    if (result.empty()) {
        GetTarget();
    } else {
        motor->Update();
    }
}

void FluorWidget::Update() {
    motor->Update();
    GetHolder();
}
