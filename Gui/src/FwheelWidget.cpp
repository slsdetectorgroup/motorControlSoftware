#include "FwheelWidget.h"
#include "GuiDefs.h"

#include <sstream>
#include <math.h>
#include <iterator>

FwheelWidget::FwheelWidget(QWidget *parent, std::string name, std::string hostname)
    : QWidget(parent), name(name), hostname(hostname) {
    setupUi(this);
    LayoutWindow();
    Initialization();
}

FwheelWidget::~FwheelWidget() {}

std::string FwheelWidget::GetName() {
    return name;
}

void FwheelWidget::LayoutWindow() {
    groupbox->setTitle(name.c_str());
	LoadAbsorptionValues();
}

void FwheelWidget::LoadAbsorptionValues() {
	std::pair <std::string, int> result = SendCommand(hostname, 2, "fwvals " + name, "FwheelWidget::LoadAbsoptionValues");
	if (result.first.empty()) {
		return;
	}
	FILE_LOG(logINFO) << "fwlist:" << result.first;
	// parse values
	std::istringstream iss(result.first);
	std::vector<std::string> list = std::vector<std::string>(std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>());
	for (unsigned int i = 0; i < list.size(); ++i) {
		comboValue->addItem(list[i].c_str());
	}
}

void FwheelWidget::Initialization() {
    connect(comboValue, SIGNAL(currentIndexChanged(int)), this, SLOT(SetValue()));
}

void FwheelWidget::GetValue() {
    disconnect(comboValue, SIGNAL(currentIndexChanged(int)), this, SLOT(SetValue()));
    std::pair <std::string, int> result = SendCommand(hostname, 2, "getfw " + name, "FwheelWidget::GetValue");
    if (!result.first.empty()) {
		// loop through all the combo list items to find a match
		bool found = false;
		for (unsigned int i = 0; i < comboValue->count(); ++i) {
			std::string text = std::string(comboValue->itemText(i).toAscii().data());
			// found match
			if (text == result.first) {
				comboValue->setCurrentIndex(i);
				found = true;
			}
		}
		// error in matching
		if (!found) {
			std::ostringstream oss;
			oss << "Could not match absorption value " << result.first << " to any in combo list for " << name;
			Message(WARNING, oss.str(), "FwheelWidget::GetValue");
		}
    }
    connect(comboValue, SIGNAL(currentIndexChanged(int)), this, SLOT(SetValue()));
    if (result.second) {
        emit UpdateSignal();
    }  
}

void FwheelWidget::SetValue() {
	std::string value = std::string(comboValue->currentText().toAscii().data());
	FILE_LOG(logINFO) << "Setting absorption value of " << name << " to " << value;
	std::ostringstream oss;
	oss << "setfw " << name << ' ' << value;
	std::pair <std::string, int> result = SendCommand(hostname, 3, oss.str(), "FwheelWidget::SetValue");
	if (result.first.empty()) {
		GetValue();
	}
	if (result.second) {
		emit UpdateSignal();
	}
}

void FwheelWidget::Update() {
    GetValue();
}

