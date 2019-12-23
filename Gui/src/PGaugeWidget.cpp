#include "PGaugeWidget.h"
#include "GuiDefs.h"

#include <QTimer>
#include <sstream>
#include <iterator>
#include <vector>

#define TIMEOUT 5000

PGaugeWidget::PGaugeWidget(QWidget *parent, std::string hostname)
    :   QWidget(parent), hostname(hostname) {
	setupUi(this);
    LayoutWindow();
    Initialization();
}

PGaugeWidget::~PGaugeWidget() {}

void PGaugeWidget::LayoutWindow() {
    spinUpdate->setValue(TIMEOUT/1000);
    timer = new QTimer(this);
    chkRefresh->setChecked(true);
    pthread_mutex_init(&mutex, NULL);
}

void PGaugeWidget::Initialization() {
    connect(chkRefresh, SIGNAL(toggled(bool)), this, SLOT(EnableUpdate(bool)));
    connect(timer, SIGNAL(timeout()), this, SLOT(TimeoutRefresh()));
}

void PGaugeWidget::EnableUpdate(bool enable) {
    pthread_mutex_lock(&mutex);
    if (enable) {
        GetPressure();
    } else {
        if (timer->isActive()) {
            timer->stop();
            Message(WARNING, "Switching off automatic pressure update", "PGaugeWidget::EnableUpdate");
        }
    }
    pthread_mutex_unlock (&mutex);
}

void PGaugeWidget::TimeoutRefresh() {
    if (!pthread_mutex_trylock (&mutex)) {
        GetPressure();
        pthread_mutex_unlock (&mutex);
    }
}

void PGaugeWidget::GetPressure() {
    FILE_LOG(logDEBUG) << "Getting pressure from Server";

    // stop update temporarily
    timer->stop();

    try {
        std::pair <std::string, int> result = SendCommand(hostname, 1, "pressure ", "PGaugeWidget::GetPressure");
        std::string timestamp = GetTimeStamp();
        // error
        bool readError = false;
        if (result.first.empty()) {
            readError = true;       
        } else {
		//std::cout<<"result:"<<result.first<<std::endl;
            std::string pressure = result.first;
            size_t startPos = pressure.find('[');
            size_t endPos = pressure.find(']');
            size_t comma = pressure.find(',');       
            if (startPos == std::string::npos || endPos == std::string::npos 
                || comma == std::string::npos
                || comma < startPos || comma > endPos) {
                readError = true; 
            } else {
                pStatusDisplay->setText(pressure.substr(startPos + 1, comma - startPos - 1).c_str());
                pValueDisplay->setText((pressure.substr(comma + 2, endPos - comma - 2) + "  hPa").c_str());  
                size_t startPos2 = pressure.find('[', endPos + 1);
                size_t endPos2 = pressure.find(']', endPos + 1);
                size_t comma2 = pressure.find(',', endPos + 1);                   
                if (startPos2 == std::string::npos || endPos2 == std::string::npos 
                    || comma2 == std::string::npos
                    || comma2 < startPos2 || comma2 > endPos2) {
                    readError = true; 
                } else {         
                    pStatusDisplay2->setText(pressure.substr(startPos2 + 1, comma2 - startPos2 - 1).c_str());
                    pValueDisplay2->setText((pressure.substr(comma2 + 2, endPos2 - comma2 - 2) + "  hPa").c_str());  
                }
            }
        }
        if (readError) {
            pStatusDisplay->setText("Read Error");
            pValueDisplay->setText("Read Error");
            pStatusDisplay2->setText("Read Error");
            pValueDisplay2->setText("Read Error");                 
        } else {
            lblUpdate->setText(("Last update:   " + timestamp).c_str());
        }
        if (result.second) {
            emit UpdateSignal();
        }

    	// restart timer
    	if (chkRefresh->isChecked()) {
        	if (!timer->isActive()) {
            	timer->start(spinUpdate->value() * 1000);
        	}
    	}

    } catch (const PressureOffError& e) {
	    disconnect(chkRefresh, SIGNAL(toggled(bool)), this, SLOT(EnableUpdate(bool)));
        chkRefresh->setChecked(false); 
	    Message(WARNING, "Vacuum pump switched off. ", "PGaugeWidget::GetPressure");
	    connect(chkRefresh, SIGNAL(toggled(bool)), this, SLOT(EnableUpdate(bool)));
        pStatusDisplay->setText("Vacuum pump off");
        pValueDisplay->setText("Vacuum pump off");
        pStatusDisplay2->setText("Vacuum pump off");
        pValueDisplay2->setText("Vacuum pump off");
	    emit SwitchedOffSignal(false);
    }


}

std::string PGaugeWidget::GetTimeStamp() {
    FILE* sysFile = popen("date", "r");
	char output[COMMAND_BUFFER_LENGTH];
	memset(output, 0, sizeof(output));
	fgets(output, sizeof(output), sysFile);
	pclose(sysFile);
    std::string result(output, TIME_BUFFER_LENGTH);
    FILE_LOG(logDEBUG) << "Date:[" << result << ']';
    if (result.find("CET ") != std::string::npos) {
        result.erase (result.find("CET "), 4);
    }
    return result;
}


void PGaugeWidget::Update() {
    if (chkRefresh->isChecked()) {
        pthread_mutex_lock(&mutex);
        GetPressure();
        pthread_mutex_unlock (&mutex);
    }
}

