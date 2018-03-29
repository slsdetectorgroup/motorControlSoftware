/********************************************//**
 * @file PGAUGE.h
 * @short Defines the PGAUGE objects
 * @author Dhanya
 ***********************************************/
#ifndef PGAUGE_H
#define PGAUGE_H
#ifdef VACUUMBOX

#include "INTERFACE.h"
#include <vector>
#include <string>
using namespace std;

/**
 *@short Defines the PGAUGE object parameters and their methods.
 */
class PGAUGE
{
public:

    /**Constructor
	 @param Interface Pointer to an interface object.<br>(The port that the  pressure gauge's connected to is accessed through this)
     */
    PGAUGE(INTERFACE* Interface);

    /**gets the Interface of the pressure gauge
     * returns the Interface of pressure gauge
     */
    INTERFACE* getInterface();

    /*for debugging - Prints out the parameters of a pressure gauge object: Name, Serialno, Interface.
     */
    void print();

    /**
     * Get Pressure from both the pressure sensors
     * @param status1 status of pressure sensor 1
     * @param value1 value of pressure sensor 1
     * @param status2 status of pressure sensor 2
     * @param value2 value of pressure sensor 2
     * @returns true if successful in getting data
     */
    bool getPressure(string& status1, double& value1, string& status2, double& value2);

    /**total number of pressure gauges per controller
     */
    static const int NumPGaugesPerController = 2;

private:

    /**Interface pointer of pressure gauge
     */
    INTERFACE* Interface;


};
#endif
#endif
