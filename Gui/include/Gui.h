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
    //void Stop();

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
    static const int WINDOW_WIDTH_TUBE = 750;
    static const int WINDOW_WIDTH_UNCHECK_TUBE = 460;
    static const int WINDOW_WIDTH_NO_TUBE = 340;
    static const int WINDOW_HEIGHT_TUBE = 520;

    static const int WINDOW_HEIGHT_REFERENCE = 180;
    static const int WINDOW_HEIGHT_PRESSURE = 300;
    static const int WINDOW_HEIGHT_MOTOR = 103;
    static const int WINDOW_HEIGHT_REF_POINTS = 113;
    static const int WINDOW_HEIGHT_FWHEEL = 114;
    static const int WINDOW_HEIGHT_FLUOR = 120;
    static const int WINDOW_HEIGHT_SLITS = 120;




};

