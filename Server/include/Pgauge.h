#pragma once

class Interface;

#include "commonDefs.h"

#include <vector>

class Pgauge {
public:
  Pgauge(Interface *interface);
  ~Pgauge();
  Interface *getInterface();
  std::string getStatus(int value);
  int getInteger(std::string result);
  std::vector<PressureGauge> getPressure();
  void print();

private:
  Interface *interface;
};
