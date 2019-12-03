/********************************************//**
 * @file PGaugeWidget.cpp
 * @short uses the form for a PGauge widget and defines its methods
 * @author Dhanya
 ***********************************************/
#include "PGaugeWidget.h"
#include "MotorWidget.h"
#include <iostream>
#include <string.h>
using namespace std;
#ifdef VACUUMBOX



//-------------------------------------------------------------------------------------------------------------------------------------------------

PGaugeWidget::PGaugeWidget(QWidget *parent):QWidget(parent), updating(false)
{
	setupUi(this);

    normal = pStatusDisplay->palette();
    red = new QPalette();
    red->setColor(QPalette::Text, Qt::red);

    connect(pushRefresh,      SIGNAL(clicked()),        this, SLOT(TimeoutRefresh()));
    connect(pushStop,      SIGNAL(clicked()),           this, SLOT(StopRefresh()));


    int ret = UpdateValueFromServer();

    spinUpdate->setValue(TIMEOUT/1000);
    timer = new QTimer(this);
    connect(timer,      SIGNAL(timeout()),              this, SLOT(TimeoutRefresh()));
    timer->start(TIMEOUT);
    if (!ret)
        StopRefresh();

}
//-------------------------------------------------------------------------------------------------------------------------------------------------

void PGaugeWidget::StopRefresh() {
    timer->stop();
    MotorWidget::ErrorMessage("Switching off automatic pressure update.\n");
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void PGaugeWidget::TimeoutRefresh() {
    if (updating)
        return;
    timer->stop();
    if (UpdateValueFromServer())
        timer->start(spinUpdate->value() * 1000);
    else
        MotorWidget::ErrorMessage("Switching off automatic pressure update.\n");
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

int PGaugeWidget::UpdateValueFromServer()
{
    updating = true;
    cout << "Updating Pressure from Server" << endl;

    const int size = 255;
    char command[size];
    memset(command, 0, size);
    char str[size];
    memset(str, 0, size);

    strcpy(command,"gui pressure ");
    strcpy(str,MotorWidget::SendCommand(2,command));

    // error
    if (strstr(str, "ERROR:") != NULL) {
        // red
        pStatusDisplay->setPalette(*red);
        pValueDisplay->setPalette(*red);
        pStatusDisplay2->setPalette(*red);
        pValueDisplay2->setPalette(*red);

        // read error
        if (strstr(str, "ERROR: Could not get pressure") != NULL) {
            pStatusDisplay->setText("READ ERROR");
            pValueDisplay->setText("READ ERROR");
            pStatusDisplay2->setText("READ ERROR");
            pValueDisplay2->setText("READ ERROR");
        } else {
            pStatusDisplay->setText("START ERROR");
            pValueDisplay->setText("START ERROR");
            pStatusDisplay2->setText("START ERROR");
            pValueDisplay2->setText("START ERROR");
        }
        cout << str << endl;
        gauge1box->setTitle("Pressure Gauge 1");
        gauge2box->setTitle("Pressure Gauge 2");
        updating = false;
        return 0;
    }

    // success
    pStatusDisplay->setPalette(normal);
    pValueDisplay->setPalette(normal);
    pStatusDisplay2->setPalette(normal);
    pValueDisplay2->setPalette(normal);

    char status1[20], status2[20];
    memset(status1, 0, 20);
    memset(status2, 0, 20);
    float value1 = -1, value2 = -1;

    const char delim[3] = ":,";
    char *token;
    token = strtok(str, delim);
    token = strtok(NULL, delim);
    strcpy(status1, token+1);
    token = strtok(NULL, delim);
    token = strtok(NULL, delim);
    sscanf(token, " %e]", &value1);
    token = strtok(NULL, delim);
    token = strtok(NULL, delim);
    strcpy(status2, token+1);
    token = strtok(NULL, delim);
    token = strtok(NULL, delim);
    sscanf(token, " %e]", &value2);

    printf("Gauge 1 [%s, %e], Gauge 2 [%s, %e]\n",status1, value1, status2, value2);


    pStatusDisplay->setText(QString(status1));
    pValueDisplay->setText(QString::number(value1, 'E', 5) + QString(" mbar"));
    pStatusDisplay2->setText(QString(status2));
    pValueDisplay2->setText(QString::number(value2, 'E', 5) + QString(" mbar"));

    if (strstr(status1,"Measurement data okay") == NULL) {
        pStatusDisplay->setPalette(*red);
        pValueDisplay->setPalette(*red);
    }
    if (strstr(status2,"Measurement data okay") == NULL) {
        pStatusDisplay2->setPalette(*red);
        pValueDisplay2->setPalette(*red);
    }

    updating = false;
    return 1;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------
#endif
