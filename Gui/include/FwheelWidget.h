#pragma once

class QStatusBar;
#include "ui_form_fwheelwidget.h"

class FwheelWidget : public QWidget, private Ui::FwheelWidgetObject {
  Q_OBJECT
public:
  FwheelWidget(QWidget *parent, std::string name, std::string hostname,
               QStatusBar *statusBar);
  ~FwheelWidget();
  std::string GetName();

public:
  void Update();

private slots:
  void SetValue();

private:
  void LayoutWindow();
  void LoadAbsorptionValues();
  void Initialization();

  void GetValue();

  std::string name;
  std::string hostname;
  QStatusBar *statusBar;
};
