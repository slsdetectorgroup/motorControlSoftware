#pragma once

#include "ui_form_motorwidget.h"

class MotorWidget:public QWidget, private Ui::MotorWidgetObject {
    Q_OBJECT
    public: 
    MotorWidget(QWidget *parent, std::string name, std::string hostname);
    ~MotorWidget();
    std::string GetName();

    public:
    void Update();

    private slots:
    void MoveAbsolute(double value);
    void MoveLeft();
    void MoveRight();

    private:
    void LayoutWindow();
    void Initialization();

    void GetPosition();
    void MoveRelative(double value);

    std::string name;
    std::string hostname;

    signals:
    void UpdateSignal();
};