#pragma once

#include "ui_form_fluorwidget.h"
class MotorWidget;

class FluorWidget:public QWidget, private Ui::FluorWidgetObject {
    Q_OBJECT
    public: 
    FluorWidget(QWidget *parent, std::string name, std::string hostname);
    ~FluorWidget();
    std::string GetName();

    public:
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

    signals:
    void UpdateSignal();
};