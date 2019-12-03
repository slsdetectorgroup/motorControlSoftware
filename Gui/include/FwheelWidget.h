 /********************************************//**
 * @file FwheelWidget.h
 * @short uses the form for a Fwheel widget and defines its methods
 * @author Dhanya
 ***********************************************/
#ifndef FWHEELWIDGET_H
#define FWHEELWIDGET_H
#ifdef LASERBOX

#include<QtGui>
#include "ui_form_fwheelwidget.h"
#include "MotorWidget.h"
//#include <vector>
//#include <string>
//using namespace std;

/**
 *@short uses the form for a Fwheel widget and defines its methods
 */

class FwheelWidget:public QWidget, private Ui::FwheelWidgetObject
{
  Q_OBJECT
  
    public: 

 
  /**Constructor
     creates the Fwheel widget using the form
     @param parent sets its parent widget to the one it was called from
     @param Name sets this FwheelWidget name to this value
  */
  FwheelWidget(QWidget *parent, const char *Name);

  /**returns the name of the Fwheel Widget
     @return the name of the Fwheel Widget
   */
  char* GetName();

  /**sets the validators for all the input widgets and connects all the slots
   */
  void Initialization();

  /**gets the position from the server and displays it
   */
  void UpdateValueFromServer();

  /**total number of fwheel widgets that will be displayed
   */
  static int NumFwheelWidgets;


    private:

  /**name of the FwheelWidget
   */
  char Name[200];

  /**Number of slots in filter wheel
   */
  static const int  NumSlots=6;

  /**vector of the absoption values of the filter wheel
   */
  vector<double>fVals;


  private slots:

  /**moves the fwheel motor according to the sample selected
     @param index the value of the item selected in combobox
   */
  void MoveFwheel(int index);


};

#endif
#endif
