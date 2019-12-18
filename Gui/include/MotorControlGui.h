 /********************************************//**
 * @file MotorControlGui.h
 * @short defines the gui objects, signals and slots
 * @author Dhanya
 ***********************************************/
#ifndef MOTORCONTROLGUI_H
#define MOTORCONTROLGUI_H
#include <QtGui>

#ifdef LASERBOX
#include "ui_form_laserboxgui.h"
#elif VACUUMBOX
#include "ui_form_vacuumboxgui.h"
#else
#include "ui_form_motorcontrolgui.h"
#endif
#include "MotorWidget.h"

#ifdef LASERBOX
#include "FwheelWidget.h"
#elif VACUUMBOX
#include "PGaugeWidget.h"
#endif

#include "OptionsWidget.h"
#include <vector>
#include <string>
using namespace std;

#define FLUOR_LASERNAME	"laser"
#define FLUOR_PARA_NUM	3

/**
 *@short Uses the main form used and defines the gui objects, 
 signals and slots
*/
#ifdef VACUUMBOX
class MotorControlGui:public QWidget, private Ui::VacuumBoxGuiObject
#elif LASERBOX
class MotorControlGui:public QWidget, private Ui::LaserBoxGuiObject
#else
class MotorControlGui:public QWidget, private Ui::MotorControlGuiObject
#endif
{
  Q_OBJECT

    public:
  /**Constructor
     creates the main widget which is the main window
     @param argc number of command line arguments for server options
     @param argv server options
     @param parent makes the parent window 0 by default
  */
  MotorControlGui(int argc, char *argv[],QWidget *parent = 0);

  /**gets the list of motors from the server, upper limits & lower limits
     of x detector, y detector and psf, then creates vector 'motors'
  */	
  void GettingListOfMotors();

#ifndef LASERBOX
  /**gets the list of fluorescence samples
  */	  
  void GetFluorValues();
#endif

  /**creates motor widget objects from another form and lays them out in the groupbox, 
     also initializes other widgets
   */  
  void LayoutWindow();

  /**create validators for any inputs and create all the connections to the slots
   */  
  void Initialization();


#ifndef LASERBOX
  /**gets the position of fluorescence from server and accordingly calculates the sample
     and displays it. If the motor is not in the exact position, it displays none.
   */  
  void UpdateEnergyFromServer();

  /**gets from the server if the high voltage for the tube is switched on or not
     and accordingly checks the high voltage gui button on/off
   */  
  void UpdateHVFromServer();

  /**gets from the server if the shutters are open/close
     and accordingly checks the appropriate shutter check boxes
   */  
  void UpdateShuttersFromServer();

  /**checks if warm up time is greater than 0(warm up in process)
     if its greater, then calls ShowWarmupGraph(), else updates HV and shutters from server
   */  
  void CheckWarmupTime();

  /**gets the eror message from tube. if its 106,109,70 or 63, the tube warms up and then 
     calls CheckWarmupTime(). if the value if 64, prints error to switch off&on the tube manually and exits
     if no warm up required, it updates actual current and actual values if the high voltage is switched on
     Always asks before warming-up. Returns 0 if the warm up was cancelled.
   */  
  void CheckWarmupRequired();

  /**converts strings to lower case 
     @param str the string to convert to lower case
     returns the string converted to lower case
    */  
  string LowerCase(string str);
#endif


 protected:

#ifndef LASERBOX
  /**keeps painting the top view and the front view.
     Also paints the warm up graphs if warm of tube in process.
     @param event if update() is called, this method is executed
   */  
  void paintEvent(QPaintEvent *event);
#endif

  /**If the window is closed, confirms with the user if they really want to quit,
     then saves the positions and the new config file and exits
     @param event if the window is closed, this method is called
   */  
  void closeEvent(QCloseEvent* event);

  /**if enter key or return key is pressed, the absolute movements are executed
     @param event this method is called if return or enter key is pressed
   */  
  void  keyPressEvent(QKeyEvent* event);




 private:

#ifndef LASERBOX
  /**Max number of widgets that can be shown in the main gui window, excluding fluorescence
   */  
  static const int  MaxNumMotorWidgets=8;

#else
  /**Max number of widgets that can be shown in the main gui window, excluding fluorescence
   */
  static const int  MaxNumMotorWidgets=3;

  /**Max number of widgets that can be shown in the main gui window
   */
  static const int  MaxNumFwheelWidgets=5;

  /**list of fwheelWidget object pointers
   */
  FwheelWidget *fwheelWidgets[MaxNumFwheelWidgets];
#endif

  /**list of motorWidget object pointers
   */  
  MotorWidget *motorWidgets[MaxNumMotorWidgets];

  /**pointer to a OptionWidget, which is the window that pops up when 'option' button is clicked
   */  
  OptionsWidget *optionsWidget;

  /**list is the list of motors in the config file
   */  
  vector<string> list;
#ifdef VACUUMBOX
  /** Pressure Gauge Widget Controller */
  PGaugeWidget *pGaugeWidget;
#endif

#ifdef LASERBOX
  /**list is the list of filter wheel motors in the config file
   */
  vector<string> fwlist;

  /**list is the list of references in the config file
   */
  vector<string> reflist;
#else
  /**the list of all the motots that can be drawn in the top view and gront view
   */  
  vector<string> drawingList;
#endif

  /**vector of all the motors according ot their controllers
   * This is to know whether to use moveabs, sim2moveabs or moveabs for 3
   */  
  vector < vector<string> > motors;

#ifndef LASERBOX
  /**vector of all the data for fluorescence samples
   */  
  vector < vector<string> > fluorescence;

  /**layout of the xray motors box
   */  
  QGridLayout *mainLayout;
#else
  /**layout of the motors box
   */
  QVBoxLayout *mainLayout;

  /**layout of the filter wheels box
   */
  QVBoxLayout *mainLayout2;
#endif

  /**frame for the widget created by clicking on the 'options' button
   */  
  QFrame* popup;

#ifndef LASERBOX
  /**scene for the voltage graph
   */  
  QGraphicsScene* scene3;

  /**scence for the current graph
   */  
  QGraphicsScene* scene4;

  /**previous point on voltage graph
   */  
  QPointF* oldv;

  /**previous point on current graph
   */  
  QPointF* oldc;

   /**previous actual voltage value
   */  
  int oldActualV;

  /**previous actual current value
   */  
  int oldActualC;

  /**voltage value
   */  
  int voltage;

  /**current value
   */  
  int current;

  /**actual voltage value
   */  
  int actualV;

  /**actual current value
   */  
  int actualC;

  /**wait time remaining in seconds
   */  
  int wt;

  /**timer for the warmup graphs
   */  
  QTimer* displayTime;

  /**timer for the ramp of actual values when high voltage is switched on
   */  
  QTimer* displayActual;

  /**timer to check if the door is opened when the shutters are open
   */  
  QTimer* checkDoorTime;

  /**true if displayTime is connected
   */  
  bool timeConnect;

  /**true if displayActual is connected
   */  
  bool actualConnect;

  /**true if checkDoorTime is connected
   */  
  bool doorConnect;

  /**true if the wt>0 and warm up graphs have to be displayed
   */  
  bool startDrawing;

  /**true if the warmup is over and need to be cleared once
   */  
  bool clearDrawing;
#endif

  /**the error message to be displayed
   */  
  char eMessage[200];

#ifndef LASERBOX
  /**limit of the slit motors
   */  
  static const double Limit = 105000;//102.5;

  /** current florescence target holder index
   */  
  int currentflist;

  /**if slits exists
   */  
  bool slits;

  /**the position of slit_x1 in motowidgets[] if it exists, else -1
   */  
  int x1;

  /**the position of slit_x2 in motowidgets[] if it exists, else -1
   */  
  int x2;

  /**the position of huber in motowidgets[] if it exists, else -1
   */ 
  int huberPos;

  /**the value of huber position default as 0
   */ 
  double huberValue;

  /**the lower limit of detector_x if its exists, else -9999
   */  
  double xLower;

  /**the upper limit of detector_x if its exists, else -9999
   */  
  double xUpper;

  /**the lower limit of detector_y if its exists, else -9999
   */  
  double yLower;

  /**the upper limit of detector_y if its exists, else -9999
   */  
  double yUpper;

  /**the lower limit of psf if its exists, else -9999
   */  
  double pLower;

  /**the upper limit of psf if its exists, else -9999
   */  
  double pUpper;

#endif

  private slots:

#ifndef LASERBOX
  /**Called when xrayGroup is toggled, creates xray interface& xray object if none, checks if tube on 
     standby or switched off, checks if warmup required, displays graphs if warm up in process
   */  
  int UpdateXrayStatus();

  /**enables/disables appropriate widgets according to the high voltage
   */  
  void UpdateXrayGroupforHV();

  /**enables/disables appropriate widgets according to the shutters, also starts the checkDoorTimer
     if any of the shutters are open
   */  
  void UpdateXrayGroupforShutters();

  /**called when the checkdoortimer stops, checks for error 63 or 64(door open), prints error msg, and
     initiates warm up by called CheckWarmupRequired()
   */  
  void CheckDoorOpen();

  /**updates the current or voltage and if the high voltage is on, checks if warm up is required
   */  
  void UpdateCurrentOrVoltage();

  /**gets the error code from the tube
   */  
  void GetErrorCodeFromTube();

  /**if wt>0, starts displayTimer and then starts drawing warm up graphs
     else updates the gui and disconnects timer
   */  
  void ShowWarmupGraph();

  /**updates actual values of voltage and current as the tube ramps up
   */  
  void UpdateActualValues();

  /**when the lower limits are set in the options menu, 
     the lower limits of det_x, det_y and psf are set from server
   */  
  void UpdateLowerLimits();

  /**when the upper limits are set in the options menu, 
     the upper limits of det_x, det_y and psf are set from server
   */  
  void UpdateUpperLimits();

  /**Updates the fluorescence list with the right list
     @param index the value of the list selected in combobox
   */  
  void ChangeFluorescenceList(int index);

  /**moves the fluorescence motor according to the sample selected
     @param index the value of the item selected in combobox
   */  
  void MoveFluorescence(int index);

  /** If true, disables all the fluorescence lists
   * and moves fluorescence to laser position
   * if false, enables all fluorescence lists
   * @param checkFluor true if laser clicked, else false
   */
  void MoveFluortoLaser(bool checkFluor);

  /**gets the slit width from the server and displays it
   */  
  void GetSlitWidthFromServer();

  /**gets the slit center from the server and displays it
   */  
  void GetCenterFromServer();      

  /**updates the slit width based on the slit positions in gui
   */  
  void UpdateSlitWidth();

  /**updates the slit center based on the slit positions in gui
   */  
  void UpdateCenter();

  /**moves the slits to the exact center
   */  
  void SlitsToCenter();

  /**moves the slits the same distance such that the width between them is 0
   */  
  void SlitsZeroWidth();

  /**moves the slit center relatively
   */  
  void CenterRelative();

  /**changes the slit width relatively
   */  
  void WidthRelative();  

  /**moves the center to an absolute position
   */  
  void CenterAbs();    

  /**changes the slit width to an absolute measurement
   */  
  void WidthAbs();
#else
  /**checks if the motors are on any of the reference points
	and updates the reference list to none or the ref name
	This is done for every abs and relative movt.
   */
  void CheckReference();

  /**moves the corvus motors x,y and z to the reference point
     @param index the value of the item selected in combobox
   */
  void MoveToReference(int index);
#endif

  /**executes all teh absolute movements that has been modified
   */  
  int Execute();

  /**updates the gui with values from the server
   */  
  void UpdateGUI();

  /**an optionsWidget is created and displayed when the 'options' button is clicked
   */  
  void OptionsMenu();
};

#endif
