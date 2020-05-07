#pragma once

class QCheckBox;
class QStatusBar;
class QTimer;
#include "ui_form_tubewidget.h"

class TubeWidget : public QWidget, private Ui::TubeWidgetObject {
  Q_OBJECT
public:
  TubeWidget(QWidget *parent, std::string hostname, QStatusBar *statusBar);
  ~TubeWidget();

public slots:
  void Update();

private slots:
  void DisplayTubeError();
  void ClearError(bool wait);
  void ClearError();
  void SetHighVoltage(bool enable);
  void SetShutters();
  void SetVoltage(int value);
  void SetCurrent(int value);
  void UpdateActualValues();
  void UpdateWarmupTiming();

private:
  void LayoutWindow();
  void Initialization();
  std::string SendTubeCommand(std::string hostname, int nCommand,
                              std::string command, std::string source);

  void GetConnectedShutters();
  void GetHighVoltage();
  void GetShutters();
  void CheckWarning();
  void GetVoltage();
  void GetCurrent();
  void GetActualVoltage();
  void GetActualCurrent();
  /* return 1 if update called, else 0 */
  int CheckWarmup();
  void AcceptWarmup();
  void DeclineWarmup();
  int GetWarmupTimeRemaining();
  void UpdateValues();

  std::string hostname;
  std::vector<QCheckBox *> chkShutters;
  QStatusBar *statusBar;
  QTimer *timerActual;
  QTimer *timerWarmup;
  static const int ACTUAL_TIME_OUT_MS = 1000;
  static const int WARMUP_TIME_OUT_MS = 10000;

signals:
  void SwitchedOffSignal(bool);
};
