#pragma once

#include <string>
#include <vector>

class ReferencePoint {
    public:
    ReferencePoint(int index, std::string name, std::vector<double> position);
    std::string getName();
    double getXPosition();
    double getYPosition();
    double getZPosition();
    void print();

    private:
    int index;
    std::string name;
    std::vector<double> position;
};
