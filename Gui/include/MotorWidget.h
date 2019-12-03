 /********************************************//**
 * @file MotorWidget.h
 * @short uses the form for a motor widget and defines its methods
 * @author Dhanya
 ***********************************************/
#ifndef MOTORWIDGET_H
#define MOTORWIDGET_H

#include<QtGui>

#include "Client.h"
#include "ui_form_motorwidget.h"

/**
 *@short uses the form for a motor widget and defines its methods
 */

class MotorWidget:public QWidget, private Ui::MotorWidgetObject
{
  Q_OBJECT
  
    public: 

  /**creating static object to emit signal from a static function
   */
  static MotorWidget *self;
 
  /**Constructor
     creates the motor widget using the form
     @param parent sets its parent widget to the one it was called from
     @param Name sets this motorWidget name to this value
  */
  MotorWidget(QWidget *parent, const char *Name);

  /**returns the name of the motor Widget
     @return the name of the motor Widget
   */
  char* GetName();

  /**returns the position of the motor Widget
     @return the position of the motor Widget
   */
  double GetPosition(); 

  /**sets the validators for all the input widgets and connects all the slots
   */
  void Initialization();

  /**gets the position from the server and displays it
   */
  void GetPositionFromServer();

  /**updates position of the motor according to the parameters and displays it
     @param Position the position to update it to based on the relative parameter value
     @param relative true if this the first arguments is a relative value and false if its an absolute value
   */
  void UpdatePosition(char Position[],bool relative);

  /**disables the motor widget
   */
  void Disable();

  /**checks if the motor widget is enabled
   */
  bool IsEnabled();

  /**checks if the input for absolute movement has been modified and returns the value,
     also updates the modified parameter to true if it has been modified
     @param modified becomes true if the input for absolute movement has been modified
     @return the string for absolute movement
   */
  char* MoveAbsolute(bool *modified);

 /**emits UpdateGUISignal signal
   */
  void emitUpdateGUISignal(){emit UpdateGUISignal();};

  /**sends the command to the server
     @param argc the number of arguments including "gui" attached in the front
     @param args the arguments 
     @return the message received back from the server
   */
  static char* SendCommand(int argc, char args[]);

 /**Changes the hostname of the server to connect to
	 @param host_ip_or_name the hostname or ip address of the server ot connect to
   */
  static void ChangeHostname(char* host_ip_or_name);

  /**uses system commands to get pcname, username etc
     @param systemCommand the command sent to system
     returns output returned by system
  */	

  static char* useSystemCommand(char* systemCommand);

  /**displays an error message
     @param errorMessage the message to be displayed
   */
  static void  ErrorMessage(char errorMessage[]);

#ifdef LASERBOX
  /**total number of motor widgets that will be displayed
   */
  static int NumMotorWidgets;
#endif

 private:

  /**name of the motorWidget
   */
  char Name[200];

  /**the Client object through which we communicate to server
   */
  static Client *client;

  /**the widget through which we display error messages
   */
  static QMessageBox *msgBox;

  /**the widget through which we diplay lockconflict error with options to unlock
   */
  static QMessageBox *lockMsg;

  /**hostname of server to connect to
   */
  static char serverHostname[1000];

 /**userdetails
   */
  static char userDetails[255];

  private slots: 

  /**moves the motor relatively according to input parameters and displays the updated position
   */
  void MoveRelative(bool positive);

  /**moves the motor in positive direction relatively according to input parameters and displays the updated position
   */
  void MovePosRelative();

  /**moves the motor in negative direction relatively according to input parameters and displays the updated position
   */
  void MoveNegRelative();

 signals:

#ifndef LASERBOX
  /**emitted when slits are moved relatively
   */
  void SlitsMoved();
#endif

  /**emitted when a successful relative movement has been executed
   */
  void RelativeMoved();

  /**emitted when there is a conflict in client usage
   */
  void UpdateGUISignal();

};

#endif
