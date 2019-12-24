#include "Gui.h"
#include "MotorWidget.h"
#include "ReferencePointsWidget.h"
#include "FluorWidget.h"
#include "SlitWidget.h"
#include "FwheelWidget.h"
#include "PGaugeWidget.h"
#include "TubeWidget.h"
#include "GuiDefs.h"

#include <QtGui>

Gui::Gui(std::string hostname)
	: QMainWindow(0), hostname(hostname), refpoints(NULL), slits(NULL), pgauge(NULL), tube(NULL) {
	setupUi(this);
	LayoutWindow();
	Initialization();
	Update();
}

Gui::~Gui() {}

void Gui::LayoutWindow() {
	layoutDone = false;
	
	std::ostringstream oss;
#ifdef XRAYBOX
	oss << "XRay Box Gui";
#elif LASERBOX
	oss << "Laser Box Gui";
#else
	oss << "Vacuum Box Gui";
#endif
	oss << " - Motor Control Software (" << hostname << ")";
	std::string title = oss.str();
	FILE_LOG(logINFOBLUE) << title;
	setWindowTitle(title.c_str());

	LoadMotorWidgets();
	LoadReferencePointsWidget();
	LoadFwheelWidgets();

// takes a lot of time to look if pressure gauge port exists
#ifdef VACUUMBOX
	groupPressure->setChecked(true);
#endif
	groupTube->setChecked(true);

	pushStop->hide();
	
	layoutDone = true;
}

