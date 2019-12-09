#pragma once

class TargetHolder;
class Controller;

#include <vector>
#include <string>


class Fluorescence {
 public:


  Fluorescence(int index, bool circular, Controller* controller);
  bool isCircular();
  Controller* getController();
  int getNumTargetLists();
  void addTarget(std::string name, std::string energy);
  std::string getTargetName(int listIndex, int targetIndex);
  std::string getTargetEnergy(int listIndex, int targetIndex);
  void print();


  
  double getLimit();
  
  /**sets the fluorescence position and its limit
   */
  void setPos(double newPosition);

  /**returns 0 if fluorescence can move to new position without crash, else  1 or -1
	 @returns 0 if fluorescence can move to new position without crash, else 1 or -1
   */ 
  int canMove(double newPosition);

  

  

 private:
  int index; 
  bool circular;
  Controller* controller;
  std::vector<TargetHolder*> targetHolder;
  int currentTargetHolder;


  double pos;
  double limit;
  
}; 
