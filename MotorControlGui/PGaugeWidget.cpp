/********************************************//**
 * @file PGaugeWidget.cpp
 * @short uses the form for a PGauge widget and defines its methods
 * @author Dhanya
 ***********************************************/
#include "PGaugeWidget.h"
#include <iostream>
#include <string.h>
using namespace std;
#ifdef VACUUMBOX



//-------------------------------------------------------------------------------------------------------------------------------------------------

PGaugeWidget::PGaugeWidget(QWidget *parent):QWidget(parent)
{
	setupUi(this);

    gauge1box->setTitle("Gauge 1");
    gauge2box->setTitle("Gauge 2");

    normal = pStatusDisplay->palette();
    red = QPalette();
    red.setColor(QPalette::Active,QPalette::WindowText,Qt::red);

    UpdateValueFromServer();

    timer = new QTimer(this);
    connect(timer,   SIGNAL(timeout()),      this, SLOT(UpdateValueFromServer()));
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void PGaugeWidget::UpdateValueFromServer()
{
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
        pStatusDisplay->setPalette(red);
        pValueDisplay->setPalette(red);
        pStatusDisplay2->setPalette(red);
        pValueDisplay2->setPalette(red);

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
    } else {
        pStatusDisplay->setPalette(normal);
        pValueDisplay->setPalette(normal);
        pStatusDisplay2->setPalette(normal);
        pValueDisplay2->setPalette(normal);
    }


    // success
    char status1[20], status2[20];
    memset(status1, 0, 20);
    memset(status2, 0, 20);
    double value1 = -1, value2 = -1;
    sscanf(str,"Gauge 1 [Status: %s, Pressure: %e], Gauge 2 [Status: %s, Pressure: %e]\n",
            status1, &value1, status2, &value2);

    pStatusDisplay->setText(QString(status1));
    pValueDisplay->setText(QString::number(value1, 'E', 5));
    pStatusDisplay2->setText(QString(status2));
    pValueDisplay2->setText(QString::number(value2, 'E', 5));

    if (strstr(status1,"Measurement data okay") == NULL) {
        pStatusDisplay->setPalette(red);
        pValueDisplay->setPalette(red);
    }
    if (strstr(status2,"Measurement data okay") == NULL) {
        pStatusDisplay2->setPalette(red);
        pValueDisplay2->setPalette(red);
    }
}

//-------------------------------------------------------------------------------------------------------------------------------------------------
#endif
