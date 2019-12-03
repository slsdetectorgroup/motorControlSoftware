#include <QApplication>
#include <iostream>
#include <string>
#include <fstream>
#include <unistd.h>

#include "MotorControlGui.h"
using namespace std;



int main(int argc, char *argv[])
{
#ifdef XRAYBOX
	cout << " This is XRay Box Gui" << endl;
#elif LASERBOX
	cout << " This is Laser Box Gui" << endl;
#else
	cout << " This is Vacuum Box Gui" << endl;
#endif
  QApplication app(argc, argv);
  app.setStyle(new QPlastiqueStyle);//not default when desktop is windows
  MotorControlGui* gui = new MotorControlGui(argc,argv);


  /* QScrollArea *scrollArea = new QScrollArea;
 scrollArea->setWidget(gui);
 scrollArea->resize(1290,945);
 scrollArea->show();*/
  gui->show();
 return app.exec();
}
