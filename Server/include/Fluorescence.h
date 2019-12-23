#pragma once

class TargetHolder;
class Controller;

#include <vector>
#include <string>


class Fluorescence {
 public:


  Fluorescence(int index, std::string name, Controller* controller, int axis);
  std::string getName();
  bool isCircular();
  Controller* getController();
  int getNumTargetHolders();
  void setCurrentTargetHolder(int index);
  int getCurrentTargetHolder();
  void addTarget(std::string name, std::string energy);
  std::string getList(int index);
  std::string updateAndGetCurrentTargetName();
  void moveToTarget(std::string target);
  void print();

 private:
  void updateCurrentTarget();

  int index; 
  std::string name;
  bool circular;
  Controller* controller;
  int axis;
  int width;
  int offset;
  std::vector<TargetHolder*> targetHolder;
  int currentTargetHolder; 
  int currentTarget;
}; 
