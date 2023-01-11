#pragma once

#include "ui_form_gui.h"
class MotorWidget;
class ReferencePointsWidget;
class FluorWidget;
class SlitWidget;
class FwheelWidget;
class PGaugeWidget;
class TubeWidget;
class OptionsWidget;

#include <QMainWindow>

class Gui : public QMainWindow, private Ui::GuiObject {
    Q_OBJECT

  public:
    Gui(std::string hostname);
    ~Gui();

  protected:
    void closeEvent(QCloseEvent *event);

  private slots:
    void LoadPressureWidget(bool userClick = true);
    void EnablePressureWidget(bool enable);
    void LoadTubeWidget(bool userClick = true);
    void EnableTubeWidget(bool enable);
    void Update();
    void ShowOptions();
    void OptionsClosed();
    // void Stop();

  private:
    void LayoutWindow();
    void CheckWindowSize();
    void LoadMotorWidgets();
    void LoadReferencePointsWidget();
    void LoadFwheelWidgets();
    void LoadOptionsWidget();
    void Initialization();

    std::string hostname;
    std::vector<MotorWidget *> motorWidgets;
    ReferencePointsWidget *refpoints;
    std::vector<FluorWidget *> fluorWidgets;
    SlitWidget *slits;
    std::vector<FwheelWidget *> fwheelWidgets;
    PGaugeWidget *pgauge;
    TubeWidget *tube;
    OptionsWidget *optionsWidget;
    bool layoutDone;
    static const int WINDOW_WIDTH_TUBE = 780;
    static const int WINDOW_WIDTH_UNCHECK_TUBE = 465;
    static const int WINDOW_WIDTH_NO_TUBE = 345;
    static const int WINDOW_HEIGHT_TUBE = 505;

    static const int WINDOW_HEIGHT_REFERENCE = 110;
    static const int WINDOW_HEIGHT_PRESSURE = 225;
    static const int WINDOW_HEIGHT_UNCHECK_PRESSURE = 53;

    static const int WINDOW_HEIGHT_MOTOR_REF = 43;
    static const int WINDOW_HEIGHT_MOTOR = 108;

    static const int WINDOW_HEIGHT_REF_POINTS = 90;

    static const int WINDOW_HEIGHT_FWHEEL_REF = 46;
    static const int WINDOW_HEIGHT_FWHEEL = 80;

    static const int WINDOW_HEIGHT_FLUOR_REF = 60;
    static const int WINDOW_HEIGHT_FLUOR = 120;

    static const int WINDOW_HEIGHT_SLITS = 350;
};
