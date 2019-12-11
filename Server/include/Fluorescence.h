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

 private:
  int index; 
  bool circular;
  Controller* controller;
  std::vector<TargetHolder*> targetHolder;
  int currentTargetHolder; 
}; 
