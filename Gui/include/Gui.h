#pragma once

#include "ui_form_gui.h"
class MotorWidget;
class ReferencePointsWidget;
class FluorWidget;
class SlitWidget;
class FwheelWidget;
class PGaugeWidget;
class TubeWidget;

#include <QMainWindow>

class Gui : public QMainWindow, private Ui::GuiObject {
    Q_OBJECT

    public:
    Gui(std::string hostname);
    ~Gui();

    protected:
    void closeEvent(QCloseEvent* event);

    private slots:
    void LoadPressureWidget(bool userClick = true);
    void EnablePressureWidget(bool enable);
    void LoadTubeWidget(bool userClick = true);
    void EnableTubeWidget(bool enable);
    void Update();
    void Stop();

    private:
    void LayoutWindow();
    void LoadMotorWidgets();
    void LoadReferencePointsWidget();
    void LoadFwheelWidgets();
    void Initialization();

    std::string hostname;
    std::vector <MotorWidget*> motorWidgets;
    ReferencePointsWidget* refpoints;
    std::vector <FluorWidget*> fluorWidgets;
    SlitWidget* slits;
    std::vector <FwheelWidget*> fwheelWidgets;
    PGaugeWidget* pgauge;
    TubeWidget* tube;
    bool layoutDone;
};

