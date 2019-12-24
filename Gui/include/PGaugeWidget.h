#pragma once

#include "ui_form_pgaugewidget.h"
class QTimer;

class PGaugeWidget:public QWidget, private Ui::PGaugeWidgetObject {
    Q_OBJECT
    public: 
    PGaugeWidget(QWidget *parent, std::string hostname);
    ~PGaugeWidget();

    public slots:
    void Update();
  
    private slots:
    void EnableUpdate(bool enable);
    void TimeoutRefresh();


    private:
    void LayoutWindow();
    void Initialization();

    void GetPressure();
    std::string GetTimeStamp();

    std::string hostname;
    QTimer* timer;
    pthread_mutex_t mutex;

    signals:
    void SwitchedOffSignal(bool);
};
