#pragma once

class QCheckBox;
#include "ui_form_tubewidget.h"

class TubeWidget:public QWidget, private Ui::TubeWidgetObject {
    Q_OBJECT
    public: 
    TubeWidget(QWidget *parent, std::string hostname);
    ~TubeWidget();

    public slots:
    void Update();
  
    private slots:
    void DisplayTubeError();
    void ClearError();
    void SetHighVoltage(bool enable);
    void SetShutters();
    void SetVoltage(int value);
    void SetCurrent(int value);

    private:
    void LayoutWindow();
    void Initialization();
    std::string SendTubeCommand(std::string hostname, int nCommand, std::string command, std::string source);

    void GetConnectedShutters();
    void GetHighVoltage();
    void GetShutters();
    void CheckWarning();
    void GetVoltage();
    void GetCurrent();
    void GetActualVoltage();
    void GetActualCurrent();

    std::string hostname;
    std::vector<QCheckBox*> chkShutters;

    signals:
    void SwitchedOffSignal(bool);
};
