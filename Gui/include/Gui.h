#pragma once

#include "ui_form_gui.h"
class MotorWidget;
class SlitWidget;
class FwheelWidget;

#include <QMainWindow>

class Gui : public QMainWindow, private Ui::GuiObject {
    Q_OBJECT

    public:
    Gui(std::string hostname);
    ~Gui();

    protected:
    void closeEvent(QCloseEvent* event);

    private slots:
    void Update();

    private:
    //void Initialization();
    void LayoutWindow();
    void LoadMotorWidgets();
    void LoadFwheelWidgets();
    void Initialization();

    std::string hostname;
    std::vector <MotorWidget*> motorWidgets;
    SlitWidget* slits;
    std::vector <FwheelWidget*> fwheelWidgets;
    bool layoutDone;
};

