#pragma once

#include "ui_form_optionswidget.h"
class MotorWidget;

#include <QMainWindow>
class OptionsWidget : public QMainWindow, private Ui::OptionsWidgetObject {
  Q_OBJECT

public:
  OptionsWidget(QWidget *parent, std::string hostname,
                std::vector<std::string> motors,
                std::vector<std::string> refpoints,
                std::vector<std::string> fluor,
                std::vector<std::string> fwheels);
  ~OptionsWidget();

  void EnableTube(bool enable);
  void EnablePressure(bool enable);

protected:
  void closeEvent(QCloseEvent *event);

public slots:
  void Update();

private slots:
  void SetMotor();
  void SetPosition(double value);
  void SetUpperLimit(double value);
  void SetLowerLimit(double value);
  void SetController();
  void SendCommandToController();
  void SendandReadCommandToController();
  void SetRefPoint();
  void SetFluor();
  void DeleteHolder();
  void AddHolder();
  void ClearHolderContents();
  void SetFwheel();
  void SetTube();
  void SetPressure();
  void SendCommandToTube();
  void SendandReadCommandToTube();

private:
  void LayoutWindow(std::vector<std::string> motors,
                    std::vector<std::string> refpoints,
                    std::vector<std::string> fluor,
                    std::vector<std::string> fwheels);
  void LoadControllers();
  void Initialization();

  void GetMotorController();
  void GetPosition();
  void GetUpperLimit();
  void GetLowerLimit();
  void GetControllerSerialNumber();
  void GetControllerInterface();
  void GetControllerMotors();
  void GetNumFluorHolders();
  void GetFwheelSerialNumber();
  void GetFwheelInterface();
  void GetTubeInterface();
  void GetPressureInterface();

  std::string hostname;
  std::vector<QLineEdit *> targets;
  std::vector<QDoubleSpinBox *> energy;

signals:
  void ClosedSignal();
};