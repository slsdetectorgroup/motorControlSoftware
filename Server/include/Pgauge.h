#pragma once

class Interface;

#include <string>

class Pgauge {
public:
    Pgauge(Interface* interface);
    Interface* getInterface();
    void print();
    bool getPressure(std::string& status1, double& value1, std::string& status2, double& value2);

private:
    Interface* interface;
};

