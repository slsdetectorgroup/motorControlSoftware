#pragma once

#include "ui_form_referencepointswidget.h"
class MotorWidget;

class ReferencePointsWidget:public QWidget, private Ui::ReferencePointsWidgetObject {
    Q_OBJECT
    public: 
    ReferencePointsWidget(QWidget *parent, std::string hostname, MotorWidget* x, MotorWidget* y, MotorWidget* z);
    ~ReferencePointsWidget();

    public:
    void Update();

    private slots:
    void GetReferencePoint();
    void SetReferencePoint(int index);

    private:
    void LayoutWindow();
    void LoadReferencePoints();
    void Initialization();

    std::string hostname;
    MotorWidget* x;
    MotorWidget* y;
    MotorWidget* z;
};