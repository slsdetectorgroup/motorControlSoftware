#pragma once

class QStatusBar;
#include "ui_form_motorwidget.h"

class MotorWidget : public QWidget, private Ui::MotorWidgetObject {
    Q_OBJECT
  public:
    MotorWidget(QWidget *parent, std::string name, std::string hostname,
                QStatusBar *statusBar);
    ~MotorWidget();
    std::string GetName();
    double GetPositionFromWidget();
    void Update();

  private slots:
    void MoveAbsolute(double value);
    void MoveLeft();
    void MoveRight();
    void Calibrate();

  private:
    void LayoutWindow();
    void Initialization();

    void GetPosition();
    void MoveRelative(double value);

    std::string name;
    std::string hostname;
    QStatusBar *statusBar;

  signals:
    void MotorMovedSignal();
};