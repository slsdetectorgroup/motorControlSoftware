 /********************************************//**
 * @file PGaugeWidget.h
 * @short uses the form for a PGauge widget and defines its methods
 * @author Dhanya
 ***********************************************/
#ifndef PGAUGEWIDGET_H
#define PGAUGEWIDGET_H
#ifdef VACUUMBOX

#include<QtGui>
#include "ui_form_pgaugewidget.h"
#include "MotorWidget.h"

/**
 *@short uses the form for a PGauge widget and defines its methods
 */

class PGaugeWidget:public QWidget, private Ui::PGaugeWidgetObject
{
  Q_OBJECT
  
    public: 

 
  /**Constructor
     creates the PGauge widget using the form
     @param parent sets its parent widget to the one it was called from
  */
  PGaugeWidget(QWidget *parent);

  private slots:

  /**
   * Stop updating pressure
   */
  void StopRefresh();

  /**
   * Refresh pressure
   */
  void TimeoutRefresh();


  /**gets the position from the server and displays it
   * @returns 1 for success, 0 for fail
   */
  int UpdateValueFromServer();



    private:

  /**
   * Connect signals to slots
   */
  void Initialization();

  /* red palette */
  QPalette* red;

  /** normal palette */
  QPalette normal;

  /** timer */
  QTimer* timer;

  /** time interval to udpate */
  static const int TIMEOUT = 5000;

  /**
   * Ensure both not calling same function
   */
  bool updating;


};

#endif
#endif