#pragma once

class Controller;

#include <string>
#include <vector>

class TargetHolder {
  public:
    TargetHolder(int index);
    int getNumTargets();
    bool isFull();
    std::string getCurrentTargetName(int index);
    void addTarget(std::string name, std::string energy);
    void changeTarget(int index, std::string name, std::string energy);
    int getTargetIndex(std::string name);
    std::string getList();
    void print();

  private:
    int index;
    std::vector<std::string> targetName;
    std::vector<std::string> targetEnergy;
    int numTargets;
};
