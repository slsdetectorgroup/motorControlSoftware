#pragma once

#include "ui_form_tubewidget.h"

class TubeWidget:public QWidget, private Ui::TubeWidgetObject {
    Q_OBJECT
    public: 
    TubeWidget(QWidget *parent, std::string hostname);
    ~TubeWidget();

    public slots:
    void Update();
  
    private slots:


    private:
    void LayoutWindow();
    void Initialization();

    std::string hostname;

    signals:
    void UpdateSignal();
    void SwitchedOffSignal(bool);
};
