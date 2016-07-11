 /********************************************//**
 * @file SLIT.h
 * @short Defines slit object
 * @author Dhanya
 ***********************************************/
#ifndef SLIT_H
#define SLIT_H
#ifndef LASERBOX
/**
  *@short Defines the slit object parameters and some methods to do before calling a slit motor move function.
  */

class SLIT
{
 public:

  /**Constructor
	 creates the slit object with the positions of both the slits
	 @param x1pos position of slit_x1
	 @param x2pos position of slit_x2
  */
  SLIT(double x1pos, double x2pos);
  
  /** returns the Limit member for slit object
	  @returns the Limit member for slit object
   */
  double getLimit();
  
  /** returns the limit position of slit_x1
	  @returns the limit position of slit_x1
   */
  double getX1Limit();

  /** returns the limit position of slit_x2
	  @returns the limit position of slit_x2
   */
  double getX2Limit();

  /**sets the new slit_x1 position and its limit
   */
  void setX1pos(double newPosition);

  /**sets the new slit_x2 position and its limit
   */
  void setX2pos(double newPosition);

  /**sets the positions and the limits of both the slits
   */
  void setBothpos(double newPosition1, double newPosition2);

  /**returns 0 if slit_x1 can move to new position without crash, else  1 or -1
	 @returns 0 if slit_x1 can move to new position without crash, else 1 or -1
   */ 
  int canX1Move(double newPosition);

  /**returns 0 if slit_x2 can move to new position without crash, else 1 or -1
	 @returns 0 if slit_x2 can move to new position without crash, else 1 or -1
   */ 
  int canX2Move(double newPosition);

  /**returns 0 if both slits can move simultaneously to new position without crash, else 1 or -1
	 @returns 0 if both slits can move simultaneously to new position without crash, else  1 or -1
   */
  int canBothSlitsMove(double newPosition1, double newPosition2);

  /**gets the distance between the slit motors
	 @returns the width between the slit motors
  */
  double getSlitWidth();

  /**gets the center of the slits compared to x1 measurements
	 @returns the center of the slits compared to x1 measurements
  */
  double getX1Center();

  /**for debugging: prints all the members of the slit object
   */
  void print();
  

 private:
  
  /**Slit_x1 position
   */
  double x1pos;
  
  /**Slit_x2 position
   */
  double x2pos;
  
  /**Slit_x1 limit position
   */
  double x1Limit;
  
  /**Slit_x2 limit position
   */
  double x2Limit;

  /**Limiting value
   */ 
  double Limit; 


}; 
#endif
#endif
