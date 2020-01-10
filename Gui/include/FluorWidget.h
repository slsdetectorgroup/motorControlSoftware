#pragma once

class QStatusBar;
#include "ui_form_fluorwidget.h"
class MotorWidget;

class FluorWidget:public QWidget, private Ui::FluorWidgetObject {
    Q_OBJECT
    public: 
    FluorWidget(QWidget *parent, std::string name, std::string hostname, QStatusBar* statusBar);
    ~FluorWidget();
    std::string GetName();
    void UpdateHolderList();
    void Update();

    private slots:
    void SetHolder(int index);
    void SetTarget(int index);
    void SetStackedWidget();
    void GetTarget();

    private:
    void LayoutWindow();
    void LoadTargetHolders();
    void Initialization();

    void GetHolder();
    void GetTargetList();

    std::string name;
    std::string hostname;
    std::vector<std::string> energy;
    MotorWidget* motor;
    QStatusBar* statusBar;
};