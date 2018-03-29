/********************************************//**
 * @file MotorControlGui.cpp
 * @short defines the gui objects, signals and slots
 * @author Dhanya
 ***********************************************/
#include "MotorControlGui.h"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <time.h>
//#include <boost/algorithm/string.hpp>
using namespace std;

#ifdef XRAYBOX
#define MAXFL 7
#else
#define MAXFL 8
#endif

//-------------------------------------------------------------------------------------------------------------------------------------------------

MotorControlGui::MotorControlGui(int argc, char *argv[],QWidget *parent):QWidget(parent)
#ifndef LASERBOX
,oldActualV(0),oldActualC(0),actualV(0),actualC(0),wt(0),timeConnect(false),
actualConnect(false),doorConnect(false),startDrawing(false),clearDrawing(false),fluor(0),currentflist(0),slits(0),x1(-1),x2(-1),
huberPos(-9999),huberValue(0),xLower(-9999),xUpper(-9999),yLower(-9999),yUpper(-9999),pLower(-9999),pUpper(-9999)
#endif
{
	setupUi(this);


#ifdef XRAYBOX
	cout << " This is XRay Box Gui" << endl;

	shutter1->setEnabled(true);
	shutter2->setEnabled(false);
	shutter3->setEnabled(true);
	shutter4->setEnabled(false);

	shutter1->setText("Up");
	shutter2->setText("-");
	shutter3->setText("Right");
	shutter4->setText("-");
#elif LASERBOX
	cout << " This is Laser Box Gui" << endl;
#else
	cout << " This is Vacuum Box Gui" << endl;
	shutter1->setText("Up");
	shutter1->setEnabled(false);
	shutter2->setEnabled(true);
	shutter3->setEnabled(true);
	shutter4->setEnabled(true);

	shutter1->setText("-");
	shutter2->setText("Up");
	shutter3->setText("Right");
	shutter4->setText("Down");
#endif


#ifndef LASERBOX
	//fixing the motor names that can be shown on the gui
	drawingList.resize(MaxNumMotorWidgets+1);
	for(int i=0;i<MaxNumMotorWidgets+1;i++)
		drawingList[i].resize(1);
	drawingList[0].assign("Fluorescence");drawingList[1].assign("Crystal");drawingList[2].assign("Huber");
	drawingList[3].assign("Slit_x1");drawingList[4].assign("Slit_x2");drawingList[5].assign("PSF");
	drawingList[6].assign("Detector_x");drawingList[7].assign("Detector_y");
#endif

	//for static functions to emit signal for lockconflict
	MotorWidget::self=new MotorWidget(this,"static");
	connect(MotorWidget::self,SIGNAL(UpdateGUISignal()),this,SLOT(UpdateGUI()));

	//if given, change the server to connect to
	if(argc>1)
		for(int i=1;i<argc;i++)
			if(!strcasecmp(argv[i],"-server"))
			{
				cout<<"gui argv:"<<argv[i+1]<<endl;
				MotorWidget::ChangeHostname(argv[i+1]);
			}

	//Check if the server is locked by someone else
	// CheckIfBeingUsed();

	//set up gui and get the actual list of motors
	GettingListOfMotors();
	LayoutWindow();
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void MotorControlGui::GettingListOfMotors()
{

	char listOfMotors[255], charSize[1];

#ifndef LASERBOX
	char message[200] ;
#endif
	string motor="";
	int numMotors=0, controller=0, size=0;

	// list of all motor widgets' names, but fluorescence is hardcoded as 1.
	vector<string> list;
#ifndef LASERBOX
	list.resize(1);
	list[0]="Fluorescence";
#endif

	strcpy(listOfMotors,MotorWidget::SendCommand(2,"gui list"));

	istringstream sstr(listOfMotors);


	motors.resize(3);
	for(int i=0;i<3;i++)
	{
		motors[i].resize(4);
		motors[i][0].assign("0");
	}

	for(int j=0;j<(int)motors.size();j++)
		for(int k=1;k<(int)motors[j].size();k++)
			motors[j][k].assign("-");


	if(sstr.good())
	{
		sstr>>listOfMotors;
		numMotors = atoi(listOfMotors);
	}

	for(int i=0;i<numMotors;i++)
	{
		if(sstr.good())
		{
			sstr>>listOfMotors;
			controller = atoi(listOfMotors);
		}

#ifndef LASERBOX
		if(sstr.good())
		{
			sstr>>motor;
			//if(boost::iequals(motor, "detector_x"))
			if(!LowerCase(motor).compare("detector_x"))
			{
				strcpy(message,MotorWidget::SendCommand(3,"gui getLowerLimit detector_x"));
				xLower = atof(message);
				strcpy(message,MotorWidget::SendCommand(3,"gui getUpperLimit detector_x"));
				xUpper = atof(message);
			}
			//if(boost::iequals(motor, "detector_y"))
			if(!LowerCase(motor).compare("detector_y"))
			{
				strcpy(message,MotorWidget::SendCommand(3,"gui getLowerLimit detector_y"));
				yLower = atof(message);
				strcpy(message,MotorWidget::SendCommand(3,"gui getUpperLimit detector_y"));
				yUpper = atof(message);
			}
			//if(boost::iequals(motor, "psf"))
			if(!LowerCase(motor).compare("psf"))
			{
				strcpy(message,MotorWidget::SendCommand(3,"gui getLowerLimit psf"));
				pLower = atof(message);
				strcpy(message,MotorWidget::SendCommand(3,"gui getUpperLimit psf"));
				pUpper = atof(message);
			}

			//if(boost::iequals(motor, "fluorescence"))
			if(!LowerCase(motor).compare("fluorescence"))
				fluor=true;
			else
			{
				list.resize((int)list.size()+1);
				list[(int)list.size()-1]=motor;

				//saving the slitx1s position of the motorWidget[]
				//if(boost::iequals(motor, "slit_x1"))
				if(!LowerCase(motor).compare("slit_x1"))
				{
					slits=true;
					x1=(int)list.size()-1;
				}

				//saving the slitx2s position of the motorWidget[]
				//else if(boost::iequals(motor, "slit_x2"))
				else if(!LowerCase(motor).compare("slit_x2"))
				{
					slits=true;
					x2=(int)list.size()-1;
				}

				//saving the huber position of the motorWidget[]
				//else if(boost::iequals(motor, "huber"))
				else if(!LowerCase(motor).compare("huber"))
				{
					huberPos=(int)list.size()-1;
				}

			}
		}
#else
		//increase list size and add motor name
		if(sstr.good())
		{
			sstr>>motor;
			list.resize((int)list.size()+1);
			list[(int)list.size()-1]=motor;
		}

#endif

		//adding motors to the motor vector.
		switch(controller)
		{
		case 1:
			size = atoi(motors[0][0].c_str())+1;//get size, plus 1
			sprintf(charSize,"%d",size);
			motors[0][0].assign(charSize);//assign the new size
			motors[0][size].assign(motor);//assign motor name at that size column
			break;

		case 2:
			size = atoi(motors[1][0].c_str())+1;
			sprintf(charSize,"%d",size);
			motors[1][0].assign(charSize);
			motors[1][size].assign(motor);
			break;

		case 3:
			size = atoi(motors[2][0].c_str())+1;
			sprintf(charSize,"%d",size);
			motors[2][0].assign(charSize);
			motors[2][size].assign(motor);
			break;
		default:
			MotorWidget::ErrorMessage("ERROR: Too many corvus controllers in the config file");
			exit(-1);
		}

	}


#ifndef LASERBOX
	if(fluor) GetFluorValues();
#endif

	this->list=list;

#ifdef LASERBOX
	MotorWidget::NumMotorWidgets=(int)list.size();

	//--------filter wheel list------------------------------------

	// list of only filter wheel widgets' names
	vector<string> fwlist;

	strcpy(listOfMotors,MotorWidget::SendCommand(2,"gui fwlist"));
	istringstream ssstr(listOfMotors);

	//increase list size and add sArgName name
	while(ssstr.good())
	{
		ssstr>>motor;
		fwlist.resize((int)fwlist.size()+1);
		fwlist[(int)fwlist.size()-1]=motor;
	}

	this->fwlist=fwlist;
	FwheelWidget::NumFwheelWidgets=(int)fwlist.size();

	//--------references list------------------------------------

	// list of only references
	vector<string> reflist;

	strcpy(listOfMotors,MotorWidget::SendCommand(2,"gui reflist"));
	istringstream sssstr(listOfMotors);


	//increase list size and add motor name
	while(sssstr.good())
	{
		sssstr>>motor;
		reflist.resize((int)reflist.size()+1);
		reflist[(int)reflist.size()-1]=motor;
	}

	this->reflist=reflist;

	//------------------------------------------------------------
#endif
	/*
  // for debuging
     for(int i=0;i<(int)motors.size();i++)
     {
     for(int j=0;j<(int)motors[i].size();j++)
     cout<<":"<<motors[i][j];
     cout<<endl;
     }
	 */


}

#ifndef LASERBOX
//-------------------------------------------------------------------------------------------------------------------------------------------------

void MotorControlGui::GetFluorValues()
{
	string sArgName;
	char listOfFLMotors[200],message[200];

	//resize fl 2-d array
	fluorescence.resize(MAXFL);
	for(int i=0;i<(int)fluorescence.size();i++)
	{
		fluorescence[i].resize(3);
		for(int j=0;j<3;j++)
			fluorescence[i][j].assign("-");
	}

	//finding out which list is being populated
	strcpy(message,MotorWidget::SendCommand(2,"gui whichflist"));
	if(strstr (listOfFLMotors,"ERROR")!=NULL)
	{
		MotorWidget::ErrorMessage(listOfFLMotors);
		exit(-1);
	}
	currentflist = atoi(message);

	//getting the current list from server
	strcpy(listOfFLMotors,MotorWidget::SendCommand(2,"gui fllist"));
	if(strstr (listOfFLMotors,"ERROR")!=NULL)
	{
		MotorWidget::ErrorMessage(listOfFLMotors);
		exit(-1);
	}

	//splitting up the data from server to popoulate fl vectors
	istringstream sstr(listOfFLMotors);
	for(int i=0;i<MAXFL;i++)

		for(int j=0;j<3;j++)
		{
			if(sstr.good()) sstr>>sArgName;
			fluorescence[i][j].assign(sArgName);
		}

	// for debuging
	/*
  for(int i=0;i<(int)fluorescence.size();i++)
    {
      for(int j=0;j<(int)fluorescence[i].size();j++)
	cout<<":"<<fluorescence[i][j];
      cout<<endl;
    }
	 */

}
#endif
//-------------------------------------------------------------------------------------------------------------------------------------------------

void MotorControlGui::LayoutWindow()
{
	int size = (int)this->list.size();

	for(int i=1;i<MaxNumMotorWidgets;i++)
		motorWidgets[i]=NULL;

#ifndef LASERBOX
	int rowpos, pos;

	if(size > MaxNumMotorWidgets)
	{
		MotorWidget::ErrorMessage("ERROR: Too many motors in the config file to be displayed");
		exit(-1);
	}

	the_box->setStyleSheet("QGroupBox#the_box { font-weight:bold }");


	//for fluorescence
	if(!fluor)
	{
		fluorLabel->hide();
		fluorName->hide();
		energyDisplay->hide();
	}
	else
	{
#ifdef XRAYBOX
		fluorLabel->addItem("Fluor List 1");
		fluorLabel->addItem("Fluor List 2");
#else
		fluorLabel->addItem("Normal Fl List");
		fluorLabel->addItem("Reverse Fl List");
#endif
		if(!currentflist)
		{
			MotorWidget::ErrorMessage("ERROR: Fl exists in config file, but list to popoulate has not been noted");
			exit(-1);
		}
		fluorLabel->setCurrentIndex(currentflist-1);
		for(int i=0;i<(int)fluorescence.size();i++)
			fluorName->addItem(QString(fluorescence[i][0].c_str()));
		UpdateEnergyFromServer();
	}

	//the tabbed widget or the slits
	if(!slits)
		tabWidget->setEnabled(false);
	else
	{
		rowpos = 0;
		pos=x1;
		mainLayout = new QGridLayout(frame2);
		for(int i=0;i<2;i++)
		{
			motorWidgets[pos] = new MotorWidget(frame2,this->list[pos].c_str());
			mainLayout->addWidget(motorWidgets[pos],rowpos,0,1,75);
			rowpos+=1;
			pos=x2;
		}
		frame2->setLayout(mainLayout);
	}

#ifdef VACUUMBOX
            pGaugeWidget = new PGaugeWidget(frame1);
            QVBoxLayout* vboxLayout = new QVBoxLayout(frame1);
            vboxLayout->addWidget(pGaugeWidget,Qt::AlignCenter);
            frame1->setLayout(vboxLayout);
#else
	//other motor widgets and never include motorWidgets[0] as it is fluorescence by default
	int frame = 1;
	mainLayout = new QGridLayout(frame1);
	pos = 1;
	rowpos = 0;

	for(int i=1;i<size;i++)
	{
		if(pos==3)
		{

			if(frame == 1)
			{
				frame = 3;
				frame1->setLayout(mainLayout);
				mainLayout = new QGridLayout(frame3);
			}
			else if(frame == 3)
			{
				frame = 4;
				frame3->setLayout(mainLayout);
				mainLayout = new QGridLayout(frame4);
			}
			pos = 1;
			rowpos = 0;
		}

		if((i==x1)||(i==x2)) continue;
		if(frame == 1) motorWidgets[i] = new MotorWidget(frame1,this->list[i].c_str());
		else if(frame == 3) motorWidgets[i] = new MotorWidget(frame3,this->list[i].c_str());
		else if(frame == 4)motorWidgets[i] = new MotorWidget(frame4,this->list[i].c_str());
		mainLayout->addWidget(motorWidgets[i],rowpos,0,1,75);
		rowpos+=1;
		pos++;
	}
	if(pos<3)
	{
		if(frame == 1) frame1->setLayout(mainLayout);
		else if(frame == 3) frame3->setLayout(mainLayout);
		else if(frame == 4) frame4->setLayout(mainLayout);
	}
#endif
	//initializing qt widgets
	displayTime = new QTimer(this);
	displayActual = new QTimer(this);
	checkDoorTime = new QTimer(this);
	oldv =  new QPointF(140,130);
	oldc =  new QPointF(140,130);


	scene3 = new QGraphicsScene;
	scene3->setItemIndexMethod(QGraphicsScene::NoIndex);
	graph1->setScene(scene3);

	scene4 = new QGraphicsScene;
	scene4->setItemIndexMethod(QGraphicsScene::NoIndex);
	graph2->setScene(scene4);

	scene3->clear();
	scene4->clear();

	//warning button
	// warning->setIcon( QIcon( "images/warning.png" ) );
	warning->setIcon( QIcon ( ":/icons/images/warning.png" ) );
	warning->setIconSize(QSize(warning->size().width(), warning->size().height() ));
	warning->setEnabled(false);
	warningText->setEnabled(false);

	//xray tube layout
	xrayGroup->setStyleSheet("QGroupBox#xrayGroup { font-weight:bold }");

	//methods
	Initialization();
	if (slits)
	{
		GetSlitWidthFromServer();
		GetCenterFromServer();
	}

	//update xray tube status
	xrayGroup->setChecked(true);

#ifndef VACUUMBOX
	setWindowTitle(tr("XRay Box Gui - Motor Movements"));
#else
	setWindowTitle(tr("Vacuum Box Gui - Motor Movements"));
	if(!slits)
	    tabWidget->hide();
#endif
#else
	int fwsize = (int)this->fwlist.size();//, mWidth=0,mHeight=0;

	//check size of motors to be displayed
	if(size > MaxNumMotorWidgets)
	{
		MotorWidget::ErrorMessage("ERROR: Too many corvus motors in the config file to be displayed");
		exit(-1);
	}
	if(size<1)
	{
		MotorWidget::ErrorMessage("ERROR: There are no motors in the config file to be displayed");
		exit(-1);
	}
	if(fwsize > MaxNumFwheelWidgets)
	{
		MotorWidget::ErrorMessage("ERROR: Too many filter wheel motors in the config file to be displayed");
		exit(-1);
	}


	//stting the font to bold for group box
	the_box->setStyleSheet("QGroupBox#the_box { font-weight:bold }");
	the_box2->setStyleSheet("QGroupBox#the_box2 { font-weight:bold }");

	//adding the motor widgets
	mainLayout = new QVBoxLayout(frame1);
	for(int i=0;i<size;i++)
	{
		motorWidgets[i] = new MotorWidget(frame1,this->list[i].c_str());
		mainLayout->addWidget(motorWidgets[i],Qt::AlignCenter);
		/*		if(i==0)
		{
			mWidth=motorWidgets[i]->size().rwidth();
			mHeight=motorWidgets[i]->size().rheight();
		}*/
	}
	//frame1->setLayout(mainLayout);

	//TO ADD MORE MOTORS TO GUI, UNCOMMENT THIS PART AND ADD MORE SPACE TO GUI
	/*	for(int i=0;i<(MaxNumMotorWidgets-MotorWidget::NumMotorWidgets);i++)
		mainLayout->insertSpacerItem(-1,new QSpacerItem(mWidth,mHeight));*/

	//adding the filter wheel widgets
	mainLayout2 = new QVBoxLayout(frame2);
	//frame1->setLayout(mainLayout);

	for(int i=0;i<fwsize;i++)
	{
		fwheelWidgets[i] = new FwheelWidget(frame2,this->fwlist[i].c_str());
		mainLayout2->addWidget(fwheelWidgets[i],Qt::AlignCenter);
	}

	//adding reference points
	refValue->addItem("None");
	refValue->setCurrentIndex(0);
	if((int)reflist.size()>0)
	{
		for(int i=0;i<(int)reflist.size();i++)
			refValue->addItem(QString(reflist[i].c_str()));
		//check if the motors are in their reference points.
		CheckReference();
	}

	//methods
	Initialization();
	setWindowTitle(tr("Laser Box Gui - Motor Movements"));
#endif
}


#ifndef LASERBOX
//-------------------------------------------------------------------------------------------------------------------------------------------------

void MotorControlGui::paintEvent(QPaintEvent *)
{
	QString pointString;
	QTextStream stream(&pointString);
	QGraphicsTextItem* item,* item2,* item3,* item4,*limItem;
	QColor* color,* color2,* color3 = new QColor(Qt::darkBlue),*color4 = new QColor(Qt::red);
	double x,y,x2,y2,xvalue,xvalue2,yvalue,yvalue2,x1increment=0,x2increment=0;
	QPen *lines = new QPen(*color4);
	lines->setStyle(Qt::DotLine);
	QFont *limFont = new QFont(0,9,0,0), *graphFont = new QFont(0,10,0,0);

	//----------------------TOP VIEW----------------------------------------------------------

	QGraphicsScene* scene = new QGraphicsScene;
	scene->setSceneRect(QRectF(QPointF(-450,-210),QPointF(450,210)));
	scene->setItemIndexMethod(QGraphicsScene::NoIndex);
	topView->setScene(scene);


	//the small color indications on the top left corner
	QGraphicsRectItem *darkGray= new QGraphicsRectItem(QRectF(QPointF(-440,-200),QPointF(-435,-195)),0,scene);
	darkGray->setPen( QPen(Qt::darkGray  ) );
	darkGray->setBrush(Qt::darkGray);
	pointString="";
	stream <<"Connected but no movements in this view";
	item = scene->addText(pointString);
	item->setPos(-430, -210);
	item->setDefaultTextColor(Qt::darkGray);

	QGraphicsRectItem *lightGray= new QGraphicsRectItem(QRectF(QPointF(-440,-180),QPointF(-435,-175)),0,scene);
	lightGray->setPen( QPen(Qt::lightGray  ) );
	lightGray->setBrush(Qt::lightGray);
	pointString="";
	stream <<"Disconnected";
	item = scene->addText(pointString);
	item->setPos(-430, -190);
	item->setDefaultTextColor(Qt::lightGray);



	// drawing all the components in light gray first to show that they are all disconnected
	color = new QColor(Qt::lightGray);
	//just to know for future reference QGraphicsRectItem *boundary= new QGraphicsRectItem(QRectF(QPointF(-450,-210),QPointF(450,210)),0,scene);
	QGraphicsRectItem *hueber= new QGraphicsRectItem(QRectF(QPointF(-300/*-200*/,-125),QPointF(300,125)),0,scene);
	hueber->setPen( QPen( *color ) );
	QGraphicsRectItem *fluores= new QGraphicsRectItem(QRectF(QPointF(-250,-12.5),QPointF(-225,12.5)),0,scene);
	fluores->setPen( QPen( *color ) );
	fluores->setBrush(*color);
	QGraphicsEllipseItem *circle = new QGraphicsEllipseItem(QRectF(QPointF(-150,-25),QPointF(-100,25)),0,scene);
	circle->setPen( QPen( *color ) );
	circle->setBrush(*color);
	QGraphicsRectItem *crystal= new QGraphicsRectItem(QRectF(QPointF(-145,-5),QPointF(-105,5 )),0,scene);
	crystal->setPen( QPen( *color ) );
	crystal->setBrush(*color);
	//for slits--
	QGraphicsLineItem *line= new QGraphicsLineItem(QLineF(QPointF(-12.5,-76.25),QPointF(-12.5,76.25)),0,scene);
	QGraphicsLineItem *line2= new QGraphicsLineItem(QLineF(QPointF(-37.5,0),QPointF(12.5,0)),0,scene);
	QGraphicsLineItem *line3= new QGraphicsLineItem(QLineF(QPointF(-37.5,-76.25),QPointF(12.5,-76.25)),0,scene);
	QGraphicsLineItem *line4= new QGraphicsLineItem(QLineF(QPointF(-37.5,76.25),QPointF(12.5,76.25)),0,scene);
	line->setPen(QPen(Qt::white));
	line2->setPen(QPen(Qt::white));
	line3->setPen(QPen(Qt::white));
	line4->setPen(QPen(Qt::white));
	if(slits)
	{
		x1increment = motorWidgets[this->x1]->GetPosition();
		x2increment = motorWidgets[this->x2]->GetPosition();
		line->setPen( *lines );
		line2->setPen(  *lines );
		line3->setPen(  *lines );
		line4->setPen(  *lines );
		pointString="0";
		limItem = scene->addText(pointString);      limItem->setPos(-60,-80);
		limItem->setDefaultTextColor(*color4);  limItem->setFont(*limFont);
		limItem = scene->addText(pointString);      limItem->setPos(-60,63);
		limItem->setDefaultTextColor(*color4);  limItem->setFont(*limFont);
		double slitLimit = Limit/2;
		pointString=QString::number(slitLimit);
		limItem = scene->addText(pointString);      limItem->setPos(-70,-10);
		limItem->setDefaultTextColor(*color4);  limItem->setFont(*limFont);
	}
	QGraphicsRectItem *slit_x1= new QGraphicsRectItem(QRectF(QPointF(-25,-76.25+x1increment),QPointF(0,-51.25+x1increment)),0,scene);
	slit_x1->setPen( QPen( *color ) );
	slit_x1->setBrush(*color);
	QGraphicsRectItem *slit_x2= new QGraphicsRectItem(QRectF(QPointF(-25,51.25-x2increment),QPointF(0,76.25-x2increment)),0,scene);
	slit_x2->setPen( QPen( *color ) );
	slit_x2->setBrush(*color);
	//---
	QGraphicsRectItem *psf= new QGraphicsRectItem(QRectF(QPointF(25,-76.25),QPointF(50,76.25)),0,scene);
	psf->setPen( QPen( *color ) );
	psf->setBrush(*color);
	//for x detector--
	QGraphicsLineItem *xline= new QGraphicsLineItem(QLineF(QPointF(137.5,-76.25),QPointF(137.5,76.25)),0,scene);
	QGraphicsLineItem *xline2= new QGraphicsLineItem(QLineF(QPointF(112.5,0),QPointF(162.5,0)),0,scene);
	QGraphicsLineItem *xline3= new QGraphicsLineItem(QLineF(QPointF(112.5,-76.25),QPointF(162.5,-76.25)),0,scene);
	QGraphicsLineItem *xline4= new QGraphicsLineItem(QLineF(QPointF(112.5,76.25),QPointF(162.5,76.25)),0,scene);
	xline->setPen(  QPen(Qt::white));
	xline2->setPen( QPen(Qt::white));
	xline3->setPen( QPen(Qt::white));
	xline4->setPen( QPen(Qt::white));
	QGraphicsRectItem *det_x= new QGraphicsRectItem(QRectF(QPointF(125,-12.5),QPointF(150,12.5)),0,scene);
	det_x->setPen( QPen( *color ) );
	det_x->setBrush(*color);
	//----
	QGraphicsRectItem *det_y= new QGraphicsRectItem(QRectF(QPointF(200,-12.5),QPointF(225,12.5)),0,scene);
	det_y->setPen( QPen( *color ) );
	det_y->setBrush(*color);


	//xy coordinates has to be for hte same order as drawingList
	double xy[MaxNumMotorWidgets+1][4];

	//fluorescence=0
	xy[0][0]=-275;  xy[0][1]=-32.5;                 xy[0][2]=xy[0][0]; xy[0][3]=xy[0][1]-15;
	//Crystal=1
	xy[1][0]=-150;  xy[1][1]=-45;                   xy[1][2]=xy[1][0]; xy[1][3]=xy[1][1]-15;
	//Hueber=2  -200
	xy[2][0]=-300;  xy[2][1]=-145;                  xy[2][2]=xy[2][0]; xy[2][3]=xy[2][1]-15;
	//Slit_x1=3
	xy[3][0]=-37.5; xy[3][1]=-96.25+x1increment;    xy[3][2]=xy[3][0]; xy[3][3]=xy[3][1]-15;
	//Slit_x2=4
	xy[4][0]=-37.5; xy[4][1]=88.75-x2increment;      xy[4][2]=xy[4][0]; xy[4][3]=xy[4][1]-15;
	//PSF=5
	xy[5][0]=25;    xy[5][1]=-95;                    xy[5][2]=xy[5][0]; xy[5][3]=xy[5][1]-15;
	//Detector_x=6
	xy[6][0]=100;   xy[6][1]=-32.5;                  xy[6][2]=xy[6][0]; xy[6][3]=xy[6][1]-15;
	//Detector_y=7
	xy[7][0]=175;   xy[7][1]=-32.5;                  xy[7][2]=xy[7][0]; xy[7][3]=xy[7][1]-15;

	//----------------------FRONT VIEW----------------------------------------------------------

	QGraphicsScene* scene2 = new QGraphicsScene;
	scene2->setSceneRect(QRectF(QPointF(0,0),QPointF(900,210)));
	scene2->setItemIndexMethod(QGraphicsScene::NoIndex);
	frontView->setScene(scene2);


	QGraphicsRectItem *darkGray2= new QGraphicsRectItem(QRectF(QPointF(10,10),QPointF(15,15)),0,scene2);
	darkGray2->setPen( QPen(Qt::darkGray  ) );
	darkGray2->setBrush(Qt::darkGray);
	pointString="";
	stream <<"Connected but no movements in this view";
	item = scene2->addText(pointString);
	item->setPos(20, 0);
	item->setDefaultTextColor(Qt::darkGray);

	QGraphicsRectItem *lightGray2= new QGraphicsRectItem(QRectF(QPointF(10,30),QPointF(15,35)),0,scene2);
	lightGray2->setPen( QPen(Qt::lightGray  ) );
	lightGray2->setBrush(Qt::lightGray);
	pointString="";
	stream <<"Disconnected";
	item = scene2->addText(pointString);
	item->setPos(20,20);
	item->setDefaultTextColor(Qt::lightGray);

	color = new QColor(Qt::lightGray);
	//QGraphicsRectItem *boundary2= new QGraphicsRectItem(QRectF(QPointF(0,0),QPointF(900,210)),0,scene2);
	QGraphicsRectItem *hueber2= new QGraphicsRectItem(QRectF(QPointF(150,175),QPointF(750,200)),0,scene2);
	hueber2->setPen( QPen( *color ) );
	hueber2->setBrush(*color);
	QGraphicsRectItem *fluores2= new QGraphicsRectItem(QRectF(QPointF(200,85),QPointF(225,110)),0,scene2);
	fluores2->setPen( QPen( *color ) );
	fluores2->setBrush(*color);
	QGraphicsRectItem *crystal2= new QGraphicsRectItem(QRectF(QPointF(325,85),QPointF(350,110)),0,scene2);
	crystal2->setPen( QPen( *color ) );
	crystal2->setBrush(*color);
	QGraphicsRectItem *slit_x12= new QGraphicsRectItem(QRectF(QPointF(425,85),QPointF(450,110)),0,scene2);
	slit_x12->setPen( QPen( *color ) );
	slit_x12->setBrush(*color);
	QGraphicsRectItem *slit_x22= new QGraphicsRectItem(QRectF(QPointF(425,85),QPointF(450,110)),0,scene2);
	slit_x22->setPen( QPen( *color ) );
	slit_x22->setBrush(*color);
	//for psf--

	QGraphicsLineItem *pline= new QGraphicsLineItem(QLineF(QPointF(512.5,27.5),QPointF(512.5,167.5)),0,scene2);
	QGraphicsLineItem *pline2= new QGraphicsLineItem(QLineF(QPointF(487.5,97.5),QPointF(537.5,97.5)),0,scene2);
	QGraphicsLineItem *pline3= new QGraphicsLineItem(QLineF(QPointF(487.5,27.5),QPointF(537.5,27.5)),0,scene2);
	QGraphicsLineItem *pline4= new QGraphicsLineItem(QLineF(QPointF(487.5,167.5),QPointF(537.5,167.5)),0,scene2);
	pline->setPen(  QPen(Qt::white));
	pline2->setPen(  QPen(Qt::white));
	pline3->setPen(  QPen(Qt::white));
	pline4->setPen(  QPen(Qt::white));
	QGraphicsRectItem *psf2= new QGraphicsRectItem(QRectF(QPointF(500,85),QPointF(525,110)),0,scene2);
	psf2->setPen( QPen( *color ) );
	psf2->setBrush(*color);
	//----
	QGraphicsRectItem *det_x2= new QGraphicsRectItem(QRectF(QPointF(575,85),QPointF(600,110)),0,scene2);
	det_x2->setPen( QPen( *color ) );
	det_x2->setBrush(*color);
	//for y detector--
	QGraphicsLineItem *yline= new QGraphicsLineItem(QLineF(QPointF(662.5,27.5),QPointF(662.5,167.5)),0,scene2);
	QGraphicsLineItem *yline2= new QGraphicsLineItem(QLineF(QPointF(637.5,97.5),QPointF(687.5,97.5)),0,scene2);
	QGraphicsLineItem *yline3= new QGraphicsLineItem(QLineF(QPointF(637.5,27.5),QPointF(687.5,27.5)),0,scene2);
	QGraphicsLineItem *yline4= new QGraphicsLineItem(QLineF(QPointF(637.5,167.5),QPointF(687.5,167.5)),0,scene2);
	yline->setPen(  QPen(Qt::white));
	yline2->setPen(  QPen(Qt::white));
	yline3->setPen(  QPen(Qt::white));
	yline4->setPen(  QPen(Qt::white));
	QGraphicsRectItem *det_y2= new QGraphicsRectItem(QRectF(QPointF(650,85),QPointF(675,110)),0,scene2);
	det_y2->setPen( QPen( *color ) );
	det_y2->setBrush(*color);
	//---

	//xy coordinates has to be for hte same order as drawingList
	double xy2[MaxNumMotorWidgets+1][4];

	//fluorescence=0
	xy2[0][0]=175;   xy2[0][1]=65;    xy2[0][2]=xy2[0][0]; xy2[0][3]=xy2[0][1]-15;
	//Crystal=1
	xy2[1][0]=312.5; xy2[1][1]=65;    xy2[1][2]=xy2[1][0]; xy2[1][3]=xy2[1][1]-15;
	//Hueber=2
	xy2[2][0]=150;   xy2[2][1]=155;   xy2[2][2]=xy2[2][0]; xy2[2][3]=xy2[2][1]-15;
	//Slit_x1=3
	xy2[3][0]=412.5; xy2[3][1]=65;    xy2[3][2]=xy2[3][0]; xy2[3][3]=xy2[3][1]-15;
	//Slit_x2=4
	xy2[4][0]=412.5; xy2[4][1]=122.5; xy2[4][2]=xy2[4][0]; xy2[4][3]=xy2[4][1]-15;
	//PSF=5
	xy2[5][0]=500;   xy2[5][1]=65;    xy2[5][2]=xy2[5][0]; xy2[5][3]=xy2[5][1]-15;
	//Detector_x=6
	xy2[6][0]=550;   xy2[6][1]=65;    xy2[6][2]=xy2[6][0]; xy2[6][3]=xy2[6][1]-15;
	//Detector_y=7
	xy2[7][0]=625;   xy2[7][1]=65;    xy2[7][2]=xy2[7][0]; xy2[7][3]=xy2[7][1]-15;


	//----------------------IF THE MOTOR EXIST---------------------------------------------------------


	//get list of motors into a string
	string listOfMotors = "";
	if(fluor)
		listOfMotors.assign("Fluorescence");
	for(int i=1; i<(int)list.size();i++)
	{
		listOfMotors.append(" ");
		listOfMotors.append(list[i]);
	}



	//default if not in string, the color is light gray means disconnected
	for(int i=0; i<MaxNumMotorWidgets+1;i++)
	{
		if(listOfMotors.find(drawingList[i])==string::npos)
		{
			pointString="";
			if(drawingList[i].size()>=2)
			{
				stream <<QString::fromStdString(drawingList[i]);
				item = scene->addText(pointString);
				item->setPos(xy[i][0], xy[i][1]);
				item->setDefaultTextColor(*color);

				item2 = scene2->addText(pointString);
				item2->setPos(xy2[i][0], xy2[i][1]);
				item2->setDefaultTextColor(*color);
			}

		}
	}

	// fluorescence
	if(fluor)
	{
		pointString="";
		fluores->setPen( QPen(*color3) );
		fluores->setBrush(*color3 );
		stream <<"Fluorescence";
		item = scene->addText(pointString);
		item->setPos(xy[0][0], xy[0][1]);
		item->setDefaultTextColor(*color3 );

		fluores2->setPen( QPen( Qt::darkGray ) );
		fluores2->setBrush( Qt::darkGray);
		item2 = scene2->addText(pointString);
		item2->setPos(xy2[0][0], xy2[0][1]);
		item2->setDefaultTextColor(QColor( Qt::darkGray));

		pointString="";
		stream <<"("<<fluorName->currentText()<< ")";
		item = scene->addText(pointString);
		item->setPos(xy[0][2], xy[0][3]);
		item->setDefaultTextColor(*color3);

		item2 = scene2->addText(pointString);
		item2->setPos(xy2[0][2], xy2[0][3]);
		item2->setDefaultTextColor(QColor( Qt::darkGray));

	}

	//checks if the following motots exists in the config file
	for(int i=1; i<MaxNumMotorWidgets;i++)
	{
		int found=0;
		if(motorWidgets[i]!=NULL)
		{
			if(!strcasecmp(motorWidgets[i]->GetName(),"Huber"))
			{
				//topView->rotate(30);//5*motorWidgets[i]->GetPosition());
				color = color3;
				color2 = new QColor( Qt::darkGray);

				hueber->setPen( QPen( *color ) );
				hueber2->setPen( QPen( *color2 ) );
				hueber2->setBrush(  *color2 );

				//scene->setTransform(QTransform().translate(0, 0).rotate(motorWidgets[i]->GetPosition()).translate(0, 0));
				huberPos=i;


				x=xy[2][0]; y=xy[2][1];
				xvalue=xy[2][2]; yvalue=xy[2][3];

				x2=xy2[2][0]; y2=xy2[2][1];
				xvalue2=xy2[2][2]; yvalue2=xy2[2][3];
				found=1;
			}

			else if(!strcasecmp(motorWidgets[i]->GetName(),"Crystal"))
			{
				color = color3;
				color2 = new QColor( Qt::darkGray);

				circle->setPen( QPen( *color ) );
				circle->setBrush( *color  );
				crystal->setPen( QPen( Qt::white) );
				crystal->setBrush( Qt::white  );

				//rotate the crystal according to its value
				crystal->setTransform(QTransform().translate(-125, 0).rotate(motorWidgets[i]->GetPosition()).translate(125, 0));

				crystal2->setPen( QPen( *color2  ) );
				crystal2->setBrush(*color2  );


				x=xy[1][0]; y=xy[1][1];
				xvalue=xy[1][2]; yvalue=xy[1][3];

				x2=xy2[1][0]; y2=xy2[1][1];
				xvalue2=xy2[1][2]; yvalue2=xy2[1][3];
				found=1;
			}
			else if(!strcasecmp(motorWidgets[i]->GetName(),"Slit_x1"))
			{
				color = color3;
				color2 = new QColor( Qt::darkGray);

				slit_x1->setPen( QPen( *color ) );
				slit_x1->setBrush( *color );
				slit_x12->setPen( QPen( *color2 ) );
				slit_x12->setBrush(  *color2 );

				x=xy[3][0]; y=xy[3][1];
				xvalue=xy[3][2]; yvalue=xy[3][3];

				x2=xy2[3][0]; y2=xy2[3][1];
				xvalue2=xy2[3][2]; yvalue2=xy2[3][3];
				found=1;
			}
			else if(!strcasecmp(motorWidgets[i]->GetName(),"Slit_x2"))
			{
				color = color3;
				color2 = new QColor( Qt::darkGray);

				slit_x2->setPen( QPen( *color ) );
				slit_x2->setBrush( *color );
				slit_x22->setPen( QPen( *color2 ) );
				slit_x22->setBrush(  *color2 );

				x=xy[4][0]; y=xy[4][1];
				xvalue=xy[4][2]; yvalue=xy[4][3];

				x2=xy2[4][0]; y2=xy2[4][1];
				xvalue2=xy2[4][2]; yvalue2=xy2[4][3];
				found=1;
			}
			else if(!strcasecmp(motorWidgets[i]->GetName(),"PSF"))
			{
				color = new QColor( Qt::darkGray);
				color2 = color3;

				double pos = (167.5-97.5-12.5) - (((140-25)/(pUpper-pLower))*(motorWidgets[i]->GetPosition()-pLower));
				psf2->setY(pos);

				//displaying limits
				pointString="";
				stream<<pUpper;
				limItem = scene2->addText(pointString);      limItem->setPos(455,25);
				limItem->setDefaultTextColor(*color4);       limItem->setFont(*limFont);
				pointString="";
				stream<<pLower;
				int limposx;
				if(pLower==0)
					limposx=465;
				else
					limposx=455;
				limItem = scene2->addText(pointString);      limItem->setPos(limposx,153);
				limItem->setDefaultTextColor(*color4);       limItem->setFont(*limFont);


				psf->setPen( QPen( *color ) );
				psf->setBrush( *color );
				psf2->setPen( QPen( *color2 ) );
				psf2->setBrush(  *color2 );

				pline->setPen(  *lines );
				pline2->setPen( *lines );
				pline3->setPen( *lines );
				pline4->setPen( *lines );

				x=xy[5][0]; y=xy[5][1];
				xvalue=xy[5][2]; yvalue=xy[5][3];

				x2=xy2[5][0]; y2=xy2[5][1]+psf2->y();
				xvalue2=xy2[5][2]; yvalue2=xy2[5][3]+psf2->y();
				found=1;
			}
			else if(!strcasecmp(motorWidgets[i]->GetName(),"Detector_x"))
			{
				color = color3;
				color2 = new QColor( Qt::darkGray);

				double pos = (76.25-12.5) - (((152.5-25)/(xUpper-xLower))*(motorWidgets[i]->GetPosition()-xLower));
				det_x->setY(pos);

				//displaying limits
				pointString="";
				stream<<xUpper;
				limItem = scene->addText(pointString);      limItem->setPos(80,-80);
				limItem->setDefaultTextColor(*color4);      limItem->setFont(*limFont);
				pointString="";
				stream<<xLower;
				int limposx;
				if(xLower==0)
					limposx=90;
				else
					limposx=80;
				limItem = scene->addText(pointString);      limItem->setPos(limposx,63);
				limItem->setDefaultTextColor(*color4);      limItem->setFont(*limFont);

				det_x->setPen( QPen( *color ) );
				det_x->setBrush( *color );
				det_x2->setPen( QPen( *color2 ) );
				det_x2->setBrush(  *color2 );

				xline->setPen(  *lines );
				xline2->setPen( *lines );
				xline3->setPen( *lines );
				xline4->setPen( *lines );

				x=xy[6][0]; y=xy[6][1]+det_x->y();
				xvalue=xy[6][2]; yvalue=xy[6][3]+det_x->y();

				x2=xy2[6][0]; y2=xy2[6][1];
				xvalue2=xy2[6][2]; yvalue2=xy2[6][3];
				found=1;
			}
			else if(!strcasecmp(motorWidgets[i]->GetName(),"Detector_y"))
			{
				color = new QColor( Qt::darkGray);
				color2 = color3;
				double pos = -((97.5-27.5-12.5) - (((140-25)/(yUpper-yLower))*(motorWidgets[i]->GetPosition()-yLower)));
				det_y2->setY(pos);

				//displaying limits
				pointString="";
				stream<<yUpper;
				limItem = scene2->addText(pointString);      limItem->setPos(605,153);
				limItem->setDefaultTextColor(*color4);       limItem->setFont(*limFont);
				pointString="";
				stream<<yLower;
				int limposx;
				if(yLower==0)
					limposx=615;
				else
					limposx=605;
				limItem = scene2->addText(pointString);      limItem->setPos(limposx,25);
				limItem->setDefaultTextColor(*color4);  limItem->setFont(*limFont);

				det_y->setPen( QPen( *color ) );
				det_y->setBrush( *color );
				det_y2->setPen( QPen( *color2 ) );
				det_y2->setBrush(  *color2 );

				yline->setPen(  *lines );
				yline2->setPen( *lines );
				yline3->setPen( *lines );
				yline4->setPen( *lines );

				x=xy[7][0]; y=xy[7][1];
				xvalue=xy[7][2]; yvalue=xy[7][3];

				x2=xy2[7][0]; y2=xy2[7][1]+det_y2->y();
				xvalue2=xy2[7][2]; yvalue2=xy2[7][3]+det_y2->y();
				found=1;
			}
			if(found)
			{
				pointString="";
				stream << QVariant(motorWidgets[i]->GetName()).toString();
				item = scene->addText(pointString);
				item->setPos(x, y);
				item->setDefaultTextColor(*color);

				item2 = scene2->addText(pointString);
				item2->setPos(x2, y2);
				item2->setDefaultTextColor(*color2);

				//changing the text to value
				pointString="";
				stream <<"("<<QVariant(motorWidgets[i]->GetPosition()).toString()<< ")";
				item = scene->addText(pointString);
				item->setPos(xvalue, yvalue);
				item->setDefaultTextColor(*color);

				item2 = scene2->addText(pointString);
				item2->setPos(xvalue2, yvalue2);
				item2->setDefaultTextColor(*color2);
			}
		}
	}

	/*
  //for debugging
  //Populate the scene
  for(int x = -1000; x < 1000; x = x + 25) 
    {
      for(int y = -1000; y < 1000; y = y + 25) 
	{

	  if(x % 100 == 0 && y % 100 == 0) 
	    {
	      scene->addRect(x, y, 2, 2,QPen(( Qt::lightGray )));

	      QString pointString;
	      QTextStream stream(&pointString);
	      stream << "(" << x << "," << y << ")";
	      QGraphicsTextItem* item = scene->addText(pointString);
	      item->setPos(x, y);
	      item->setDefaultTextColor (QColor(Qt::lightGray) );
	    } 
	  else 
	    {
	      scene->addRect(x, y, 1, 1,QPen(( Qt::lightGray )));
	    }
	}
    }
	 */

	topView->centerOn(0,0);
	/*
  //checks if huber motor is connected and if the value is different from the previous value, rotate the top view
  if((huberPos!=-9999)&&(huberValue!=motorWidgets[huberPos]->GetPosition()))
    {
      huberValue = motorWidgets[huberPos]->GetPosition();
      topView->rotate(huberValue); 
      //fluores->rotate(-huberValue);
    }
	 */
	topView->show();


	/*
  //Populate the scene
  for(int x = 0; x < 900; x = x + 25) 
    {
      for(int y = 0; y < 210; y = y + 25) 
	{

	  if(x % 100 == 0 && y % 100 == 0) 
	    {
	      scene2->addRect(x, y, 2, 2,QPen(( Qt::lightGray )));

	      QString pointString;
	      QTextStream stream(&pointString);
	      stream << "(" << x << "," << y << ")";
	      QGraphicsTextItem* item = scene2->addText(pointString);
	      item->setPos(x, y);
	      item->setDefaultTextColor (QColor(Qt::lightGray) );
	    } 
	  else 
	    {
	      scene2->addRect(x, y, 1, 1,QPen(( Qt::lightGray )));
	    }
	}
    }
	 */

	frontView->centerOn(450,105);
	frontView->show();

	//---------------------FOR GRAPHS------------------------------------------

	color = new QColor(Qt::white);
	color2 = new QColor(Qt::blue);


	//this is kept in the beginning for times when hv is switched off while warmup
	//and clear drawing was set to true
	if(clearDrawing)
	{
		oldv =  new QPointF(140,130);
		oldc =  new QPointF(140,130);

		scene3->setSceneRect(QRectF(QPointF(oldv->x()-140,0),QPointF(oldv->x()+60,150)));
		scene4->setSceneRect(QRectF(QPointF(oldc->x()-140,0),QPointF(oldc->x()+60,150)));


		QGraphicsRectItem *clear1 = new QGraphicsRectItem(QRectF(QPointF(scene3->sceneRect().left()-5,-5),QPointF(scene3->sceneRect().right()+5,155)),0,scene3);
		QGraphicsRectItem *clear2 = new QGraphicsRectItem(QRectF(QPointF(scene4->sceneRect().left()-5,-5),QPointF(scene4->sceneRect().right()+5,155)),0,scene4);
		clear1->setPen( QPen( Qt::white ) );
		clear1->setBrush(  Qt::white );
		clear2->setPen( QPen(  Qt::white ) );
		clear2->setBrush(   Qt::white );

		//else the graph gets screwed up since nowhere is this reset
		oldActualC = 0;
		oldActualV = 0;

		clearDrawing  = false;
	}


	if((startDrawing)&&(warmupBox->isEnabled()))
		//if((startDrawing)&&(!Trycount))
	{

		cout<<"vn:"<<voltage<<"  cn:"<<current;
		cout<<"  v:"<<actualV<<"  c:"<<actualC<<endl;
		cout<<"vpos:"<<130-2*actualV<<"  cpos:"<<130-1.5*actualC<<endl;
		cout<<"oldv("<<oldv->x()<<","<<oldv->y()<<")";
		cout<<"   oldc("<<oldc->x()<<","<<oldc->y()<<")\n";
		cout<<"wt:"<<wt<<endl;

		if((oldActualC!=actualC)||(oldActualV!=actualV))
		{
			cout<<"gonna start drawing graph! \n\n";

			//set the scene each time..as the graph actually keeps moving to the right because wait time could be huge
			//remove these items as we dont want to see the previous lines when we draw the graph again
			scene3->setSceneRect(QRectF(QPointF(oldv->x()-140,0),QPointF(oldv->x()+60,150)));
			scene4->setSceneRect(QRectF(QPointF(oldc->x()-140,0),QPointF(oldc->x()+60,150)));


			//to clear out second weird lines which gets pasted after the scenerect is moved
			QGraphicsRectItem *clearSecondLines1 = new QGraphicsRectItem(QRectF(QPointF(oldv->x()+2,-2),QPointF(oldv->x()+35,129)),0,scene3);
			QGraphicsRectItem *clearSecondLines2 = new QGraphicsRectItem(QRectF(QPointF(oldc->x()+2,-2),QPointF(oldc->x()+35,129)),0,scene4);
			clearSecondLines1->setPen( QPen( *color ) );
			clearSecondLines1->setBrush(  *color );
			clearSecondLines2->setPen( QPen( *color ) );
			clearSecondLines2->setBrush(  *color );
			QGraphicsRectItem *clearSecondLines11 = new QGraphicsRectItem(QRectF(QPointF(oldv->x()+10,130),QPointF(oldv->x()+35,155)),0,scene3);
			QGraphicsRectItem *clearSecondLines22 = new QGraphicsRectItem(QRectF(QPointF(oldc->x()+10,130),QPointF(oldc->x()+35,155)),0,scene4);
			clearSecondLines11->setPen( QPen( *color ) );
			clearSecondLines11->setBrush(  *color );
			clearSecondLines22->setPen( QPen( *color ) );
			clearSecondLines22->setBrush(  *color );



			//axes and the gray lines
			QGraphicsLineItem *vh = new QGraphicsLineItem(QLineF(QPointF(oldv->x()-120,130),QPointF(oldv->x()+40,130)),0,scene3);
			QGraphicsLineItem *ch = new QGraphicsLineItem(QLineF(QPointF(oldc->x()-120,130),QPointF(oldc->x()+40,130)),0,scene4);
			QGraphicsLineItem *vv = new QGraphicsLineItem(QLineF(QPointF(oldv->x()-120,10),QPointF(oldv->x()-120,130)),0,scene3);
			QGraphicsLineItem *cv = new QGraphicsLineItem(QLineF(QPointF(oldc->x()-120,10),QPointF(oldc->x()-120,130)),0,scene4);

			QGraphicsLineItem *l1 = new QGraphicsLineItem(QLineF(QPointF(oldv->x()-120,10),QPointF(oldv->x()+40,10)),0,scene3);
			QGraphicsLineItem *l2 = new QGraphicsLineItem(QLineF(QPointF(oldv->x()-120,30),QPointF(oldv->x()+40,30)),0,scene3);
			QGraphicsLineItem *l3 = new QGraphicsLineItem(QLineF(QPointF(oldv->x()-120,50),QPointF(oldv->x()+40,50)),0,scene3);
			QGraphicsLineItem *l4 = new QGraphicsLineItem(QLineF(QPointF(oldv->x()-120,70),QPointF(oldv->x()+40,70)),0,scene3);
			QGraphicsLineItem *l5 = new QGraphicsLineItem(QLineF(QPointF(oldv->x()-120,90),QPointF(oldv->x()+40,90)),0,scene3);
			QGraphicsLineItem *l6 = new QGraphicsLineItem(QLineF(QPointF(oldv->x()-120,110),QPointF(oldv->x()+40,110)),0,scene3);
			QGraphicsLineItem *m1 = new QGraphicsLineItem(QLineF(QPointF(oldc->x()-120,10),QPointF(oldc->x()+40,10)),0,scene4);
			QGraphicsLineItem *m2 = new QGraphicsLineItem(QLineF(QPointF(oldc->x()-120,25),QPointF(oldc->x()+40,25)),0,scene4);
			QGraphicsLineItem *m3 = new QGraphicsLineItem(QLineF(QPointF(oldc->x()-120,40),QPointF(oldc->x()+40,40)),0,scene4);
			QGraphicsLineItem *m4 = new QGraphicsLineItem(QLineF(QPointF(oldc->x()-120,55),QPointF(oldc->x()+40,55)),0,scene4);
			QGraphicsLineItem *m5 = new QGraphicsLineItem(QLineF(QPointF(oldc->x()-120,70),QPointF(oldc->x()+40,70)),0,scene4);
			QGraphicsLineItem *m6 = new QGraphicsLineItem(QLineF(QPointF(oldc->x()-120,85),QPointF(oldc->x()+40,85)),0,scene4);
			QGraphicsLineItem *m7 = new QGraphicsLineItem(QLineF(QPointF(oldc->x()-120,100),QPointF(oldc->x()+40,100)),0,scene4);
			QGraphicsLineItem *m8 = new QGraphicsLineItem(QLineF(QPointF(oldc->x()-120,115),QPointF(oldc->x()+40,115)),0,scene4);
			QGraphicsLineItem *m9 = new QGraphicsLineItem(QLineF(QPointF(oldc->x()-120,130),QPointF(oldc->x()+40,130)),0,scene4);
			vh->setPen( QPen( *color2 ) );
			ch->setPen( QPen( *color2) );
			vv->setPen( QPen( *color2 ) );
			cv->setPen( QPen( *color2) );

			l1->setPen( QPen( Qt::DotLine ) );
			l2->setPen( QPen( Qt::DotLine ) );
			l3->setPen( QPen( Qt::DotLine ) );
			l4->setPen( QPen( Qt::DotLine ) );
			l5->setPen( QPen( Qt::DotLine ) );
			l6->setPen( QPen( Qt::DotLine ) );

			m1->setPen( QPen( Qt::DotLine ) );
			m2->setPen( QPen( Qt::DotLine ) );
			m3->setPen( QPen( Qt::DotLine ) );
			m4->setPen( QPen( Qt::DotLine ) );
			m5->setPen( QPen( Qt::DotLine ) );
			m6->setPen( QPen( Qt::DotLine ) );
			m7->setPen( QPen( Qt::DotLine ) );
			m8->setPen( QPen( Qt::DotLine ) );
			m9->setPen( QPen( Qt::DotLine ) );

			//voltage point rectangles
			scene3->addRect(oldv->x()-120,10,1,1,QPen(( Qt::darkBlue )));
			scene3->addRect(oldc->x()-120,30,1,1,QPen(( Qt::darkBlue )));
			scene3->addRect(oldc->x()-120,50,1,1,QPen(( Qt::darkBlue )));
			scene3->addRect(oldc->x()-120,70,1,1,QPen(( Qt::darkBlue )));
			scene3->addRect(oldc->x()-120,90,1,1,QPen(( Qt::darkBlue )));
			scene3->addRect(oldc->x()-120,110,1,1,QPen(( Qt::darkBlue )));
			scene3->addRect(oldc->x()-120,130,1,1,QPen(( Qt::darkBlue )));

			// current point rectangles
			scene4->addRect(oldc->x()-120,10,1,1,QPen(( Qt::darkBlue )));
			scene4->addRect(oldc->x()-120,25,1,1,QPen(( Qt::darkBlue )));
			scene4->addRect(oldc->x()-120,40,1,1,QPen(( Qt::darkBlue )));
			scene4->addRect(oldc->x()-120,55,1,1,QPen(( Qt::darkBlue )));
			scene4->addRect(oldc->x()-120,70,1,1,QPen(( Qt::darkBlue )));
			scene4->addRect(oldc->x()-120,85,1,1,QPen(( Qt::darkBlue )));
			scene4->addRect(oldc->x()-120,100,1,1,QPen(( Qt::darkBlue )));
			scene4->addRect(oldc->x()-120,115,1,1,QPen(( Qt::darkBlue )));
			scene4->addRect(oldc->x()-120,130,1,1,QPen(( Qt::darkBlue )));

			//to draw the graph lines
			cout<<"started va line\n";
			QGraphicsLineItem *va = new QGraphicsLineItem(QLineF(*oldv,QPointF(oldv->x()+30,130-2*actualV)),0,scene3);
			QGraphicsLineItem *ca = new QGraphicsLineItem(QLineF(*oldc,QPointF(oldc->x()+30,130-1.5*actualC)),0,scene4);
			cout<<"done drawing line\n";
			cout<<"------va:("<<oldv->x()<<","<<oldv->y()<<") to ("<<oldv->x()+30<<","<<130-2*actualV<<")\n";
			va->setPen( QPen( *color2 ) );
			ca->setPen( QPen( *color2) );
			scene3->addRect(oldv->x()+30-1,130-2*actualV-1, 2, 2,QPen(( Qt::darkBlue )));
			scene4->addRect(oldc->x()+30-1,130-1.5*actualC-1, 2, 2,QPen(( Qt::darkBlue )));

			//for the wait time axis
			QGraphicsLineItem *vw = new QGraphicsLineItem(QLineF(QPointF(oldv->x()+30,10),QPointF(oldv->x()+30,130)),0,scene3);
			QGraphicsLineItem *cw = new QGraphicsLineItem(QLineF(QPointF(oldc->x()+30,10),QPointF(oldc->x()+30,130)),0,scene4);
			vw->setPen( QPen(Qt::DotLine) );
			cw->setPen( QPen(Qt::DotLine) );
			scene3->addRect(oldv->x()+30,130, 1, 1,QPen(( Qt::darkBlue )));
			scene4->addRect(oldc->x()+30,130, 1, 1,QPen(( Qt::darkBlue )));

			int wtInMin = wt/60;
			qreal wtX,wtY;
			pointString="";
			stream<<wtInMin;
			if(wtInMin<10)
			{
				wtX = oldv->x()+23;
				wtY = oldc->x()+23;
			}
			else if(wtInMin>99)
			{
				wtX = oldv->x()+18;
				wtY = oldc->x()+18;
			}
			else
			{
				wtX = oldv->x()+20;
				wtY = oldc->x()+20;
			}
			item3 = scene3->addText(pointString); item3->setPos(wtX,130); item3->setDefaultTextColor(*color2);  item3->setFont(*graphFont);
			item4 = scene4->addText(pointString); item4->setPos(wtY,130); item4->setDefaultTextColor(*color2);  item3->setFont(*graphFont);

			//to clear the stuff that goes beyong the axis
			QGraphicsRectItem *boundaryg1 = new QGraphicsRectItem(QRectF(QPointF(scene3->sceneRect().left()-5,0),QPointF(scene3->sceneRect().left()+18,130)),0,scene3);
			QGraphicsRectItem *boundaryg11 = new QGraphicsRectItem(QRectF(QPointF(scene3->sceneRect().left()-5,131),QPointF(scene3->sceneRect().left()+40,150)),0,scene3);

			QGraphicsRectItem *boundaryg2 = new QGraphicsRectItem(QRectF(QPointF(scene4->sceneRect().left()-5,0),QPointF(scene4->sceneRect().left()+18,130)),0,scene4);
			QGraphicsRectItem *boundaryg22 = new QGraphicsRectItem(QRectF(QPointF(scene4->sceneRect().left()-5,131),QPointF(scene4->sceneRect().left()+40,150)),0,scene4);

			boundaryg1->setPen( QPen( *color ) );
			boundaryg1->setBrush(  *color );
			boundaryg2->setPen( QPen( *color ) );
			boundaryg2->setBrush(  *color );
			boundaryg11->setPen( QPen( *color ) );
			boundaryg11->setBrush(  *color );
			boundaryg22->setPen( QPen( *color ) );
			boundaryg22->setBrush(  *color );




			//marking the voltage and current values in graphs
			pointString="80";
			item4 = scene4->addText(pointString); item4->setPos(oldc->x()-145,10-10); item4->setDefaultTextColor(*color2);  item4->setFont(*graphFont);
			pointString="70";
			item4 = scene4->addText(pointString); item4->setPos(oldc->x()-145,25-10); item4->setDefaultTextColor(*color2);  item4->setFont(*graphFont);

			pointString="60";
			item3 = scene3->addText(pointString); item3->setPos(oldv->x()-145,10-10); item3->setDefaultTextColor(*color2);  item3->setFont(*graphFont);
			item4 = scene4->addText(pointString); item4->setPos(oldc->x()-145,40-10); item4->setDefaultTextColor(*color2);  item4->setFont(*graphFont);
			pointString="50";
			item3 = scene3->addText(pointString); item3->setPos(oldv->x()-145,30-10); item3->setDefaultTextColor(*color2);  item3->setFont(*graphFont);
			item4 = scene4->addText(pointString); item4->setPos(oldc->x()-145,55-10); item4->setDefaultTextColor(*color2);  item4->setFont(*graphFont);
			pointString="40";
			item3 = scene3->addText(pointString); item3->setPos(oldv->x()-145,50-10); item3->setDefaultTextColor(*color2);  item3->setFont(*graphFont);
			item4 = scene4->addText(pointString); item4->setPos(oldc->x()-145,70-10); item4->setDefaultTextColor(*color2);  item4->setFont(*graphFont);
			pointString="30";
			item3 = scene3->addText(pointString); item3->setPos(oldv->x()-145,70-10); item3->setDefaultTextColor(*color2);  item3->setFont(*graphFont);
			item4 = scene4->addText(pointString); item4->setPos(oldc->x()-145,85-10); item4->setDefaultTextColor(*color2);  item4->setFont(*graphFont);
			pointString="20";
			item3 = scene3->addText(pointString); item3->setPos(oldv->x()-145,90-10); item3->setDefaultTextColor(*color2);  item3->setFont(*graphFont);
			item4 = scene4->addText(pointString); item4->setPos(oldc->x()-145,100-10); item4->setDefaultTextColor(*color2); item4->setFont(*graphFont);
			pointString="10";
			item3 = scene3->addText(pointString); item3->setPos(oldv->x()-145,110-10); item3->setDefaultTextColor(*color2); item3->setFont(*graphFont);
			item4 = scene4->addText(pointString); item4->setPos(oldc->x()-145,115-10); item4->setDefaultTextColor(*color2); item4->setFont(*graphFont);
			pointString="0";
			item3 = scene3->addText(pointString); item3->setPos(oldv->x()-140,130-10); item3->setDefaultTextColor(*color2); item3->setFont(*graphFont);
			item4 = scene4->addText(pointString); item4->setPos(oldc->x()-140,130-10); item4->setDefaultTextColor(*color2); item4->setFont(*graphFont);

			//setting the old value variables to the new values fr the next round
			oldv =  new QPointF(oldv->x()+30,130-2*actualV);
			oldc =  new QPointF(oldc->x()+30,130-1.5*actualC);
			oldActualC = actualC;
			oldActualV = actualV;
			//Trycount++;
		}

	}


	//always center on where wt is
	graph1->centerOn(oldv->x()-40,75);
	graph1->show();
	graph2->centerOn(oldc->x()-40,75);
	graph2->show();


}
#endif

//-------------------------------------------------------------------------------------------------------------------------------------------------

void MotorControlGui::Initialization()
{

#ifndef LASERBOX
	//relative displays
	QDoubleValidator *noNeg = new QDoubleValidator(centerRelDisplay);
	noNeg->setBottom(0);

	centerRelDisplay->setText("1.000");
	centerRelDisplay->setValidator(noNeg);

	widthRelDisplay->setText("1.000");
	widthRelDisplay->setValidator(noNeg);

	//absolute displays
	centerAbsDisplay->setText("");
	centerAbsDisplay->setValidator(noNeg);

	widthAbsDisplay->setText("");
	widthAbsDisplay->setValidator(noNeg);

	//xraytube
	QIntValidator *volt = new QIntValidator(0,60,setV);
	setV->setText("");
	setV->setValidator(volt);
	QIntValidator *curr = new QIntValidator(0,80,setC);
	setC->setText("");
	setC->setValidator(curr);
	//connections


	if(slits)
	{
		//center connections
		connect(exactCenter,SIGNAL(clicked()),this,SLOT(SlitsToCenter()));
		connect(centerMinus,SIGNAL(clicked()),this,SLOT(CenterRelative()));
		connect(centerPlus,SIGNAL(clicked()),this,SLOT(CenterRelative()));

		//slitwidth connections
		connect(zeroWidth,SIGNAL(clicked()),this,SLOT(SlitsZeroWidth()));
		connect(widthMinus,SIGNAL(clicked()),this,SLOT(WidthRelative()));
		connect(widthPlus,SIGNAL(clicked()),this,SLOT(WidthRelative()));


		connect(motorWidgets[x1],SIGNAL(SlitsMoved()),this,SLOT(UpdateSlitWidth()));
		connect(motorWidgets[x2],SIGNAL(SlitsMoved()),this,SLOT(UpdateSlitWidth()));
		connect(motorWidgets[x1],SIGNAL(SlitsMoved()),this,SLOT(UpdateCenter()));
		connect(motorWidgets[x2],SIGNAL(SlitsMoved()),this,SLOT(UpdateCenter()));
	}

	//for relative movt
	for(int i=1;i<MaxNumMotorWidgets;i++)
		if(motorWidgets[i]!=NULL)
			connect(motorWidgets[i],SIGNAL(RelativeMoved()),this,SLOT(update()));

	//connect for fluorescense
	connect(fluorLabel,SIGNAL(currentIndexChanged(int)),this,SLOT(ChangeFluorescenceList(int)));
	connect(fluorName,SIGNAL(currentIndexChanged(int)),this,SLOT(MoveFluorescence(int)));


	//xrayGroup Box
	connect(xrayGroup,SIGNAL(toggled(bool)), this,SLOT(UpdateXrayStatus()));
	//high voltage
	connect(highVoltage,SIGNAL(toggled(bool)), this,SLOT(UpdateXrayGroupforHV()));
	//shutters
	connect(shutter1,SIGNAL(stateChanged(int)), this,SLOT(UpdateXrayGroupforShutters()));
	connect(shutter2,SIGNAL(stateChanged(int)), this,SLOT(UpdateXrayGroupforShutters()));
	connect(shutter3,SIGNAL(stateChanged(int)), this,SLOT(UpdateXrayGroupforShutters()));
	connect(shutter4,SIGNAL(stateChanged(int)), this,SLOT(UpdateXrayGroupforShutters()));
	//setvoltage
	connect(setVoltage,SIGNAL(clicked()), this,SLOT(UpdateCurrentOrVoltage()));
	//setcurrent
	connect(setCurrent,SIGNAL(clicked()), this,SLOT(UpdateCurrentOrVoltage()));
	//get error code
	connect(getErrorCode,SIGNAL(clicked()), this,SLOT(GetErrorCodeFromTube()));

#else
	//for relative movt
	for(int i=0;i<MotorWidget::NumMotorWidgets;i++)
		connect(motorWidgets[i],SIGNAL(RelativeMoved()),this,SLOT(CheckReference()));

	//the references
	connect(refValue,SIGNAL(currentIndexChanged(int)),this,SLOT(MoveToReference(int)));
#endif

	//update all the positions
	connect(updateAll,SIGNAL(clicked()),this,SLOT(UpdateGUI()));


	//options menu
	connect(options,SIGNAL(clicked()), this,SLOT(OptionsMenu()));

}


#ifndef LASERBOX
//-------------------------------------------------------------------------------------------------------------------------------------------------

void  MotorControlGui::UpdateEnergyFromServer()
{
	char message[200] = "gui pos fluorescence";
	double pos;
	char cDisplay[200];

	strcpy(message,MotorWidget::SendCommand(3,message));
	if(strstr (message,"ERROR")!=NULL)
		MotorWidget::ErrorMessage(message);
	else
	{
		pos = atof(message);

		int found =0;
		for(int i=0;i<(int)fluorescence.size();i++)
			//fabs takes the absolute value..and then the difference is taken so that the exact precision can b ignored
			if(fabs(pos-i*fluorWidth) < 0.0000001 )
			{
				sprintf(cDisplay,"%s KeV",fluorescence[i][1].c_str());
				energyDisplay->setText(QString(cDisplay));
				fluorName->setCurrentIndex(i);
				found=1;
				break;
			}
		//if its not in the exact range
		if(!found)
		{
			fluorName->addItem("None");
			fluorName->setCurrentIndex((int)fluorescence.size());
			energyDisplay->setText("");
		}

	}

	update();
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void MotorControlGui::UpdateHVFromServer()
{
	char message[200];
	strcpy(message,MotorWidget::SendCommand(2,"gui gethv "));
	if(strstr (message,"No")!=NULL)
		highVoltage->setChecked(false);
	else
		highVoltage->setChecked(true);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void MotorControlGui::UpdateShuttersFromServer()
{
	char message[200];

	strcpy(message,MotorWidget::SendCommand(2,"gui getshutters "));

	if(strstr (message,"1:ON")!=NULL)
		shutter1->setChecked(true);
	else
		shutter1->setChecked(false);

	if(strstr (message,"2:ON")!=NULL)
		shutter2->setChecked(true);
	else
		shutter2->setChecked(false);

	if(strstr (message,"3:ON")!=NULL)
		shutter3->setChecked(true);
	else
		shutter3->setChecked(false);

	if(strstr (message,"4:ON")!=NULL)
		shutter4->setChecked(true);
	else
		shutter4->setChecked(false);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void MotorControlGui::CheckWarmupTime()
{
	cout<<"in warmuptime()\n";

	wt = atoi(MotorWidget::SendCommand(2,"gui getwtime"));
	//check wt many times till u think it might be the right one
	while(wt<0)
		wt = atoi(MotorWidget::SendCommand(2,"gui getwtime"));

	if(wt==0)
		wt = atoi(MotorWidget::SendCommand(2,"gui getwtime"));
	cout<<"\nwt:"<<wt<<endl;
	//do something with the wt obtained; either draw graph or gethv and shutters from server
	if(wt>0)
	{
		MotorWidget::ErrorMessage("Warm-up of X-Ray Tube is in progress...");
		warmupBox->setEnabled(true);
		highVoltage->setChecked(true);
		highVoltage->setStyleSheet("background: red");
		//disabling the set voltage,current, shutters and hv buttons
		setVoltage->setEnabled(false);
		setV->setEnabled(false);
		setCurrent->setEnabled(false);
		setC->setEnabled(false);
		unit1->setEnabled(false);
		unit2->setEnabled(false);
		shutterBox->setEnabled(false);
		//start drawing the graph
		ShowWarmupGraph();
	}
	else
	{
		warmupBox->setEnabled(false);
		UpdateHVFromServer();
		UpdateShuttersFromServer();

		UpdateXrayGroupforHV();
		UpdateXrayGroupforShutters();

		if(highVoltage->isChecked())
		{
			MotorWidget::ErrorMessage("WARNING: Leaving High Voltage ON");
			UpdateActualValues();
		}


	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

//gotta make this a returning function to be able to exit from it if someone doesnt want a warm up
//need to update comment of this cfunction in the h file.  
void MotorControlGui::CheckWarmupRequired()
{
	cout<<"in checkwarmuprequired()\n";

	char message[200],eMess[200],warmupTiming[200];

	int value =  atoi(MotorWidget::SendCommand(2,"gui geterr"));
	cout<<"value="<<value<<endl;

	if ((value==70)||(value==63)||(value==64)||(value==65))
	{
		MotorWidget::SendCommand(2,"gui clear ");

		//setting the voltage and current again -  to get the error again since we cleared the error for 70, 63, 64, 65
		sprintf(message,"gui setv %d",voltage);
		MotorWidget::SendCommand(3,message);
		sprintf(message,"gui setc %d",current);
		MotorWidget::SendCommand(3,message);
	}

	//to switch on hv if hv was off to get the error msg
	strcpy(message,MotorWidget::SendCommand(2,"gui gethv "));
	if(strstr (message,"No")!=NULL)
		MotorWidget::SendCommand(3,"gui hv on ");
	value =  atoi(MotorWidget::SendCommand(2,"gui geterr"));

	cout<<"value inside warmup function after hv on="<<value<<endl;
	if(value==0)
	{
		highVoltage->setStyleSheet("background: red");
		UpdateActualValues();
		//shutterBox->setEnabled(true);
	}
	else if(value==116)
	{
		MotorWidget::ErrorMessage("ERROR: An error of 116.Please consult technician.");
		xrayGroup->setTitle("X-Ray Tube ERROR: sr:12 is 116");
		highVoltage->setChecked(false);
		actualVDisp->setText("");
		actualCDisp->setText("");
		xrayGroup->setChecked(false);
	}
	else
	{

		if ((value!=106)&&(value!=109)) //((value==70)||(value==63)||(value==64)||(value==65))
		{
			strcpy(eMess,MotorWidget::SendCommand(2,"gui getemess "));
			sprintf(message,"ERROR: An error of %d:",value);
			strcat(message,eMess);
			strcat(message,"Please tell Dhanya of this error and how you got it. Thanks!\n\nDisabling Xray tube part of the GUI");
			MotorWidget::ErrorMessage(message);

			sprintf(message,"X-Ray Tube ERROR: sr:12 is %d", value);
			xrayGroup->setTitle(message);
			highVoltage->setChecked(false);
			MotorWidget::SendCommand(2,"gui clear");
			MotorWidget::SendCommand(2,"gui clear");
			actualVDisp->setText("");
			actualCDisp->setText("");
			xrayGroup->setChecked(false);
		}
		else
		{
			cout<<"voltage to be warmed up to="<<voltage<<endl;
			//should be only for errors 106 and 109. asking the user to confirm warmup
			//also getting the last warmup time stamp
			sprintf(message,"gui readwarmuptiming %d",voltage);
			strcpy(warmupTiming,MotorWidget::SendCommand(3,message));
			if(strstr (warmupTiming,"ERROR")!=NULL)
			{
				MotorWidget::ErrorMessage(warmupTiming);
				strcpy(warmupTiming,"unknown");
			}

			strcpy(message,"The X-Ray Tube needs to be warmed-up. The last successful warm-up at this voltage was at ");
			strcat(message,warmupTiming);
			strcat(message,"\n\tNOTE: Skipping warmup to go directly to desired voltage \n\tis possible only manually at the tube");
			strcat(message,"\nInitiate Warm-Up?");

			//      sprintf(message,"The X-Ray Tube needs to be warmed-up. The last successful warm-up at this voltage was at %s.\n Initiate Warm-Up?",warmupTiming);
			int ret = QMessageBox::warning(this, "X-Ray tube Warm-up", message, QMessageBox::Ok | QMessageBox::Cancel);

			//if the user canceled the warm up, then it displays an warning message and reverts to last operating values.
			if (ret == QMessageBox::Cancel)
			{
				MotorWidget::ErrorMessage("WARNING:You have chosen not to warm-up. Reverting to last operating voltage and current.");
				MotorWidget::SendCommand(2,"gui clear ");
				MotorWidget::SendCommand(2,"gui geterr ");
				MotorWidget::SendCommand(2,"gui geterr "); //to clear the -9999 error

				//reverting the voltage values
				voltage = atoi(MotorWidget::SendCommand(2,"gui getv"))/1000;
				current = atoi(MotorWidget::SendCommand(2,"gui getc"))/1000;
				setVDisp->setText(QString::number(voltage));
				setCDisp->setText(QString::number(current));

				//to check if the hv is really switched on
				strcpy(message,MotorWidget::SendCommand(2,"gui gethv "));
				if(strstr (message,"No")==NULL)
				{
					highVoltage->setStyleSheet("background: red");
					UpdateActualValues();
					//shutterBox->setEnabled(true);
				}
				else
					highVoltage->setChecked(false);

			}
			else
			{ //warm up process

				//switch off hv to start warmup, else hv switches off with the warmup cmd
				//strcpy(message,MotorWidget::SendCommand(2,"gui gethv "));
				// if(strstr (message,"No")==NULL)
				MotorWidget::SendCommand(3,"gui hv off ");


				int value =  atoi(MotorWidget::SendCommand(2,"gui geterr"));
				if (value==-9999)
					value =  atoi(MotorWidget::SendCommand(2,"gui geterr"));
				cout<<"value after switching off before warmup="<<value<<endl;
				cout<<"voltage from server:"<<(atoi(MotorWidget::SendCommand(2,"gui getv"))/1000)<<endl;


				//clearing the error since 70 error comes back wen u do hv off
				if ((value==70)||(value==63)||(value==64)||(value==65))
					MotorWidget::SendCommand(2,"gui clear ");

				//setting the voltage and current again
				sprintf(message,"gui setv %d",voltage);
				MotorWidget::SendCommand(3,message);
				sprintf(message,"gui setc %d",current);
				MotorWidget::SendCommand(3,message);

				cout<<"voltage after setting it again before warmup & warming up to:  "<<(atoi(MotorWidget::SendCommand(2,"gui getv"))/1000)<<endl;

				//initiating warmup
				sprintf(message,"gui warmup %d",voltage);
				MotorWidget::SendCommand(3,message);
				highVoltage->setStyleSheet("background: red");

				//to make sure the error is cleared
				value =  atoi(MotorWidget::SendCommand(2,"gui geterr"));

				//successful warmup startup
				if(!value)
					CheckWarmupTime();
				//unsuccessful due to some reason
				else
				{
					strcpy(eMess,MotorWidget::SendCommand(2,"gui getemess "));
					sprintf(message,"ERROR: An error of %d:",value);
					strcat(message,eMess);
					strcat(message,"If you can't resolve what it is, please tell Dhanya of this error and how you got it. Disabling the Xray Tube part of the GUI. Thanks!");
					MotorWidget::ErrorMessage(message);

					sprintf(message,"X-Ray Tube ERROR: sr:12 is %d", value);
					xrayGroup->setTitle(message);
					highVoltage->setChecked(false);
					actualVDisp->setText("");
					actualCDisp->setText("");
					xrayGroup->setChecked(false);
				}
			}
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

int  MotorControlGui::UpdateXrayStatus()
{
	if(xrayGroup->isChecked())
	{
		int xrayStatus;
		char message[200];

		xrayStatus = atoi(MotorWidget::SendCommand(2,"gui xraystatus "));

		if((xrayStatus==-9)||(xrayStatus==-1))
		{
			strcpy(message,MotorWidget::SendCommand(2,"gui createxrayport "));
			if(strstr (message,"ERROR: Unable")!=NULL)
			{
				strcat(message,"\nSwitch it on/Fix it to enable its GUI");
				MotorWidget::ErrorMessage(message);
				xrayGroup->setTitle("X-Ray Tube's SERIAL PORT not created/ Switch on XRay Tube.");
				xrayGroup->setChecked(false);
				return 0;
			}
#ifdef VACUUMBOX
			if(strstr (message,"ERROR: Created")!=NULL)
			{
				strcat(message,"\nCould not create port for pressure monitoring.");
				MotorWidget::ErrorMessage(message);
			}
#endif
		}

		//--- for standby/switched off-----
		strcpy(message,MotorWidget::SendCommand(2,"gui isstandby "));

		if(strstr (message,"No")!=NULL)
			xrayGroup->setTitle("X-Ray Tube");
		else
		{
			if(strstr (message,"off")!=NULL)
			{
				MotorWidget::ErrorMessage("WARNING: The X-Ray Tube is on Switched off!\nSwitch it on to enable its GUI");
				xrayGroup->setTitle("X-Ray Tube SWITCHED OFF");
			}
			else
			{
				MotorWidget::ErrorMessage("WARNING: The X-Ray Tube is on Standby!\nSwitch it on to enable its GUI");
				xrayGroup->setTitle("X-Ray Tube ON STANDBY");
			}
			xrayGroup->setChecked(false);
			return 0;
		}

		//get voltage and current value
		voltage = atoi(MotorWidget::SendCommand(2,"gui getv"))/1000;
		current = atoi(MotorWidget::SendCommand(2,"gui getc"))/1000;
		setVDisp->setText(QString::number(voltage));
		setCDisp->setText(QString::number(current));
		actualV  = atoi(MotorWidget::SendCommand(2,"gui getactualv"))/1000;
		actualC  = atoi(MotorWidget::SendCommand(2,"gui getactualc"))/1000;
		actualCDisp->setText(QString::number(actualC));
		actualVDisp->setText(QString::number(actualV));

		//checks if warmup was going on, if not gets hv and shutters from server, if yes, displays graph
		CheckWarmupTime();

	}
	else
	{
		//if the xray group box was disabled
		if(!QString::compare(xrayGroup->title(), "X-Ray Tube",Qt::CaseInsensitive))
			MotorWidget::ErrorMessage("WARNING: You just disabled the GUI for the X-Ray Tube.\nPlease monitor its status on your own now OR through the command line");
	}

	return 0;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void MotorControlGui::UpdateXrayGroupforHV()
{
	cout<<" in updatexraygroupforhv()\n";

	if(!highVoltage->isChecked())
	{
		MotorWidget::SendCommand(3,"gui hv off ");
		highVoltage->setStyleSheet("background: gray");
		warning->setEnabled(false);
		warningText->setEnabled(false);
		shutter1->setChecked(false);
		shutter2->setChecked(false);
		shutter3->setChecked(false);
		shutter4->setChecked(false);
		shutterBox->setEnabled(false);
		wtDisp->setText("");
		warmupBox->setEnabled(false);

		//enabling the change voltage buttons if hv switched off while ramping up
		setVoltage->setEnabled(true);
		setV->setEnabled(true);
		unit1->setEnabled(true);
		setCurrent->setEnabled(true);
		setC->setEnabled(true);
		unit2->setEnabled(true);


		//display actual voltage and current
		actualC=0; actualV=0;
		actualCDisp->setStyleSheet("color: darkBlue");
		actualVDisp->setStyleSheet("color: darkBlue");
		actualCDisp->setText(QString::number(actualC));
		actualVDisp->setText(QString::number(actualV));

		/*
      //display voltage and current
      //while changing voltage, sometimes you need to switch off hv and this might be where it screws up
      voltage = atoi(MotorWidget::SendCommand(2,"gui getv"))/1000;
      current = atoi(MotorWidget::SendCommand(2,"gui getc"))/1000;
      setVDisp->setText(QString::number(voltage));
      setCDisp->setText(QString::number(current));
		 */
	}

	//if high voltage was switched on the gui and  no warm up is going on
	else if(!warmupBox->isEnabled())
		CheckWarmupRequired();


}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void MotorControlGui::UpdateXrayGroupforShutters()
{
	//finds out which shutter ws checked/unchecked to switch it on/off
	char message[200];
	int s;
	QCheckBox *checkedBox = qobject_cast<QCheckBox *>(sender());

	if(checkedBox!=NULL)
	{
		QString temp = checkedBox->text();
#ifdef XRAYBOX
		if(!QString::compare(temp,"Up")) s=1;
		else  s=3;
#else
		if (!QString::compare(temp,"Up")) s=2;
		else if (!QString::compare(temp,"Right")) s=3;
		else  s=4;
#endif
		sprintf(message,"gui shutter %d ",s);
		if(checkedBox->isChecked())
			strcat(message,"on");
		else
			strcat(message,"off");

		MotorWidget::SendCommand(4,message);
	}


	// check if each is enabled, else it creates segmentation error for isChecked() when not enabled
	bool safe = true;
	if(shutter1->isEnabled())
		if(shutter1->isChecked())
			safe = false;
	if(shutter2->isEnabled())
		if(shutter2->isChecked())
			safe = false;
	if(shutter3->isEnabled())
		if(shutter3->isChecked())
			safe = false;
	if(shutter4->isEnabled())
		if(shutter4->isChecked())
			safe = false;
	if(safe)
	{
		//stop the timer and disable the warning sign
		doorConnect = false;
		checkDoorTime->stop();
		checkDoorTime->disconnect();
		warning->setEnabled(false);
		warningText->setEnabled(false);
	}
	else
	{
		if(!doorConnect)
		{
			//start timer to check door and display the warning labels
			doorConnect = connect(checkDoorTime, SIGNAL(timeout()), this,SLOT(CheckDoorOpen()) );
			if(highVoltage->isChecked()) //making sure the warmup wasnt cancelled if there was a warmup
				checkDoorTime->start(200);
		}
		if(highVoltage->isChecked())//making sure the warm up wasnt cancelled if there was a warmup
		{
			warning->setEnabled(true);
			warningText->setEnabled(true);
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void MotorControlGui::CheckDoorOpen()
{
	char message[200];
	strcpy(message,MotorWidget::SendCommand(2,"gui geterr"));

	int value = atoi(message);
	if((value==63)||(value==64))
	{
		//disconnect timer
		checkDoorTime->disconnect(); //displayActual->disconnect(this);
		doorConnect = false;
		//display error message
		MotorWidget::ErrorMessage("ERROR: By opening the door while the shutters were open, you have switched off the High Voltage!\n\nSwitch off and then back on the Xray Tube MANUALLY to continue");
		//disable shutters and uncheck high voltage
		shutterBox->setEnabled(false);
		highVoltage->setChecked(false);
		//display actual voltage and current
		actualC=0; actualV=0;
		actualCDisp->setStyleSheet("color: darkBlue");
		actualVDisp->setStyleSheet("color: darkBlue");
		actualCDisp->setText(QString::number(actualC));
		actualVDisp->setText(QString::number(actualV));

		//MotorWidget::ErrorMessage("ERROR: Warm-up required to reset this error!"); CheckWarmupRequired();
		//to clear the error
		MotorWidget::SendCommand(2,"gui clear ");
	}
	else
		checkDoorTime->start(200);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void MotorControlGui::UpdateCurrentOrVoltage()
{
	cout<<" in updatecurrentorvoltage()\n";

	char message[200]="";
	QPushButton *clickedButton = qobject_cast<QPushButton *>(sender());

	if(!QString::compare(clickedButton->text(), "Set Voltage"))
	{
		if(!QString::compare(setV->text(), ""))
			MotorWidget::ErrorMessage("ERROR: Please enter a value to set the X-Ray Tube voltage");
		else if(!QString::compare(setV->text(),setVDisp->text()))
			MotorWidget::ErrorMessage("ERROR: The high voltage is already set to this voltage");
		else
		{
			//clear the 70 before u set the voltage or before u try to read it
			int value =  atoi(MotorWidget::SendCommand(2,"gui geterr"));
			cout<<"value="<<value<<endl;
			if ((value==70)||(value==63)||(value==64)||(value==65))
				MotorWidget::SendCommand(2,"gui clear ");

			sprintf(message,"gui setv %d",setV->text().toInt());
			strcpy(message,MotorWidget::SendCommand(3,message));

			cout<<"voltage from server:"<<(atoi(MotorWidget::SendCommand(2,"gui getv"))/1000)<<endl;

			if(strstr (message,"ERROR")==NULL)
			{
				voltage = setV->text().toInt();
				setVDisp->setText(QString::number(voltage));
			}
			MotorWidget::ErrorMessage(message);

		}
	}
	else
	{
		if(!QString::compare(setC->text(), ""))
			MotorWidget::ErrorMessage("ERROR: Please enter a value to set the X-Ray Tube current");
		else if(!QString::compare(setC->text(),setCDisp->text()))
			MotorWidget::ErrorMessage("ERROR: The high voltage is already set to this current");
		else
		{
			//clear the 70 before u set the voltage or before u try to read it
			int value =  atoi(MotorWidget::SendCommand(2,"gui geterr"));
			cout<<"value="<<value<<endl;
			if ((value==70)||(value==63)||(value==64)||(value==65))
				MotorWidget::SendCommand(2,"gui clear ");

			sprintf(message,"gui setc %d",setC->text().toInt());
			strcpy(message,MotorWidget::SendCommand(3,message));
			if(strstr (message,"ERROR")==NULL)
			{
				current = setC->text().toInt();
				setCDisp->setText(QString::number(current));
			}
			MotorWidget::ErrorMessage(message);
		}
	}

	//ull get the error code only when hv is on
	if(highVoltage->isChecked())
	{
		int value =  atoi(MotorWidget::SendCommand(2,"gui geterr"));
		if((value!=0)&&(value!=109))
		{
			char eMess[200];
			strcpy(eMess,MotorWidget::SendCommand(2,"gui getemess "));
			sprintf(message,"ERROR: An error of %d:",value);
			strcat(message,eMess);
			strcat(message,"Please tell Dhanya this error occurred while changing voltage or current.");
			MotorWidget::ErrorMessage(message);
		}
		if(value==0)
			UpdateActualValues();
		else
			CheckWarmupRequired();
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void MotorControlGui::GetErrorCodeFromTube()
{
	char message[200]="Error Code:";
	strcat(message,MotorWidget::SendCommand(2,"gui geterr"));
	strcat(message,":");
	strcat(message,MotorWidget::SendCommand(2,"gui getemess"));
	strcat(message," \n warmup time remaining: ");
	strcat(message,MotorWidget::SendCommand(2,"gui getwtime"));
	MotorWidget::ErrorMessage(message);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void MotorControlGui::ShowWarmupGraph()
{
	cout<<" in showwarmupgraph()\n";
	char message[200]="";

	//if this function is called when hv switched off while warming up
	if(!highVoltage->isChecked())
	{
		MotorWidget::ErrorMessage("ERROR: High Voltage has been switched off during the warm-up process.\nHence, the voltage/current has not been changed");

		voltage = atoi(MotorWidget::SendCommand(2,"gui getv"))/1000;
		current = atoi(MotorWidget::SendCommand(2,"gui getc"))/1000;
		setVDisp->setText(QString::number(voltage));
		setCDisp->setText(QString::number(current));
		displayTime->disconnect();
		timeConnect = false;
		startDrawing = false;
		clearDrawing = true;
		update();
	}
	else
	{
		//getting the actual voltages, current and wait time to plot graph
		actualV  = atoi(MotorWidget::SendCommand(2,"gui getactualv"))/1000;
		actualC  = atoi(MotorWidget::SendCommand(2,"gui getactualc"))/1000;
		wt = atoi(MotorWidget::SendCommand(2,"gui getwtime"));

		//check wt many times till u think it might be the right one
		while(wt<0)
			wt = atoi(MotorWidget::SendCommand(2,"gui getwtime"));

		if(wt==0)
			wt = atoi(MotorWidget::SendCommand(2,"gui getwtime"));

		//do something with the wt obtained; either draw graph or gethv and shutters from server
		if(wt>0)
		{
			//display warmuptme remaining in min and sec
			char time[200];
			int reminder = wt%60;
			int quotient = wt/60;
			sprintf(time,"%d min %d sec",quotient,reminder);
			wtDisp->setText(QString(time));
			//display actual voltage and current
			actualCDisp->setStyleSheet("color: red");
			actualVDisp->setStyleSheet("color: red");
			actualCDisp->setText(QString::number(actualC));
			actualVDisp->setText(QString::number(actualV));
			//start timer and update the graph
			if(!timeConnect)
				timeConnect = connect(displayTime, SIGNAL(timeout()), this,SLOT(ShowWarmupGraph()) );

			displayTime->start(2000);
			startDrawing = true;
			update();
		}
		//when warming up is over
		else
		{
			//this should enable shutterbox if hv=on
			UpdateHVFromServer();
			if(highVoltage->isChecked())
				shutterBox->setEnabled(true);
			UpdateShuttersFromServer();
			wtDisp->setText("");
			warmupBox->setEnabled(false);
			//enaabling the set voltage,current, shutters and hv buttons
			setVoltage->setEnabled(true);
			setV->setEnabled(true);
			setCurrent->setEnabled(true);
			setC->setEnabled(true);
			unit1->setEnabled(true);
			unit2->setEnabled(true);
			if(highVoltage->isChecked())
			{
				MotorWidget::ErrorMessage("Warm-up of X-Ray Tube is Complete. Leaving High Voltage switched on");
				//now saving the timestamp since warmup was successful
				sprintf(message,"gui writewarmuptiming %d",voltage);
				strcpy(message,MotorWidget::SendCommand(3,message));
				if(strstr (message,"ERROR")!=NULL)
					MotorWidget::ErrorMessage(message);
			}
			else
			{
				//technically this should not happen
				MotorWidget::ErrorMessage("ERROR: High Voltage has been switched off during the warm-up process.\nHence, the voltage/current has not been changed");
				voltage = atoi(MotorWidget::SendCommand(2,"gui getv"))/1000;
				current = atoi(MotorWidget::SendCommand(2,"gui getc"))/1000;
				setVDisp->setText(QString::number(voltage));
				setCDisp->setText(QString::number(current));
			}
			//display actual voltage and current
			actualCDisp->setStyleSheet("color: darkBlue");
			actualVDisp->setStyleSheet("color: darkBlue");
			actualCDisp->setText(QString::number(actualC));
			actualVDisp->setText(QString::number(actualV));
			//disconnect timer and update graph
			displayTime->disconnect();
			timeConnect = false;
			startDrawing = false;
			clearDrawing = true;
			update();
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void MotorControlGui::UpdateActualValues()
{
	cout<<" in updateactualvalues()\n";
	char message[200]="";

	if(highVoltage->isChecked())
	{

		actualC = atoi(MotorWidget::SendCommand(2,"gui getactualc"))/1000;
		actualV = atoi(MotorWidget::SendCommand(2,"gui getactualv"))/1000;

		cout<<"voltage="<<voltage<<" actual voltage="<<actualV<<endl;
		cout<<"current="<<current<<" actual current="<<actualC<<endl;


		//now saving the timestamp since warmup was successful

		sprintf(message,"gui writewarmuptiming %d",voltage);
		strcpy(message,MotorWidget::SendCommand(3,message));
		if(strstr (message,"ERROR")!=NULL)
			MotorWidget::ErrorMessage(message);


		if((actualC!=current)||(actualV!=voltage))
		{
			shutterBox->setEnabled(false);
			setVoltage->setEnabled(false);
			setV->setEnabled(false);
			unit1->setEnabled(false);
			setCurrent->setEnabled(false);
			setC->setEnabled(false);
			unit2->setEnabled(false);
			actualCDisp->setStyleSheet("color: red");
			actualVDisp->setStyleSheet("color: red");
			actualCDisp->setText(QString::number(actualC));
			actualVDisp->setText(QString::number(actualV));
			//start timer and update actualvalues
			if(!actualConnect)
				actualConnect = connect(displayActual, SIGNAL(timeout()), this,SLOT(UpdateActualValues()) );

			displayActual->start(200);
		}
		else
		{
			shutterBox->setEnabled(true);
			setVoltage->setEnabled(true);
			setV->setEnabled(true);
			unit1->setEnabled(true);
			setCurrent->setEnabled(true);
			setC->setEnabled(true);
			unit2->setEnabled(true);
			//display actual voltage and current
			actualCDisp->setStyleSheet("color: darkBlue");
			actualVDisp->setStyleSheet("color: darkBlue");
			actualCDisp->setText(QString::number(actualC));
			actualVDisp->setText(QString::number(actualV));
			//disconnect timer
			displayActual->disconnect();
			actualConnect = false;
		}

		update();
	}
	else
	{
		cout<<"inside disconnecting updateactualvalues timer\n";
		displayActual->disconnect();
		actualConnect = false;

		//display voltage and current
		voltage = atoi(MotorWidget::SendCommand(2,"gui getv"))/1000;
		current = atoi(MotorWidget::SendCommand(2,"gui getc"))/1000;
		setVDisp->setText(QString::number(voltage));
		setCDisp->setText(QString::number(current));
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void MotorControlGui::UpdateLowerLimits()
{
	char message[200];

	//they are equal to -9999 if they are not in the motor list
	if(xLower!=-9999)
	{
		strcpy(message,MotorWidget::SendCommand(3,"gui getLowerLimit detector_x"));
		xLower = atof(message);
	}
	if(yLower!=-9999)
	{
		strcpy(message,MotorWidget::SendCommand(3,"gui getLowerLimit detector_y"));
		yLower = atof(message);
	}

	if(pLower!=-9999)
	{
		strcpy(message,MotorWidget::SendCommand(3,"gui getLowerLimit psf"));
		pLower = atof(message);
	}
}


//-------------------------------------------------------------------------------------------------------------------------------------------------

void MotorControlGui::UpdateUpperLimits()
{
	char message[200];

	if(xLower!=-9999)
	{
		strcpy(message,MotorWidget::SendCommand(3,"gui getUpperLimit detector_x"));
		xUpper = atof(message);
	}
	if(yLower!=-9999)
	{
		strcpy(message,MotorWidget::SendCommand(3,"gui getUpperLimit detector_y"));
		yUpper = atof(message);
	}
	if(pLower!=-9999)
	{
		strcpy(message,MotorWidget::SendCommand(3,"gui getUpperLimit psf"));
		pUpper = atof(message);
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void MotorControlGui::ChangeFluorescenceList(int index)
{
	char message[200];

	currentflist=index+1;
	sprintf(message,"gui loadflist %d ",currentflist);
	strcpy(message,MotorWidget::SendCommand(3,message));
	if(strstr (message,"ERROR")!=NULL)
		MotorWidget::ErrorMessage(message);
	GetFluorValues();

	disconnect(fluorLabel,SIGNAL(currentIndexChanged(int)),this,SLOT(ChangeFluorescenceList(int)));
	disconnect(fluorName,SIGNAL(currentIndexChanged(int)),this,SLOT(MoveFluorescence(int)));
	//to remove all the current list
	while(fluorName->count()>=1)
		fluorName->removeItem(0);
	//to add the updated list
	for(int i=0;i<(int)fluorescence.size();i++)
		fluorName->addItem(QString(fluorescence[i][0].c_str()));
	connect(fluorLabel,SIGNAL(currentIndexChanged(int)),this,SLOT(ChangeFluorescenceList(int)));
	connect(fluorName,SIGNAL(currentIndexChanged(int)),this,SLOT(MoveFluorescence(int)));

	UpdateEnergyFromServer();

}
//-------------------------------------------------------------------------------------------------------------------------------------------------

void MotorControlGui::MoveFluorescence(int index)
{
	//if its not asked to move to 'none'
	if(QString::compare(fluorName->currentText(),"None"))
	{
		if(fluorName->count()>(int)fluorescence.size())
			fluorName->removeItem((int)fluorescence.size());

		char cDisplay[200],message[200],charPos[200];
		sprintf(cDisplay,"%s KeV",fluorescence[index][1].c_str());
		energyDisplay->setText(QString(cDisplay));

		sprintf(charPos,"%f",index*fluorWidth);
		sprintf(message,"gui moveabs fluorescence %s ",charPos);

		strcpy(message,MotorWidget::SendCommand(4,message));

		if(strstr (message,"ERROR")!=NULL)
			MotorWidget::ErrorMessage(message);
	}

}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void  MotorControlGui::GetSlitWidthFromServer()
{
	char message[200] = "gui getslitwidth ";
	strcpy(message,MotorWidget::SendCommand(2,message));
	widthDisplay->setText(QString(message));
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void  MotorControlGui::GetCenterFromServer()
{
	char message[200] = "gui getcenter ";
	strcpy(message,MotorWidget::SendCommand(2,message));
	centerDisplay->setText(QString(message));
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void  MotorControlGui::UpdateSlitWidth()
{
	double pos1 = motorWidgets[x1]->GetPosition();
	double pos2 = motorWidgets[x2]->GetPosition();
	double width = (Limit - pos2) - pos1;
	char charWidth[200];
	sprintf(charWidth,"%f",width);
	widthDisplay->setText(QString(charWidth));
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void  MotorControlGui::UpdateCenter()
{
	double pos1 = motorWidgets[x1]->GetPosition();
	double pos2 = motorWidgets[x2]->GetPosition();
	double center = (pos1 + Limit - pos2)/2;
	char charCenter[200];
	sprintf(charCenter,"%f",center);
	centerDisplay->setText(QString(charCenter));
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void  MotorControlGui::SlitsToCenter()
{
	char message[200] = "gui exactcenter ";

	double center = Limit/2;
	char charCenter[200];
	sprintf(charCenter,"%f",center);

	strcpy(message,MotorWidget::SendCommand(2,message));
	if(strstr (message,"ERROR")==NULL)
	{
		motorWidgets[x1]->UpdatePosition(charCenter,0);
		motorWidgets[x2]->UpdatePosition(charCenter,0);
		widthDisplay->setText("0.000000");
		centerDisplay->setText(QString(charCenter));
	}
	else
		MotorWidget::ErrorMessage(message);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void  MotorControlGui::SlitsZeroWidth()
{
	double center = widthDisplay->text().toDouble()/2;
	char charCenter[200];
	sprintf(charCenter,"%f",center);

	char message[200] = "gui widthabs 0 ";
	strcpy(message,MotorWidget::SendCommand(3,message));
	if(strstr (message,"ERROR")==NULL)
	{
		motorWidgets[x1]->UpdatePosition(charCenter,1);
		motorWidgets[x2]->UpdatePosition(charCenter,1);
		widthDisplay->setText("0.000000");
		UpdateCenter();
	}
	else
		MotorWidget::ErrorMessage(message);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void  MotorControlGui::CenterRelative()
{
	char message[200] = "gui centerrel ";
	char charPos[200]="";
	QString text = centerRelDisplay->text();
	double pos = text.toDouble();

	if(!QString::compare(text, ""))
		MotorWidget::ErrorMessage("ERROR: Please enter a value for the center relative position");
	else
	{

		QPushButton *clickedButton = qobject_cast<QPushButton *>(sender());
		if(!QString::compare(clickedButton->text(), "-"))
			pos = 0 - pos;

		sprintf(charPos," %f ",pos);
		strcat(message,charPos);
		strcpy(message,MotorWidget::SendCommand(3,message));
		if(strstr (message,"ERROR")==NULL)
		{
			motorWidgets[x1]->UpdatePosition(charPos,1);
			pos = 0 - pos;
			sprintf(charPos," %f ",pos);
			motorWidgets[x2]->UpdatePosition(charPos,1);
			UpdateCenter();
		}
		else
			MotorWidget::ErrorMessage(message);
	}

}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void  MotorControlGui::WidthRelative()
{
	char message[200] = "gui widthrel ";
	char charPos[200]="";
	QString text = widthRelDisplay->text();
	double pos = text.toDouble();

	if(!QString::compare(text, ""))
		MotorWidget::ErrorMessage("ERROR: Please enter a value for the relative width");
	else
	{

		QPushButton *clickedButton = qobject_cast<QPushButton *>(sender());
		if(!QString::compare(clickedButton->text(), "-"))
			pos = 0 - pos;

		sprintf(charPos," %f ",pos);
		strcat(message,charPos);
		strcpy(message,MotorWidget::SendCommand(3,message));
		if(strstr (message,"ERROR")==NULL)
		{
			pos = 0 - (pos/2);
			sprintf(charPos," %f ",pos);
			motorWidgets[x1]->UpdatePosition(charPos,1);
			motorWidgets[x2]->UpdatePosition(charPos,1);
			UpdateSlitWidth();
		}
		else
			MotorWidget::ErrorMessage(message);
	}

}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void  MotorControlGui::CenterAbs()
{
	char message[200] = "gui centerabs ";
	char charCenter[200]="", charPos[200];
	QString text = centerAbsDisplay->text();
	double center = text.toDouble();

	double oldCenter = centerDisplay->text().toDouble();
	double pos = center - oldCenter;
	sprintf(charPos," %f ",pos);

	if(!QString::compare(text, ""))
		MotorWidget::ErrorMessage("ERROR: Please enter a value for the slit width");
	else
	{
		sprintf(charCenter," %f ",center);
		strcat(message,charCenter);
		strcpy(message,MotorWidget::SendCommand(3,message));
		if(strstr (message,"ERROR")==NULL)
		{
			motorWidgets[x1]->UpdatePosition(charPos,1);
			pos = 0-pos;
			sprintf(charPos," %f ",pos);
			motorWidgets[x2]->UpdatePosition(charPos,1);
			centerDisplay->setText(QString(text));
			centerAbsDisplay->setText("");
		}
		else
			MotorWidget::ErrorMessage(message);
	}

}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void  MotorControlGui::WidthAbs()
{
	char message[200] = "gui widthabs ";
	char charWidth[200]="", charPos[200];
	QString text = widthAbsDisplay->text();
	double width = text.toDouble();

	double oldWidth = widthDisplay->text().toDouble();
	double pos = 0-((width - oldWidth)/2);
	sprintf(charPos," %f ",pos);

	if(!QString::compare(text, ""))
		MotorWidget::ErrorMessage("ERROR: Please enter a value for the slit width");
	else
	{
		sprintf(charWidth," %f ",width);
		strcat(message,charWidth);
		strcpy(message,MotorWidget::SendCommand(3,message));
		if(strstr (message,"ERROR")==NULL)
		{
			motorWidgets[x1]->UpdatePosition(charPos,1);
			motorWidgets[x2]->UpdatePosition(charPos,1);
			widthDisplay->setText(QString(text));
			widthAbsDisplay->setText("");
		}
		else
			MotorWidget::ErrorMessage(message);
	}

}

#else
//-------------------------------------------------------------------------------------------------------------------------------------------------

void  MotorControlGui::CheckReference()
{
	char message[200] = "gui checkref";
	strcpy(message,MotorWidget::SendCommand(2,message));
	if(!strcasecmp(message,"None"))
		refValue->setCurrentIndex(0);
	else
	{
		for(int i=0;i<(int)reflist.size();i++)
		{
			if(!strcasecmp(reflist[i].c_str(),message))
			{
				refValue->setCurrentIndex(i+1);
				break;
			}
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void  MotorControlGui::MoveToReference(int index)
{
	//index=0 is "None". when its not in any reference point
	if(index>0)
	{
		char message[200] = "gui ref ";
		strcat(message,reflist[index-1].c_str());
		strcpy(message,MotorWidget::SendCommand(3,message));
		if(strstr (message,"ERROR")!=NULL)
		{
			MotorWidget::ErrorMessage(message);
			refValue->setCurrentIndex(0);
		}
		UpdateGUI();
	}
}

#endif

//-------------------------------------------------------------------------------------------------------------------------------------------------
int  MotorControlGui::Execute()
{
	int num;
	bool modified;
	char message[255];
	char positions[3][200];
	char motorNames[3][200];
	int loopValue[3];

	//each controller
	for(int i=0;i<(int)motors.size();i++)
	{
		num = 0;
		//each motor in a contoller
		for(int j=0;j<(int)motors[i].size();j++)
		{
			if(motors[i][j]=="-")
				break;

			//comparing with each widget
			for(int k=1;k<MaxNumMotorWidgets;k++)
			{
				if(motorWidgets[k]==NULL) continue;
				if(!strcasecmp(motorWidgets[k]->GetName(),motors[i][j].c_str()))
				{
					modified = false;
					strcpy(positions[num],motorWidgets[k]->MoveAbsolute(&modified));
					sprintf(positions[num],"%f",atof(positions[num])); //to get the zeroes after the decimal pt
					strcpy(motorNames[num],motorWidgets[k]->GetName());
					loopValue[num] = k;

					if(modified)
					{
#ifndef LASERBOX
						if((k==2)||(k==3))
						{
							if((centerAbsDisplay->isModified())&&(QString::compare(centerAbsDisplay->text(), "")))
							{
								MotorWidget::ErrorMessage("ERROR: The slits cannot be moved absolutely and the center changed at the same time");
								return -1;
							}

							if((widthAbsDisplay->isModified())&&(QString::compare(widthAbsDisplay->text(), "")))
							{
								MotorWidget::ErrorMessage("ERROR: The slits cannot be moved absolutely and the width changed at the same time");
								return -1;
							}
						}
#endif
						//if no errors update num
						num++;
					}

				}
			}
		}

		// if no motors to be moved for this controller,go to the next controller
		if(!num) continue;

		if(num==1)
		{
			sprintf(message,"gui moveabs %s %s ",motorNames[0],positions[0]);
			strcpy(message,MotorWidget::SendCommand(4,message));
			if(strstr (message,"ERROR")==NULL)
			{
				motorWidgets[loopValue[0]]->UpdatePosition(positions[0],0);
#ifndef LASERBOX
				if((strstr (message,"SLIT")==NULL)&&(slits))
					UpdateSlitWidth();
#endif
			}
			else
			{
				MotorWidget::ErrorMessage(message);
				return -1;
			}
		}

		else if(num==2)
		{
			sprintf(message,"gui sim2moveabs %s %s %s %s",motorNames[0],positions[0],
					motorNames[1],positions[1]);
			strcpy(message,MotorWidget::SendCommand(6,message));
			if(strstr (message,"ERROR")==NULL)
			{
				motorWidgets[loopValue[0]]->UpdatePosition(positions[0],0);
				motorWidgets[loopValue[1]]->UpdatePosition(positions[1],0);
#ifndef LASERBOX
				if((strstr (message,"SLIT")==NULL)&&(slits))
					UpdateSlitWidth();
#endif
			}
			else
			{
				MotorWidget::ErrorMessage(message);
				return -1;
			}

		}
		else
		{
			sprintf(message,"gui sim3moveabs %s %s %s %s %s %s",motorNames[0],positions[0],
					motorNames[1],positions[1],motorNames[2],positions[2]);
			strcpy(message,MotorWidget::SendCommand(8,message));
			if(strstr (message,"ERROR")==NULL)
			{
				motorWidgets[loopValue[0]]->UpdatePosition(positions[0],0);
				motorWidgets[loopValue[1]]->UpdatePosition(positions[1],0);
				motorWidgets[loopValue[2]]->UpdatePosition(positions[2],0);
#ifndef LASERBOX
				if((strstr (message,"SLIT")==NULL)&&(slits))
					UpdateSlitWidth();
#endif
			}
			else
			{
				MotorWidget::ErrorMessage(message);
				return -1;
			}

		}

	}

#ifndef LASERBOX
	if(  ((centerAbsDisplay->isModified())&&(QString::compare(centerAbsDisplay->text(), ""))) &&
			((widthAbsDisplay->isModified())&&(QString::compare(widthAbsDisplay->text(), "")))  )
	{
		MotorWidget::ErrorMessage("ERROR: The center and the width cannot be changed at the same time");
		return -1;
	}

	if((centerAbsDisplay->isModified())&&(QString::compare(centerAbsDisplay->text(), "")))
		CenterAbs();


	if ((widthAbsDisplay->isModified())&&(QString::compare(widthAbsDisplay->text(), "")))
		WidthAbs();

	update();
#else
	//check if the motors are in their reference points
	CheckReference();
#endif

	return 0;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void MotorControlGui::UpdateGUI()
{
	cout<<"inside the updategui method()";

#ifndef LASERBOX
	if(fluor)
	{
		GetFluorValues();
		if(currentflist!= ((fluorLabel->currentIndex()) +1))
		{
			disconnect(fluorLabel,SIGNAL(currentIndexChanged(int)),this,SLOT(ChangeFluorescenceList(int)));
			disconnect(fluorName,SIGNAL(currentIndexChanged(int)),this,SLOT(MoveFluorescence(int)));
			fluorLabel->setCurrentIndex(currentflist-1);
			//to remove all the current list
			while(fluorName->count()>=1)
				fluorName->removeItem(0);
			for(int i=0;i<(int)fluorescence.size();i++)
				fluorName->addItem(QString(fluorescence[i][0].c_str()));
			connect(fluorLabel,SIGNAL(currentIndexChanged(int)),this,SLOT(ChangeFluorescenceList(int)));
			connect(fluorName,SIGNAL(currentIndexChanged(int)),this,SLOT(MoveFluorescence(int)));
		}
		UpdateEnergyFromServer();
	}
#endif

	for(int i=1;i<MaxNumMotorWidgets;i++)
		if(motorWidgets[i]!=NULL)
			motorWidgets[i]->GetPositionFromServer();

#ifndef LASERBOX
	if(slits)
	{
		UpdateSlitWidth();
		UpdateCenter();
	}
	if(xrayGroup->isChecked())
	{
		UpdateXrayStatus();
		/*
      UpdateHVFromServer();
      UpdateShuttersFromServer();
      UpdateXrayGroupforHV();
      UpdateXrayGroupforShutters();
      setVDisp->setText(QString::number(atoi(MotorWidget::SendCommand(2,"gui getv"))/1000));
      setCDisp->setText(QString::number(atoi(MotorWidget::SendCommand(2,"gui getc"))/1000));
      CheckWarmupTime();

      //if hv=off earlier,checks if warmup required
      if(!highVoltage->isChecked())
	CheckWarmupRequired();
		 */
	}
	update();
#else
	for(int i=0;i<FwheelWidget::NumFwheelWidgets;i++)
		fwheelWidgets[i]->UpdateValueFromServer();
	if((int)reflist.size()>0)
		CheckReference();
#endif
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void MotorControlGui::OptionsMenu()
{ 
#ifndef LASERBOX
	vector <string> actualList(this->list);
	if(!fluor)
	{
		actualList.erase(actualList.begin());
	}
	optionsWidget = new OptionsWidget(this,actualList);
	connect(optionsWidget,SIGNAL(LowerLimitChanged()), this,SLOT(UpdateLowerLimits()));
	connect(optionsWidget,SIGNAL(UpperLimitChanged()), this,SLOT(UpdateUpperLimits()));
#else
	optionsWidget = new OptionsWidget(this,this->list);
#endif


	optionsWidget->show();
	connect(optionsWidget,SIGNAL(OptionsClosed()), this,SLOT(UpdateGUI()));

	//center widget
	QDesktopWidget *desktop = QApplication::desktop();
	int width = optionsWidget->frameGeometry().width();
	int height = optionsWidget->frameGeometry().height();
	int screenWidth = desktop->width();
	int screenHeight = desktop->height();
	int x = (screenWidth - width) / 2;
	int y = (screenHeight - height) / 2;
	optionsWidget->move( x, y );

}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void MotorControlGui::closeEvent(QCloseEvent* event)
{
	cout<<"inside the closeEvent function..\n";

	int ret = QMessageBox::warning(this, "Quit Motor Control GUI", "Are you sure you want to quit?", QMessageBox::Ok | QMessageBox::Cancel);
	if (ret == QMessageBox::Ok)
	{
		char command[200];
		strcpy(command,MotorWidget::SendCommand(2,"gui save"));
		if(strstr (command,"ERROR")!=NULL)
			MotorWidget::ErrorMessage(command);

		MotorWidget::SendCommand(2,"gui unlock");

		event->accept();
	}
	else
		event->ignore();

}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void MotorControlGui::keyPressEvent(QKeyEvent* event)
{
	cout<<"inside KeyPressEvent()\n";
	if((event->key() == Qt::Key_Return) ||(event->key() == Qt::Key_Enter))
		Execute();
	else
		event->ignore();
}


#ifndef LASERBOX
//-------------------------------------------------------------------------------------------------------------------------------------------------

string MotorControlGui::LowerCase(string str)
{
	string s = str;
	std::string::iterator i = s.begin();
	while(i!=s.end()) *i=tolower(*(i++));
	return s;
}
#endif

//-------------------------------------------------------------------------------------------------------------------------------------------------
