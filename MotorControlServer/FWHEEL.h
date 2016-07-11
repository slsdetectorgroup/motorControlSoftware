/********************************************//**
 * @file FWHEEL.h
 * @short Defines the FWHEEL objects
 * @author Dhanya
 ***********************************************/
#ifndef FWHEEL_H
#define FWHEEL_H
#ifdef LASERBOX

#include "INTERFACE.h"
#include <vector>
#include <string>
using namespace std;

/**
 *@short Defines the FWHEEL object parameters and their methods.
 */
class FWHEEL
{
public:

	/**Constructor
	 @param Name Name of the filter wheel
	 @param Serialno The Serial number of the filter wheel
	 @param ValueList The absorption values for all positions of the filter wheel
	 @param Interface Pointer to an interface object.<br>(The port that the  filter wheel's connected to is accessed through this)
	 */
	FWHEEL(char* Name, char* Serialno, vector<double> ValueList, INTERFACE* Interface);

	/**gets the name of the filter wheel.
	 * returns the name of filter wheel
	 */
	char* getName();

	/**gets the Serialno of the filter wheel.
	 * returns the Serialno of the filter wheel
	 */
	char* getSerialno();

	/**gets the Interface of the filter wheel.
	 * returns the Interface of filter wheel
	 */
	INTERFACE* getInterface();

	/**sets the filter wheel to the position.
	 * Only used during server start up to know the position of wheel
	 * to start position 1
	 */
	void setStartPosition();

	/**gets the Value of the filter wheel.
	 * returns the Value of filter wheel
	 */
	double getValue();

	/**sets the Value of filter wheel.
	 * @param Value sets Value of filter wheel to this value
	 * returns 1 if it worked and it was a valid value, else 0
	 */
	int setValue(double Value);



	/*for debugging - Prints out the parameters of a filter wheel object: Name, Serialno, Value, Interface.
	 */
	void print();

	/**total number of filter wheel
	 */
	static int NumFwheels;

	/**total number of filter wheel
	 */
	static const int NumSlotsInWheel=6;

	/**Absorption Value list of all positions of filter wheel
	 */
	vector<double> ValueList;

private:

	/**Name of filter wheel
	 */
	char Name[200];

	/**Serialno connected to filter wheel
	 */
	char Serialno[200];

	/**Absorption Value of filter wheel
	 */
	double Value;

	/**Interface pointer of filter wheel
	 */
	INTERFACE* Interface;


};
#endif
#endif
