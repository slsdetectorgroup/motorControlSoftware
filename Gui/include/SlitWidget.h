#pragma once

class QStatusBar;
#include "ui_form_slitwidget.h"
class MotorWidget;

class SlitWidget:public QWidget, private Ui::SlitWidgetObject {
    Q_OBJECT
    public: 
    SlitWidget(QWidget *parent, std::string hostname, QStatusBar* statusBar);
    ~SlitWidget();

    public slots:
    void Update();

    private slots:
    void SetSlitWidth(double value);
    void SetCenter(double value);

    private:
    void LayoutWindow();
    void Initialization();

    void GetSlitWidth();
    void GetCenter();

    std::string hostname;
    MotorWidget* motorx1;
    MotorWidget* motorx2;
    QStatusBar* statusBar;
};