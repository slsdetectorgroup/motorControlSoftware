#pragma once

class Controller;

#include <vector>
#include <string>

class TargetHolder {
  public:
  TargetHolder(int index);
  int getNumTargets();
  bool isFull();
  std::string getCurrentTargetName(int index);
  void addTarget(std::string name, std::string energy);
  std::string getList();
  void print();

  private:
  int index;
  std::vector<std::string> targetName;
  std::vector<std::string> targetEnergy;
  int numTargets;
};
