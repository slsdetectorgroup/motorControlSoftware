#ifndef COMMON_DEFS_H
#define COMMON_DEFS_H

#include <stdint.h> 


typedef float float32_t;
typedef int int32_t;

/** 
    
This file contains all the basic definitions common to the client and to the server


 * @author Anna Bergamaschi
 * @version 0.1alpha (any string)
 * @see slsDetector
*/

/** default maximum string length */
#define MAX_STR_LENGTH 1000

/** socket port number */
#define PORT_NO 1111

#ifdef XRAYBOX
#define Server_Hostname "pc-x-ray-box.psi.ch"
#elif LASERBOX
#define Server_Hostname "mpc2085.psi.ch"
#else
#define Server_Hostname "mpc2069.psi.ch"
#endif
/** 
   return values


*/

enum {
  OK, /**< function successfully executed */
  FAIL,  /**< error occurred */
  GOODBYE  /**< stop */
};

/** 
   function indexes to call on the server


*/

enum {
  F_MY_COMMAND, /**< dummy function */
  F_ANY_OTHER_FUNCTION  /**< ??? */

};




#endif