void Gui::LoadMotorWidgets() {
	
	std::string result;
	while (result.empty()) {
 		result = SendCommand(hostname, 1, "listmotors", "Gui::LayoutWindow");
	}
	FILE_LOG(logDEBUG) << "listmotors:" << result;

	// parse motor names
	std::istringstream iss(result);
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
			if (fluorWidgets.size() == 0) {
				groupFluorescence->setEnabled(true);
				lblFluors->hide();
			}	
			fluorWidgets.push_back(new FluorWidget(this, motorName, hostname));
			int currentIndex = fluorWidgets.size() - 1;
			gridFluorescence->addWidget(fluorWidgets[currentIndex], currentIndex, 0);
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

void Gui::LoadReferencePointsWidget() {
	// get number of reference points
	std::string result = SendCommand(hostname, 1, "nref", "Gui::LoadReferencePointsWidget");
    if (result.empty()) {
		return;
	}
	try {
		// if no refernce points, return
		int nref = getInteger(result);
		if (nref == 0) {
			return;
		}
	} catch (const std::exception& e) {
		Message(WARNING, e.what(), "Gui::LoadReferencePointsWidget");
		return;
	}
	MotorWidget* x = NULL;
	MotorWidget* y = NULL;
	MotorWidget* z = NULL;
	for (unsigned int i = 0; i< motorWidgets.size(); ++i) {
		if (motorWidgets[i]->GetName() == "Detector_x") {
			x = motorWidgets[i];
			continue;
		}
		if (motorWidgets[i]->GetName() == "Detector_y") {
			y = motorWidgets[i];
			continue;
		}
		if (motorWidgets[i]->GetName() == "Detector_z") {
			z = motorWidgets[i];
			continue;
		}
	}
	if (x == NULL || y == NULL || z == NULL) {
		Message(WARNING, "No reference points added as Detector_x, Detector_y or Detector_z is missing", "Gui::LoadReferencePointsWidget");
		return;
	}
	groupReferencePoints->setEnabled(true);
	lblReferencePoints->hide();
	refpoints = new ReferencePointsWidget(this, hostname, x, y, z);
	gridReferencePoints->addWidget(refpoints, 0, 0);	
}

void Gui::LoadFwheelWidgets() {
	// get number of filter wheels
	std::string result = SendCommand(hostname, 1, "nfw", "Gui::LoadFwheelWidgets");
    if (result.empty()) {
		return;
	}
	try {
		// if no filter wheels, return
		int nfwheel = getInteger(result);
		if (nfwheel == 0) {
			return;
		}
	} catch (const std::exception& e) {
		Message(WARNING, e.what(), "Gui::LoadFwheelWidgets");
		return;
	}

	// get list of filter wheel names
	result = SendCommand(hostname, 1, "fwlist", "Gui::LoadFwheelWidgets");
	if (result.empty()) {
		return;
	}
	FILE_LOG(logDEBUG) << "fwlist:" << result;

	// parse filter wheel names
	std::istringstream iss(result);
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
	connect(groupPressure, SIGNAL(toggled(bool)), this, SLOT(LoadPressureWidget()));
	connect(groupTube, SIGNAL(toggled(bool)), this, SLOT(LoadTubeWidget()));
}

void Gui::LoadPressureWidget(bool userClick) {
	std::string result = SendCommand(hostname, 1, "ispressure", "Gui::LoadPressureWidget");
    	if (!result.empty()) {
			if (result == "on") {
				EnablePressureWidget(true);
			} else {
				if (userClick) {
					Message(WARNING, "Vacuum pump is switched off.", "Gui::LoadPressureWidget");
				}
				EnablePressureWidget(false);
			}
    	}
}

void Gui::EnablePressureWidget(bool enable) {
	disconnect(groupPressure, SIGNAL(toggled(bool)), this, SLOT(LoadPressureWidget()));
	if (pgauge != NULL) {
		disconnect(pgauge, SIGNAL(SwitchedOffSignal(bool)), this, SLOT(EnablePressureWidget(bool)));
	}
	if (!enable) {
		if (pgauge != NULL) {
			gridPressure->removeWidget(pgauge);
			delete pgauge;
			pgauge = NULL;
			lblPressure->show();
		}
		groupPressure->setChecked(false);
	} else {
		if (pgauge == NULL) {
			lblPressure->hide();
			pgauge = new PGaugeWidget(this, hostname);
			gridPressure->addWidget(pgauge, 0, 0);	
		}
		pgauge->Update();
		groupPressure->setChecked(true);
	}
	connect(groupPressure, SIGNAL(toggled(bool)), this, SLOT(LoadPressureWidget()));
	if (pgauge != NULL) {
		connect(pgauge, SIGNAL(SwitchedOffSignal(bool)), this, SLOT(EnablePressureWidget(bool)));
	}
}

void Gui::LoadTubeWidget(bool userClick) {
	std::string result = SendCommand(hostname, 1, "istube", "Gui::LoadTubeWidget");
    	if (!result.empty()) {
			if (result == "on") {
				EnableTubeWidget(true);
			} else {
				if (userClick) {
					if (result == "standby") {
						Message(WARNING, "Xray Tube is on Stand-by.", "Gui::LoadTubeWidget");
					} else {
						Message(WARNING, "Xray Tube is switched off.", "Gui::LoadTubeWidget");
					}
				}
				EnableTubeWidget(false);
			}
    	}
}

void Gui::EnableTubeWidget(bool enable) {
	disconnect(groupTube, SIGNAL(toggled(bool)), this, SLOT(LoadTubeWidget()));
	if (tube != NULL) {
		disconnect(tube, SIGNAL(SwitchedOffSignal(bool)), this, SLOT(EnableTubeWidget(bool)));
	}
	if (!enable) {
		if (tube != NULL) {
			gridTube->removeWidget(tube);
			delete tube;
			tube = NULL;
			lblTube->show();
		}
		groupTube->setChecked(false);
	} else {
		if (tube == NULL) {
			lblTube->hide();
			tube = new TubeWidget(this, hostname);
			gridTube->addWidget(tube, 0, 0);	
		}
		tube->Update();
		groupTube->setChecked(true);
	}
	connect(groupTube, SIGNAL(toggled(bool)), this, SLOT(LoadTubeWidget()));
	if (tube != NULL) {
		connect(tube, SIGNAL(SwitchedOffSignal(bool)), this, SLOT(EnableTubeWidget(bool)));
	}
}

void Gui::Update() {
	if (!layoutDone) {
		return;
	}
	statusbar->showMessage("Updating ..."); // gets replaced by temp explanations of tool tip texts (not permanent)

	// update motors
	for (unsigned int i = 0; i < motorWidgets.size(); ++i) {
		motorWidgets[i]->Update();
		statusbar->showMessage("Updating ...");
	}
	// udpate reference points
	if (refpoints != NULL) {
		refpoints->Update();
		statusbar->showMessage("Updating ...");
	}	
	// udpate slits
	if (slits != NULL) {
		slits->Update();
		statusbar->showMessage("Updating ...");
	}
	// update fluorescence
	for (unsigned int i = 0; i < fluorWidgets.size(); ++i) {
		fluorWidgets[i]->Update();
		statusbar->showMessage("Updating ...");
	}	
	// update fwheels
	for (unsigned int i = 0; i < fwheelWidgets.size(); ++i) {
		fwheelWidgets[i]->Update();
		statusbar->showMessage("Updating ...");
	}		
	// udpate pressure gauge (only if enabled)
	if (groupPressure->isChecked()) {
		LoadPressureWidget(false);	
		statusbar->showMessage("Updating ...");
	}
	// udpate pressure gauge (only if enabled)
	if (groupTube->isChecked()) {
		LoadTubeWidget(false);	
		statusbar->showMessage("Updating ...");
	}

	statusbar->showMessage("Update completed", 2 * 1000);
}

void Gui::Stop() {
    pushStop->setChecked(true);
    FILE_LOG(logINFO) << "Stopping all motors";
    std::string result = SendCommand(hostname, 1, "stopall", "Gui::Stop");
    pushStop->setChecked(false);
	Update();
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




