#include "FluorWidget.h"
#include "MotorWidget.h"
#include "GuiDefs.h"

#include <sstream>

FluorWidget::FluorWidget(QWidget *parent, std::string name, std::string hostname)
    : QWidget(parent), name(name), hostname(hostname) {
    setupUi(this);
    LayoutWindow();
    Initialization();
}

FluorWidget::~FluorWidget() {}

std::string FluorWidget::GetName() {
    return name;
}

void FluorWidget::LayoutWindow() {
    groupbox->setTitle(name.c_str());
    //comboHolder->lineEdit()->setAlignment(Qt::AlignHCenter);
    //comboTarget->lineEdit()->setAlignment(Qt::AlignHCenter);
    LoadTargetHolders();
    motor = new MotorWidget(this, name, hostname);
    gridPageMotor->addWidget(motor, 0, 0);
    stackedWidget->setCurrentIndex(0);
}

void FluorWidget::LoadTargetHolders() {
	std::pair <std::string, int> result = SendCommand(hostname, 2, "numflist " + name, "FwheelWidget::LoadTargetHolders");
	if (result.first.empty()) {
		return;
	}
	FILE_LOG(logDEBUG) << "numfl:" << result.first;
    if (!result.first.empty()) {
        try {
            int numHolders = getInteger(result.first);
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
    std::pair <std::string, int> result = SendCommand(hostname, 2, "whichflist " + name, "FluorWidget::GetHolder");
    if (!result.first.empty()) {
        try {
            int index = getInteger(result.first);
            comboHolder->setCurrentIndex(index);
        } catch (const std::exception& e) {
            Message(WARNING, e.what(), "FluorWidget::GetHolder");
        }
    }
    connect(comboHolder, SIGNAL(currentIndexChanged(int)), this, SLOT(SetHolder(int)));
    if (result.second) {
        emit UpdateSignal();
    } else {
        GetTargetList();
    }
}

void FluorWidget::GetTargetList() {
    disconnect(comboTarget, SIGNAL(currentIndexChanged(int)), this, SLOT(SetTarget(int)));
    std::ostringstream oss;
    oss << "fllist " << name << " " << comboHolder->currentIndex();
    std::pair <std::string, int> result = SendCommand(hostname, 3, oss.str(), "FwheelWidget::GetTargetList");
	if (!result.first.empty()) {
        FILE_LOG(logDEBUG) << "fllist:" << result.first;
        // parse values
        std::istringstream iss(result.first);
        std::vector<std::string> list = std::vector<std::string>(std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>());
        if (list.size() == 0) {
            Message(CRITICAL, "Could not get target list for this holder", "FluorWidget::GetTargetList");
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
    if (result.second) {
        emit UpdateSignal();
    } else {
        GetTarget();
    }    
}

void FluorWidget::SetHolder(int index) {
    FILE_LOG(logINFO) << "Setting " << name << "'s target holder to " << index;
    std::ostringstream oss;
    oss << "loadflist " << name << ' ' << index;
    std::pair <std::string, int> result = SendCommand(hostname, 3, oss.str(), "FluorWidget::SetHolder");
    if (result.first.empty()) {
        GetHolder();
    }
    if (result.second) {
        emit UpdateSignal();
    } else {
        GetTargetList();
    }
}

void FluorWidget::GetTarget() {
    disconnect(comboTarget, SIGNAL(currentIndexChanged(int)), this, SLOT(SetTarget(int)));
    std::pair <std::string, int> result = SendCommand(hostname, 2, "getfl " + name, "FluorWidget::GetTarget");
    if (!result.first.empty()) {
		// loop through all the combo list items to find a match
		bool found = false;
		for (unsigned int i = 0; i < comboTarget->count(); ++i) {
			std::string text = std::string(comboTarget->itemText(i).toAscii().data());
			// found match
			if (text == result.first) {
				comboTarget->setCurrentIndex(i);
                dispEnergy->setText(std::string(energy[i] + " KeV").c_str());
				found = true;
			}
		}
		// error in matching
		if (!found) {
			std::ostringstream oss;
			oss << "Could not match target " << result.first << " to any in combo list for " << name;
			Message(WARNING, oss.str(), "FluorWidget::GetTarget");
		}
    }
    connect(comboTarget, SIGNAL(currentIndexChanged(int)), this, SLOT(SetTarget(int)));
    if (result.second) {
        emit UpdateSignal();
    }  
}

void FluorWidget::SetTarget(int index) {
    std::string target = std::string(comboTarget->currentText().toAscii().data());
    FILE_LOG(logINFO) << "Moving " << name << " to target " << target;
    // display energy
    dispEnergy->setText(std::string(energy[index] + " KeV").c_str());
    // move to target
    std::ostringstream oss;
    oss << "movefl " << name << ' ' << target;
    std::pair <std::string, int> result = SendCommand(hostname, 3, oss.str(), "FluorWidget::SetTarget");
    if (result.first.empty()) {
        GetTarget();
    }
    if (result.second) {
        emit UpdateSignal();
    } else {
        motor->Update();
    }
}

void FluorWidget::Update() {
    motor->Update();
    GetHolder();
}
