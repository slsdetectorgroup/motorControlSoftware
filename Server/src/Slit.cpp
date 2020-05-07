#include "Slit.h"
#include "Motor.h"
#include "commonDefs.h"

#include <iostream>

#define SLITS_SUM_LIMIT (105.00)
#define SLITS_INDIVIDUAL_LIMIT (100.00)

Slit::Slit(Controller *controllerX1, Controller *controllerX2, int axisX1,
           int axisX2)
    : controllerX1(controllerX1), controllerX2(controllerX2), axisX1(axisX1),
      axisX2(axisX2), limit(SLITS_SUM_LIMIT) {
  FILE_LOG(logINFO) << "Slit: [controllerX1:" << controllerX1->getName()
                    << ", controllerX2:" << controllerX2->getName()
                    << ", axisX1:" << axisX1 << ", axisX2:" << axisX2 << "]";
}

void Slit::updateLimits() {
  double x1pos = controllerX1->getMotor(axisX1)->getPosition();
  double x2pos = controllerX2->getMotor(axisX2)->getPosition();
  double limitx1 = limit - x2pos;
  if (limitx1 > SLITS_INDIVIDUAL_LIMIT) {
    limitx1 = SLITS_INDIVIDUAL_LIMIT;
  }
  double limitx2 = limit - x1pos;
  if (limitx2 > SLITS_INDIVIDUAL_LIMIT) {
    limitx2 = SLITS_INDIVIDUAL_LIMIT;
  }
  controllerX1->getMotor(axisX1)->setUpperLimit(limitx1);
  controllerX2->getMotor(axisX2)->setUpperLimit(limitx2);
  FILE_LOG(logINFO) << "Updated upperlimits of slits ("
                    << controllerX1->getMotor(axisX1)->getUpperLimit() << ", "
                    << controllerX2->getMotor(axisX2)->getUpperLimit() << ')';
}

std::string Slit::canBothMove(double position1, double position2) {
  if (position1 > SLITS_INDIVIDUAL_LIMIT) {
    return "slit_x1 cannot move beyond hard limits";
  }
  if (position2 > SLITS_INDIVIDUAL_LIMIT) {
    return "slit_x2 cannot move beyond hard limits";
  }
  if (position1 + position2 > SLITS_SUM_LIMIT) {
    return "The slits will crash into each other";
  }
  return "";
}

double Slit::getSlitWidth() {
  double x1pos = controllerX1->getMotor(axisX1)->getPosition();
  double x2pos = controllerX2->getMotor(axisX2)->getPosition();
  return limit - x2pos - x1pos;
}

void Slit::setSlitWidth(double width) {
  double currentwidth = getSlitWidth();
  if (Motor::matches(currentwidth, width)) {
    return;
  }
  double halfwidth = (currentwidth - width) / 2;
  double x1pos = controllerX1->getMotor(axisX1)->getPosition();
  double x2pos = controllerX2->getMotor(axisX2)->getPosition();
  double x1NewPos = x1pos + halfwidth;
  double x2NewPos = x2pos + halfwidth;
  std::string errMove = canBothMove(x1NewPos, x2NewPos);
  if (!errMove.empty()) {
    std::ostringstream oss;
    oss << "Cannot set slitwidth to " << std::fixed << width << ". " << errMove;
    throw RuntimeError(oss.str());
  }
  // update limits each time else they will not move
  controllerX1->moveAbs(x1NewPos, axisX1);
  updateLimits();
  controllerX2->moveAbs(x2NewPos, axisX2);
  updateLimits();
  if (!Motor::matches(getSlitWidth(), width)) {
    std::ostringstream oss;
    oss << "Could not set slit width to " << width << ". Got " << std::fixed
        << getSlitWidth();
    throw RuntimeError(oss.str());
  }
}

double Slit::getCenter() {
  double x1pos = controllerX1->getMotor(axisX1)->getPosition();
  double x2pos = controllerX2->getMotor(axisX2)->getPosition();
  return (limit - x2pos + x1pos) / 2;
}

void Slit::setCenter(double center) {
  double currentcenter = getCenter();
  double relPos = center - currentcenter; // center according to x1 direction
  if (Motor::matches(relPos, 0.0)) {
    return;
  }
  double x1pos = controllerX1->getMotor(axisX1)->getPosition();
  double x2pos = controllerX2->getMotor(axisX2)->getPosition();
  double x1NewPos = x1pos + relPos;
  double x2NewPos = x2pos - relPos;
  std::string errMove = canBothMove(x1NewPos, x2NewPos);
  if (!errMove.empty()) {
    std::ostringstream oss;
    oss << "Cannot set center to " << std::fixed << center << ". " << errMove;
    throw RuntimeError(oss.str());
  }
  // move the appropriate one first, else they will crash (update limits each
  // time else they will not move)
  if (relPos > 0) {
    controllerX2->moveAbs(x2NewPos, axisX2);
    updateLimits();
    controllerX1->moveAbs(x1NewPos, axisX1);
    updateLimits();
  } else {
    controllerX1->moveAbs(x1NewPos, axisX1);
    updateLimits();
    controllerX2->moveAbs(x2NewPos, axisX2);
    updateLimits();
  }
  if (!Motor::matches(getCenter(), center)) {
    std::ostringstream oss;
    oss << "Could not set center to " << center << ". Got " << std::fixed
        << getCenter();
    throw RuntimeError(oss.str());
  }
}

void Slit::print() {
  std::cout << "Slit" << std::endl;
  std::cout << "====" << std::endl;
  std::cout << "\tLimit: " << limit << ", Controllers: ["
            << controllerX1->getName() << ", " << controllerX2->getName() << ']'
            << std::endl;
}
