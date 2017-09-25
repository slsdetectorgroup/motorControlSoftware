/********************************************//**
 * @file INITIALIZE.h
 * @short reads config files, moves the motors, and saves positions and a new config file
 * @author Dhanya
 ***********************************************/
#ifndef INITIALIZE_H
#define INITIALIZE_H


#include "MOTOR.h"
#include "INTERFACE.h"

#ifdef LASERBOX
#include "FWHEEL.h"
#else
#include "SLIT.h"
#include "XRAY.h"
#ifdef VACUUMBOX
#include "PRESSURE.h"
#endif
#endif

#include <vector>
#include <string>
#include "time.h"
using namespace std;



/**
  *@short reads config files, creates motor and interface objects, also executes commands from commandline
  */

class INITIALIZE
{
 public:
#ifndef LASERBOX
  /**Constructor
	 reads config files, gets the controller interface,creates motor objects and interface objects
	 @param fName name of config file
	 @param fName2 name of positions file to save the positions to
	 @param fName3 name of new config file
	 @param fName4 name of warmupTimingsstamp file
  */
  INITIALIZE(string const fName,string const fName2,string const fName3,string const fName4);
#else
  /**Constructor
	 reads config files, gets the controller interface,creates motor objects and interface objects
	 @param fName name of config file
	 @param fName2 name of positions file to save the positions to
	 @param fName3 name of new config file
  */
  INITIALIZE(string const fName,string const fName2,string const fName3);
#endif
  /**Destructor
	 closes all the serial fds for the interface
  */
  ~INITIALIZE();

  /**executes commands from the command line through both server and local
	 @param argc number of command line arguments - 1 (not including the object name eg ./localClient)
	 @param args array of command line arguments (not including the object name eg ./localClient
	 @param mess message to be sent to client after executing the command
	 @returns 0 for successful command execution,-1 for fail
  */	
  int executeCommand(int argc, char* args[], char mess[]);

#ifndef LASERBOX
  /**executes commands from the command line through both server and local
	 @param sLine the fluorescence line read from the config file
	 @param fIndex the index of fluorescence
  */	
  void initFluorNames(string sLine, int &fIndex);

#else
  /**gets the interface of the filter wheel by specifying serial number
   * @param serialNum serial number of the filter wheel
   * returns the interfacenumber
   */
  int getFWInterface(string const serialNum);
#endif
 
  /**called from the constructor,creates motor/interface objects for current line read from config file,
	 calls the getContInterface() to get the controller interface after reading all controllers
	 @param nArg number of arguments in current line of config file
	 @param args array of arguments in current line of config file
  */                                 
  void init(int nArg, char *args[]);

  /**called from init() after reading in all the controllers,
	 gets the controller interface and sets the Controller[][] and ContInterface[][] values
	 by sending 'getserialno' to each interface
   */
  void getContInterface();

  /**the positions of the motors are saved to file before exiting from the program
	 @param fName name of file the positions are saved in
	 returns the errormessage
  */
  char* savePositions(string const fName);

  /**a new config file is written with any updated values
	 @param oldFName the name of the config file used in the beginning
	 @param fName the name of the new config file to save everything to
	 returns the errormessage
  */
  char* saveToNewConfigFile(string const oldFName, string const fName);

  /**if the positions of the motors in config file(the ones being presently used)
	 are different from the positions saved to file the last time,
	 then it resets the positions of the motors to that of the file
	 @param fName name of file the positions were saved in
  */
  void setOrigPositions(string const fName);

  /**waits till all the controllers are not busy 
   */
  void check_busy(); 

#ifndef LASERBOX
  /**reads the warmupTimingstamps file upon startup
	 @param fName name of file of the warmuptiming stamps
	 returns 0
  */
  int ReadWarmupTimestamps(string const fName);

  /**writes the warmupTimingstamps file upon gui close
	 @param fName name of file of the warmuptiming stamps
	 returns 0
  */
  int WriteWarmupTimestamps(string const fName);
#endif
 private:

//#ifdef LOCAL
  /**array of motor object pointers
   */    
  MOTOR* Motor[10];

  /**array of interface object pointers
   */
  INTERFACE* Interface[6];


#ifndef LASERBOX
  /**pointer to xray tube serial object
   */
  INTERFACE* TubeInterface;

  /**pointer to pressure serial object for vacuumbox
   */
  INTERFACE* PressureInterface;

  /**xray class object
   */
  XRAY* XrayTube;

#ifdef VACUUMBOX
  /** pressure object
   */
  PRESSURE* Pressure;
#endif

 /**a slit pointer
   */
  SLIT* Slit;

  /**if slit_x1 exists,0 for not,else position of its object ptr in Motor[] array
   */
  int slit1_exists;

  /**if slit_x2 exists,0 for not,else position of its object ptr in Motor[] array
   */
  int slit2_exists;

  /**if fluorescence exists,0 for not,else position of its object ptr in Motor[] array
   */
  int fluor_exists;

  /**if fluorescence second set exists,0 for not,else 1
   */
  int set2_exists;

  /**max power of the xray tube
   */
  int maxTubePower;

  /**xray tube status while starting server
   */
  int xrayStatus;
#else
  /**array of fwheel object pointers
   */
  FWHEEL* Fwheel[5];

  /**array of interface object pointers for the filter wheels
   */
  INTERFACE* FWInterface[5];
#endif


  /**name of config file
   */
  string configFileName;

  /**name of configNew file
   */
  string configNewFileName;

  /**name of positions file
   */
  string positionsFileName;

#ifndef LASERBOX
  /**name of warmuptime file
   */
  string warmupTimeFileName;

  /**fluorwidth  41.95 or 45
   */
#ifdef XRAYBOX
  static const double fluorwidth = 41.95;
#else
  static const double fluorwidth = 45;
#endif

  /**max fluorescence values =7 or 8
   */
#ifdef XRAYBOX
  static const int maxfluorvalues = 7;
#else
  static const int maxfluorvalues = 8;
#endif

  /**list of fluorescence names
   */
  vector < vector<string> > fluorList;

 /**list of fluorescence set1  names
   */
  vector < vector<string> > fluorList1;

 /**list of fluorescence set2 names
   */
  vector < vector<string> > fluorList2;

  /**the list of warmup Timings for all the different voltages of the XRay Tube
   */  
  vector<string> warmupTimings;
#endif


//#endif

  /**number of controllers
   */
  int NUMBER_OF_CONTROLLERS;

#ifdef LASERBOX
  /**number of reference points
   */
  int NUMBER_OF_REFPOINTS;

  /** reference points. cols: name, detx position, dety position, detz position
   */
  vector <vector<string> >ReferencePoints;
#endif


  /** 2-d array: 1st col is controller names, 2nd col is controller serial numbers
   */
  vector <vector<string> >Controller;

  /** 2-d array: 1st col is controller names, 2nd col is the port the controller is connected to
   */
  vector <vector<string> >ContInterface;

};

#endif
