#pragma once

class Controller;

#include <string>
#include <vector>

class ReferencePoint {
public:
  ReferencePoint(int index, std::string name, std::vector<double> position);
  std::string getName();
  std::vector<double> getPositions();
  bool isMatch(std::vector<double> pos);
  void print();

private:
  int index;
  std::string name;
  std::vector<double> position;
};
