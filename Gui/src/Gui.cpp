#include "Gui.h"
#include "MotorWidget.h"
#include "SlitWidget.h"
#include "FwheelWidget.h"
#include "GuiDefs.h"

#include <QtGui>

Gui::Gui(std::string hostname)
	: QMainWindow(0), hostname(hostname), slits(NULL) {
	setupUi(this);
	LayoutWindow();
	Initialization();
	Update();
}

Gui::~Gui() {}

void Gui::LayoutWindow() {
	layoutDone = false;
	
	LoadMotorWidgets();
	LoadFwheelWidgets();
	
	layoutDone = true;
}

void Gui::LoadMotorWidgets() {
	
	std::pair <std::string, int> result;
	while (result.first.empty()) {
 		result = SendCommand(hostname, 1, "listmotors", "Gui::LayoutWindow");
	}
	FILE_LOG(logDEBUG) << "listmotors:" << result.first;

	// parse motor names
	std::istringstream iss(result.first);
	while (iss.good()) {
		std::string motorName;
		iss >> motorName;
		if (motorName.empty()) {
			continue;
		}

		// create slit widget
		if (motorName.find("Slit") != std::string::npos) {
			if (slits == NULL) {
				groupSlits->setEnabled(true);
				lblSlits->hide();
				slits = new SlitWidget(this, hostname);
				gridSlits->addWidget(slits, 0, 0);
			}
			continue;
		}

		// create fluorescence widget
		if (motorName.find("Fluorescence") != std::string::npos) {
			std::cout << "creating fluorescence widget" << std::endl;
			continue;
		}

		// create motor widget
		if (motorWidgets.size() == 0) {
			groupMotors->setEnabled(true);
			lblMotors->hide();
		}	
		motorWidgets.push_back(new MotorWidget(this, motorName, hostname));
		int currentIndex = motorWidgets.size() - 1;
		gridMotors->addWidget(motorWidgets[currentIndex], currentIndex, 0);
	}	
}

void Gui::LoadFwheelWidgets() {
	// get number of filter wheels
	std::pair <std::string, int> result = SendCommand(hostname, 1, "nfw", "Gui::LoadFwheelWidgets");
    if (result.first.empty()) {
		return;
	}
	try {
		// if no filter wheels, return
		int nfwheel = getInteger(result.first);
		if (nfwheel == 0) {
			return;
		}
	} catch (const std::exception& e) {
		Message(WARNING, e.what(), "Gui::LoadFwheelWidgets");
	}

	// get list of filter wheel names
	result = SendCommand(hostname, 1, "fwlist", "Gui::LoadFwheelWidgets");
	if (result.first.empty()) {
		return;
	}
	FILE_LOG(logDEBUG) << "fwlist:" << result.first;

	// parse filter wheel names
	std::istringstream iss(result.first);
	while (iss.good()) {
		std::string name;
		iss >> name;
		if (name.empty()) {
			continue;
		}

		// create filter wheel widget
		if (fwheelWidgets.size() == 0) {
			groupFilterWheels->setEnabled(true);
			lblFilterWheels->hide();
		}

		fwheelWidgets.push_back(new FwheelWidget(this, name, hostname));
		int currentIndex = fwheelWidgets.size() - 1;
		gridFilterWheels->addWidget(fwheelWidgets[currentIndex], currentIndex, 0);
	}
}

void Gui::Initialization() {
	connect(pushUpdate, SIGNAL(clicked()), this, SLOT(Update()));
	for (unsigned int i = 0; i < motorWidgets.size(); ++i) {
		connect(motorWidgets[i], SIGNAL(UpdateSignal()), this, SLOT(Update()));
	}
	if (slits != NULL) {
		connect(slits, SIGNAL(UpdateSignal()), this, SLOT(Update()));		
	}
}

void Gui::Update() {
	if (!layoutDone) {
		return;
	}
	statusbar->showMessage("Updating ...");

	// update motors
	for (unsigned int i = 0; i < motorWidgets.size(); ++i) {
		motorWidgets[i]->Update();
	}
	// udpate slits
	if (slits != NULL) {
		slits->Update();
	}
	statusbar->showMessage("Update completed", 3 * 1000);
}

void Gui::closeEvent(QCloseEvent* event) {
	if (FAIL == Message(QUESTION,  
		"Are you sure you want to quit?", 
		"Gui::closeEvent", 
		"Quit Motor Control GUI")) {
		event->ignore();
	} else {
		// warmuptimings write //TODO
		SendCommand(hostname, 1, "unlock", "Gui::closeEvent");
		event->accept();
	}
}