 /********************************************//**
 * @file Slit.cpp
 * @short Defines the slit objects
 * @author Dhanya
 ***********************************************/

#include "Slit.h"
#ifndef LASERBOX

#include <iostream>
using namespace std;


//-------------------------------------------------------------------------------------------------------------------------------------------------

Slit::Slit(double x1pos, double x2pos):x1pos(x1pos),x2pos(x2pos),Limit(105000)//102.5)
{
  x1Limit=Limit-x2pos;
  x2Limit=Limit-x1pos;
#ifdef VERY_VERBOSE
  print();
#endif 
}

//-------------------------------------------------------------------------------------------------------------------------------------------------


double Slit::getLimit()
{
  return Limit;
}


//-------------------------------------------------------------------------------------------------------------------------------------------------


double Slit::getX1Limit()
{
  return x1Limit;
}


//-------------------------------------------------------------------------------------------------------------------------------------------------


double Slit::getX2Limit()
{
  return x2Limit;
}


//-------------------------------------------------------------------------------------------------------------------------------------------------


void Slit::setX1pos(double newPosition)
{
  x1pos=newPosition;
  x2Limit=Limit-x1pos;
#ifdef VERY_VERBOSE
  print();
#endif
}


//-------------------------------------------------------------------------------------------------------------------------------------------------


void Slit::setX2pos(double newPosition)
{
  x2pos=newPosition;
  x1Limit=Limit-x2pos;
#ifdef VERY_VERBOSE
  print();
#endif
}


//-------------------------------------------------------------------------------------------------------------------------------------------------


void Slit::setBothpos(double newPosition1, double newPosition2)
{
  x1pos=newPosition1;
  x2pos=newPosition2;
  x1Limit=Limit-x2pos;
  x2Limit=Limit-x1pos;
#ifdef VERY_VERBOSE
  print();
#endif
}

//-------------------------------------------------------------------------------------------------------------------------------------------------


int Slit::canX1Move(double newPosition)
{
  if(newPosition > x1Limit)  
	return 1;
  else if(newPosition < 0)
	return -1;
  //return 0 if it can move
  else
	return 0;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------


int Slit::canX2Move(double newPosition)
{
  if(newPosition >  x2Limit)
	return 1;
  else if (newPosition < 0)
	return -1;
  //return 0 if it can move  
  else 
	return 0;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------


int Slit::canBothSlitsMove(double newPosition1, double newPosition2)
{
  double newX1Limit = Limit - newPosition2;
  double newX2Limit = Limit - newPosition1;

  if(newPosition1 > newX1Limit)
	return 1;
  else if (newPosition1 < 0)
	return -1;
  else if(newPosition2 > newX2Limit)
	return 1;
  else if (newPosition2 < 0)
	return -1;
  //reutrn 0 of they can move
  else 
	return 0;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------


double Slit::getSlitWidth()
{
    return x1Limit - x1pos;
}


//-------------------------------------------------------------------------------------------------------------------------------------------------


double Slit::getX1Center()
{
  return (x1Limit+x1pos)/2;
}


//-------------------------------------------------------------------------------------------------------------------------------------------------


void Slit::print()
{
  cout<<"\n\nSLITS:";
  cout<<"\nSlit x1 position:"<<x1pos;
  cout<<"\nSlit x2 position:"<<x2pos;
  cout<<"\nSlit x1 Limit position:"<<x1Limit;
  cout<<"\nSlit x2 Limit position:"<<x2Limit;
  cout<<"\nX1 center pos:"<<getX1Center();
  cout<<"\n";
}

//-------------------------------------------------------------------------------------------------------------------------------------------------
#endif
