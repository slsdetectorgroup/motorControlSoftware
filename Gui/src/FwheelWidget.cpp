/********************************************//**
 * @file FwheelWidget.cpp
 * @short uses the form for a Fwheel widget and defines its methods
 * @author Dhanya
 ***********************************************/
#include "FwheelWidget.h"
#include <iostream>
#include <sstream>
using namespace std;
#ifdef LASERBOX


int FwheelWidget::NumFwheelWidgets(0);

//-------------------------------------------------------------------------------------------------------------------------------------------------

FwheelWidget::FwheelWidget(QWidget *parent, const char *Name):QWidget(parent)
{
	if(strcmp(Name,"static"))
	{
		setupUi(this);
		strcpy(this->Name,Name);

		Initialization();
		UpdateValueFromServer();
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

char* FwheelWidget::GetName()
{
	return Name;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void FwheelWidget::Initialization()
{
	char command[200]="",cTemp[255]="";

	//initializations
	strcpy(this->Name,Name);
	fgroupbox->setTitle(Name);

	//get the values of the filter wheel
	fVals.resize(NumSlots);
	strcpy(command,"gui fvals ");
	strcat(command,this->Name);
	strcpy(cTemp,MotorWidget::SendCommand(3,command));
	istringstream sstr(cTemp);
	//store the values
	for(int j=0;j<NumSlots;j++)
	{
		if(sstr.good())
		{
			sstr>>cTemp;
			fVals[j]=atof(cTemp);
			fValue->addItem(QString(cTemp));
		}
	}

	UpdateValueFromServer();

	//connections
	connect(fValue,SIGNAL(currentIndexChanged(int)),this,SLOT(MoveFwheel(int)));

}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void FwheelWidget::UpdateValueFromServer()
{
	double val;
	bool found=false;
	int i;
	char message[200] = "gui fgetval ";
	strcat(message,Name);
	val=atof(MotorWidget::SendCommand(3,message));
	for(i=0;i< NumSlots;i++)
	{
		if(val==fVals[i])
		{
			found=true;
			break;
		}
	}
	if(!found)
	{
		MotorWidget::ErrorMessage((char*)"ERROR: Could not match value from server to the lists for filter wheel.\nThis should not happen.");
		exit(-1);
	}
	else
		fValue->setCurrentIndex(i);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void FwheelWidget::MoveFwheel(int index)
{
	char message[200]="gui fsetval ",cIndex[5]="";
	strcat(message,Name);
	strcat(message," ");
	sprintf(cIndex,"%f",fVals[index]);
	strcat(message,cIndex);

    strcpy(message,MotorWidget::SendCommand(4,message));

    if(strstr (message,"ERROR")!=NULL)
    	MotorWidget::ErrorMessage(message);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------
#endif
