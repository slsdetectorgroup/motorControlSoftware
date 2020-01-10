#pragma once

#include "ui_form_optionswidget.h"
class MotorWidget;


#include <QMainWindow>
class OptionsWidget : public QMainWindow, private Ui::OptionsWidgetObject {
    Q_OBJECT

    public:
    OptionsWidget(QWidget *parent, std::string hostname, 
        std::vector <std::string> motors,
        std::vector <std::string> refpoints,
        std::vector <std::string> fluor);
    ~OptionsWidget();    
    
    protected:
    void closeEvent(QCloseEvent* event);

    private slots:
    void SetMotor(int index);
    void SetPosition(double value);
    void SetUpperLimit(double value);
    void SetLowerLimit(double value);   
    void SetRefPoint(int index);
    void SetFluor(int index);
    void DeleteHolder();
    void AddHolder();
    void ClearHolderContents();

    public slots:
    void Update();

    private:
    void LayoutWindow(
        std::vector <std::string> motors,
        std::vector <std::string> refpoints,
        std::vector <std::string> fluor);
    void Initialization();

    void GetMotorController();
    void GetPosition();
    void GetUpperLimit();
    void GetLowerLimit();
    void GetNumFluorHolders();

    std::string hostname;
    std::vector <QLineEdit*> targets;
    std::vector <QDoubleSpinBox*> energy;

    signals:
    void ClosedSignal();
};