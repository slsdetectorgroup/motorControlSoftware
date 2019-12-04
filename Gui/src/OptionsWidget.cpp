 /********************************************//**
 * @file OptionsWidget.cpp
 * @short creates the menu and widgets for the options menu
 * @author Dhanya
 ***********************************************/
#include "OptionsWidget.h"
#include <iostream>
using namespace std;

//-------------------------------------------------------------------------------------------------------------------------------------------------

OptionsWidget::OptionsWidget(QWidget *parent,vector<string> list):QFrame(parent,Qt::Popup|Qt::SubWindow),current(-1)
{
  setupUi(this);
  this->list=list;

  LayoutWindow();
  Initialization();

}
//-------------------------------------------------------------------------------------------------------------------------------------------------

void OptionsWidget::LayoutWindow()
{
  for(int i=0;i<(int)list.size();i++)
    {
      motorName->addItem(QString(list[i].c_str()));
      optionWidgets[i] = new MotorWidget(optionsFrame,list[i].c_str());
      optionWidgets[i]->hide();
    }

  if(motorName->count()<=(int)this->list.size()) 
    {
      motorName->addItem("Please select");
      motorName->setCurrentIndex((int)list.size());
    }
  motorBox->setEnabled(false);
  motorBox->hide();

  setWindowTitle(tr("Options Menu"));

}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void OptionsWidget::Initialization()
{
  posInput->setValidator(new QDoubleValidator(posInput));
  upperInput->setValidator(new QDoubleValidator(upperInput));
  lowerInput->setValidator(new QDoubleValidator(lowerInput));

  QDoubleValidator *noNegative = new QDoubleValidator(speedInput);
  noNegative->setBottom(0);
  speedInput->setValidator(noNegative);

  //motorName
  connect(motorName,SIGNAL(currentIndexChanged(int)),this,SLOT(ChangeMotorWidget(int)));

  //options
  connect(moveAbs,SIGNAL(clicked()),this,SLOT(MoveAbsolute()));
  connect(home,SIGNAL(clicked()),this,SLOT(MoveToHome()));
  connect(setPos,SIGNAL(clicked()),this,SLOT(SetOption()));
  connect(setSpeed,SIGNAL(clicked()),this,SLOT(SetOption()));
  connect(setUpper,SIGNAL(clicked()),this,SLOT(SetOption()));
  connect(setLower,SIGNAL(clicked()),this,SLOT(SetOption()));

  //ok button
  connect(done,SIGNAL(clicked()),this,SLOT(close()));

}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void OptionsWidget::ChangeMotorWidget(int index)
{
  motorBox->setTitle(list[index].c_str());

  /*
  for(int i=0;i<motorName->count();i++)
    cout<<i<<":"<<motorName->itemText(i).toStdString()<<endl;
  */

  if(motorName->count()>(int)this->list.size()) 
	{
	  motorName->removeItem((int)this->list.size());
	  motorBox->setEnabled(true);
	  motorBox->show();
	}
      else
	optionWidgets[current]->hide();
  
  optionWidgets[index]->show();
  if(  (!QString::compare( motorName->currentText(), "fluorescence",Qt::CaseInsensitive))||
       (!QString::compare( motorName->currentText(), "slit_x1",Qt::CaseInsensitive)) ||
       (!QString::compare( motorName->currentText(), "slit_x2",Qt::CaseInsensitive)) )
    {
      setPos->setEnabled(false);
      posInput->setEnabled(false);
    }
  else
    {
      setPos->setEnabled(true);
      posInput->setEnabled(true);
      posInput->setModified(false);
    }
  speedInput->setText("");
  upperInput->setText("");
  lowerInput->setText("");

  current=index;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void OptionsWidget::MoveAbsolute()
{
  char charPos[200],command[200]; 
  bool modified = false;
  strcpy(charPos,optionWidgets[current]->MoveAbsolute(&modified));

  if(modified)
    {
      sprintf(command,"gui moveabs %s %s ",list[current].c_str(),charPos);

      strcpy(command,MotorWidget::SendCommand(4,command));

      if(strstr (command,"ERROR")==NULL)
	optionWidgets[current]->UpdatePosition(charPos,0);
      else
	MotorWidget::ErrorMessage(command);
    }
}


//-------------------------------------------------------------------------------------------------------------------------------------------------

void OptionsWidget::MoveToHome()
{
  char message[200]="gui cal "; 
  strcat(message,list[current].c_str());
  strcpy(message,MotorWidget::SendCommand(3,message));
  if(strstr (message,"ERROR")==NULL)
    optionWidgets[current]->UpdatePosition((char*)"0",0);
  else
    MotorWidget::ErrorMessage(message);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void OptionsWidget::SetOption()
{
  QString text;
  char message[200];
  char motor[200];

  strcpy(motor,motorName->currentText().toStdString().c_str());
  
  QPushButton *clickedButton = qobject_cast<QPushButton *>(sender());
  if(!QString::compare(clickedButton->text(), "Set Position"))
    {
      char charPos[200];
      text = posInput->text();
      sprintf(charPos,"%f",text.toDouble());
      
      if(!QString::compare(text, ""))
	MotorWidget::ErrorMessage((char*)"ERROR: Please enter a value to set the position");
      else
	{
	  if(posInput->isModified())
	    {
	      sprintf(message,"gui setpos %s %s ",motor,text.toStdString().c_str());
	      strcpy(message,MotorWidget::SendCommand(4,message));
	      if(strstr (message,"ERROR")==NULL)
		{
		  posInput->setModified(false);
		  //update position
		  optionWidgets[current]->UpdatePosition(charPos,0); 
		}
	      MotorWidget::ErrorMessage(message);
	    }
	}
    }
  else if(!QString::compare(clickedButton->text(), "Set Speed"))
    {
      text = speedInput->text();
      if(!QString::compare(text, ""))
	MotorWidget::ErrorMessage((char*)"ERROR: Please enter a value to set the speed");
      else
	{
	  if(speedInput->isModified())
	    {


	      sprintf(message,"gui setspeed %s %s ",motor,text.toStdString().c_str());
	      strcpy(message,MotorWidget::SendCommand(4,message));
	      if(strstr (message,"ERROR")==NULL)
		speedInput->setModified(false);
	      MotorWidget::ErrorMessage(message);	     
	    }
	}
    }
  else if(!QString::compare(clickedButton->text(), "Set Upper Limit"))
    {
      text = upperInput->text();
      if(!QString::compare(text, ""))
	MotorWidget::ErrorMessage((char*)"ERROR: Please enter a value to set the upper limit");
      else
	{
	  if(upperInput->isModified())
	    {
	      sprintf(message,"gui setupper %s %s ",motor,text.toStdString().c_str());
	      strcpy(message,MotorWidget::SendCommand(4,message));
	      if(strstr (message,"ERROR")==NULL)
		{
		  upperInput->setModified(false);
#ifndef LASERBOX
		  emit UpperLimitChanged();
#endif
		}
	      MotorWidget::ErrorMessage(message);	     
	    }
	}
    }
  else
    {
      text = lowerInput->text();
      if(!QString::compare(text, ""))
	MotorWidget::ErrorMessage((char*)"ERROR: Please enter a value to set the lower limit");
      else
	{
	  if(lowerInput->isModified())
	    {
	      sprintf(message,"gui setlower %s %s ",motor,text.toStdString().c_str());
	      strcpy(message,MotorWidget::SendCommand(4,message));
	      if(strstr (message,"ERROR")==NULL)
		{
		  lowerInput->setModified(false);
#ifndef LASERBOX
		  emit LowerLimitChanged();
#endif
		}
	      MotorWidget::ErrorMessage(message);	     
	    }
	}
    }
}

//-------------------------------------------------------------------------------------------------------------------------------------------------


void OptionsWidget::closeEvent(QCloseEvent* event)
{
  emit OptionsClosed();
  event->accept();
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void OptionsWidget::keyPressEvent(QKeyEvent* event)
{
  if((event->key() == Qt::Key_Return) ||(event->key() == Qt::Key_Enter))
    MoveAbsolute();
  else
    event->ignore();
}
//-------------------------------------------------------------------------------------------------------------------------------------------------

