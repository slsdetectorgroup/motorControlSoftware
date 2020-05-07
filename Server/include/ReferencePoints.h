#pragma once

class ReferencePoint;
class Controller;

#include <string>
#include <vector>

class ReferencePoints {
public:
  ReferencePoints(std::vector<Controller *> controller, std::vector<int> axis);
  int size();
  void add(std::string name, std::vector<double> position);
  std::vector<double> getCurrentMotorPositions();
  std::vector<double> getPositions(std::string name);
  std::string getList();
  std::string getCurrentReferenceName();
  void moveTo(std::string name);
  void print();

private:
  std::vector<Controller *> controller;
  std::vector<int> axis;
  std::vector<ReferencePoint *> referencePoint;
};
