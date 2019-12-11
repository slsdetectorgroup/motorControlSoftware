#pragma once

class TargetHolder;
class Controller;

#include <vector>
#include <string>


class Fluorescence {
 public:


  Fluorescence(int index, std::string name, Controller* controller);
  std::string getName();
  bool isCircular();
  Controller* getController();
  int getNumTargetHolders();
  void setCurrentTargetHolder(int index);
  int getCurrentTargetHolder();
  std::string getCurrentTargetName();
  void addTarget(std::string name, std::string energy);
  std::string getList();
  void print();

 private:
  int index; 
  std::string name;
  bool circular;
  Controller* controller;
  std::vector<TargetHolder*> targetHolder;
  int currentTargetHolder; 
  int currentTarget;
}; 
