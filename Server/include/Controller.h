#pragma once

class Interface;
class Motor;

#include <vector>
#include <string>

class Controller {
    public:
    
    Controller(const int index, const std::string name, const std::string serialNumber);
    static bool CheckControllerSerialNumber(Interface* interface, std::string serialNumber);
    std::string getName() const;
    std::string getSerialNumber() const;

    void setInterface(Interface* interface);
    Interface* getInterface() const;

    void setMotor(const int axis, Motor* motor);
    Motor* getMotor(int axis) const;

    void updateAxisEnabled();
    void debugPositions();
    void moveRel(double position, int axis);
    void moveAbs(double position, int axis);
    void resetPosition(double position, int axis);
    void calibrate(int axis);
    void rangeMeasure(int axis);
    void stop();

    // advanced
    void sendCommand(std::string command);
    std::string sendCommandAndReadBack(std::string command);

    void print();

    private:
    const int index;
    const std::string name;
    const std::string serialNumber;
    Interface* interface;
    std::vector<Motor*> motor;
};