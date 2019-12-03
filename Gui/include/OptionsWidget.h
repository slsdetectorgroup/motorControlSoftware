 /********************************************//**
 * @file OptionsWidget.h
 * @short creates the menu and widgets for the options menu
 * @author Dhanya
 ***********************************************/
#ifndef OPTIONSWIDGET_H
#define OPTIONSWIDGET_H

#include<QtGui>
#include "ui_form_optionsgui.h"
#include "MotorWidget.h"

/**
 *@short creates the menu and widgets for the options menu
 */

class OptionsWidget:public QFrame, private Ui::OptionsObject
{ 
  Q_OBJECT
  
    public: 

  /**Constructor
     creates the options menu and the optionWidget
     @param parent the parent widget is set to this value
     @param list list of motors in the config file
  */
  OptionsWidget(QWidget *parent,vector<string> list);

  /**updates the drop down menu with the list of motors
  */		
  void LayoutWindow();

  /**sets the validators for the input parameters and makes all the connectiosn to the slots
  */		
  void Initialization();

 protected:

  /**emits OptionsClosed() signal when this window is closed
     @param event when the window is closed, this method is executed
  */		
  void closeEvent(QCloseEvent* event);

  /**moves the motor absolutely when enter or return key is pressed
     @param event when enter or return key is pressed
  */		
  void  keyPressEvent(QKeyEvent* event);
 
 private:

  /**maximum number of motor widget objects to show in the menu
  */		
  static const int  NumOptionWidgets=8;

  /**list of motorWidget object pointers
  */		
  MotorWidget *optionWidgets[NumOptionWidgets];

  /**the value of the current item in drop down menu
  */		
  int current;

  /**list of the motors in the config file
  */		
  vector<string> list;
 
  private slots:

  /**called when a different motor is selected from the drop down menu
     @param index the value of the item on the drop down menu
  */		
  void ChangeMotorWidget(int index);

  /**if the input for absolute movement is modified, moves it and updates position
  */		
  void MoveAbsolute();

  /**calibrates the motor to the lower limit switch and sets the position to zero
  */		
  void MoveToHome();

  /**if either set position,speed, limits are selected, this is called.Whenever a limit is changed,
     an appropriate signal is emitted
  */		
  void SetOption();

 signals:

  /**emitted when the options window is closed
  */		
  void OptionsClosed();

#ifndef LASERBOX
  /**emited when the lower limit is changed
  */		
  void LowerLimitChanged();

  /**emitted when the upper limit is changed
  */		
  void UpperLimitChanged();
#endif
};

#endif
