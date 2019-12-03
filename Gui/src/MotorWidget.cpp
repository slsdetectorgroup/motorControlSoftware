 /********************************************//**
 * @file MotorWidget.cpp
 * @short uses the form for a motor widget and defines its methods
 * @author Dhanya
 ***********************************************/
#include "MotorWidget.h"

#include "commonDefs.h"

#include <iostream>
using namespace std;


//-------------------------------------------------------------------------------------------------------------------------------------------------

Client * MotorWidget::client= new Client();

//-------------------------------------------------------------------------------------------------------------------------------------------------

QMessageBox * MotorWidget::msgBox;

//-------------------------------------------------------------------------------------------------------------------------------------------------

QMessageBox * MotorWidget::lockMsg;

//-------------------------------------------------------------------------------------------------------------------------------------------------

char MotorWidget::serverHostname[1000];

//-------------------------------------------------------------------------------------------------------------------------------------------------

char MotorWidget::userDetails[255];

//-------------------------------------------------------------------------------------------------------------------------------------------------

MotorWidget* MotorWidget::self;

//-------------------------------------------------------------------------------------------------------------------------------------------------
#ifdef LASERBOX
int MotorWidget::NumMotorWidgets(0);
#endif

MotorWidget::MotorWidget(QWidget *parent, const char *Name):QWidget(parent)
{
  if(strcmp(Name,"static"))
    {
      setupUi(this);
      strcpy(this->Name,Name);  
      Initialization();
      GetPositionFromServer();
    }
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

char* MotorWidget::GetName()
{
  return Name;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

double MotorWidget::GetPosition()
{
   return posDisplay->text().toDouble();
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void MotorWidget::Initialization()
{
  //initializations
  strcpy(this->Name,Name);
  groupbox->setTitle(Name);

  absDisplay->setText("");
  absDisplay->setValidator(new QDoubleValidator(absDisplay));

  QDoubleValidator *rel = new QDoubleValidator(relDisplay);
  rel->setBottom(0);

  relDisplay->setText("1.000");
  relDisplay->setValidator(rel);

  //connections
  connect(minus,SIGNAL(clicked()),this,SLOT(MoveNegRelative()));
  connect(plus,SIGNAL(clicked()),this,SLOT(MovePosRelative()));   
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void MotorWidget::GetPositionFromServer()
{
  char message[200] = "gui pos ";
  strcat(message,Name);
  strcpy(message,SendCommand(3,message));
  posDisplay->setText(QString(message));
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void MotorWidget::UpdatePosition(char Position[],bool relative)
{
  char absPosition[200];
  if(relative)
    {
      double pos = atof(Position) + posDisplay->text().toDouble();
      sprintf(absPosition," %f ",pos);
    }
  else
    {
      //this is to set modified to false
      absDisplay->setModified(false);
      strcpy(absPosition,Position);
    }

  posDisplay->setText(QString(absPosition));
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void MotorWidget::Disable()
{
  posDisplay->setText("");
  absDisplay->setText("");
  relDisplay->setText("");
  groupbox->setEnabled(false);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

bool MotorWidget::IsEnabled()
{
  return groupbox->isEnabled();
}

//-------------------------------------------------------------------------------------------------------------------------------------------------


char* MotorWidget::MoveAbsolute(bool *modified)
{
  char position[200] = "";
  char *p = position;
  if ((absDisplay->isModified())&&(QString::compare(absDisplay->text(), "")))
    {
      *modified=true;  
      sprintf(position,absDisplay->text().toAscii().constData());   
     }
  return p;
}


//-------------------------------------------------------------------------------------------------------------------------------------------------

void MotorWidget::MovePosRelative()
{
  MoveRelative(true);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void MotorWidget::MoveNegRelative()
{
  MoveRelative(false);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void MotorWidget::MoveRelative(bool positive)
{
  char message[200] = "gui moverel ";
  char charPos[200]="";
  QString text = relDisplay->text();
  double pos = text.toDouble();

  if(!QString::compare(text, ""))
    ErrorMessage("ERROR: Please enter a value for the relative position");
  else
    {

      QPushButton *clickedButton = qobject_cast<QPushButton *>(sender());
      
      // if(!QString::compare(clickedButton->text(), "-"))
      if(!positive)
	      pos=0-pos;  

      sprintf(charPos," %f ",pos);
      strcat(message,Name);
      strcat(message,charPos);
      strcpy(message,SendCommand(4,message));
      if(strstr (message,"ERROR")==NULL)
	{
	  UpdatePosition(charPos,1);
	  emit RelativeMoved();

#ifndef LASERBOX
	  if (groupbox->title().contains("slit", Qt::CaseInsensitive))
	  emit SlitsMoved();
#endif
	}
      else
	ErrorMessage(message);
    }
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

char* MotorWidget::SendCommand(int argc, char args[])
{
  char buffer[255]="",userName[255],pcName[255], currTime[255];
  char sargs[255];
  //just to avoid warning for returning address of local variable
  char *p=buffer;
  

  if(!strcmp(serverHostname,"")) 
    strcpy(serverHostname,Server_Hostname);

  //save userDetais only once
  if(!strcmp(userDetails,"")) 
    {
	  strcpy(userName,useSystemCommand("whoami"));
	  strcpy(userName,useSystemCommand("whoami"));
	  strcpy(pcName,useSystemCommand("uname -n"));
	  strcpy(pcName,useSystemCommand("uname -n"));
	  sprintf(userDetails,"%s %s %d",userName,pcName,getpid());
	  /*DEBUGGING
      cout<<endl<<"userDetails:"<<userDetails<<"."<<endl;
      */
    }

  //include userdetails, timestamp and commands to send ot server
  strcpy(currTime,useSystemCommand("date"));
  sprintf(sargs,"%s %s %s ",args,userDetails,currTime);
  argc+=4;
  //cout<<"hostname:"<<serverHostname<<" argc:"<<argc<<" sargs:"<<sargs<<"."<<endl;

  //connect socket and send message
  if(client->Connect(serverHostname))
    {
      strcpy(buffer,client->SendCommand(argc,sargs));
      client->Disconnect();
    }
  else
    {
      ErrorMessage("ERROR: Cannot connect to server");
      exit(-1);
    }

  //if there is a conflict betweeen users
#ifdef XRAYBOX
  if(strstr (buffer,"ERROR:The x-ray box is in use by ")!=NULL)
#elif LASERBOX
  if(strstr (buffer,"ERROR:The laser box is in use by ")!=NULL)
#else
  if(strstr (buffer,"ERROR:The vacuum box is in use by ")!=NULL)
#endif
    {
      strcat(buffer,"\nUnlock the previous user and update your GUI?");

      //choice to unlock previous user
      lockMsg = new QMessageBox(QMessageBox::Warning, "X-Ray tube LOCK",tr(buffer),QMessageBox::Ok,lockMsg);
      QPushButton *cancelButton = lockMsg->addButton(QMessageBox::Cancel);
      lockMsg->setDefaultButton(cancelButton);
      int ret = lockMsg->exec();
      if (ret == QMessageBox::Ok)
	{
	  strcpy(buffer,SendCommand(2,"gui unlock"));
	  strcat(buffer,"\nStarting up/Updating GUI in a few seconds...");
	  ErrorMessage(buffer);
	  //if this was done at the start of gui, let it continue
	  if(strstr (args,"gui list")!=NULL)
	    strcpy(buffer,SendCommand(2,"gui list"));
	  //not done at start of gui, so have to update gui
	  else
	    {
	      cout<<flush;
	      strcpy(buffer,"ERROR:Your previous command was unsuccessful due to the update process. You may execute your commands now.");
	      self->emitUpdateGUISignal();
	    }
	}
      else
	{
	  strcpy(buffer,"You have chosen to let the other user continue.\nExiting the GUI...\n");
	  ErrorMessage(buffer);
	  exit(-1);
	}
    }
  else if(strstr (buffer,"ERROR:Another user had updated the server")!=NULL)
    {
      cout<<flush;
      ErrorMessage(buffer);
      strcpy(buffer,"ERROR:Your previous command was unsuccessful due to the update process. You may execute your commands now.");
      self->emitUpdateGUISignal();
    }


return p;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void MotorWidget::ChangeHostname(char* host_ip_or_name)
{
  strcpy(serverHostname,host_ip_or_name);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------


char* MotorWidget::useSystemCommand(char* systemCommand)
{
  const char *WHITESPACE=" \t\n\r";
  char *result;
  char output[255];

  //using sys cmds to get output or str
  FILE* sysFile = popen(systemCommand, "r");
  fgets(output, sizeof(output), sysFile);
  pclose(sysFile);

  if(!strcmp(systemCommand,"date"))
    {
      result = output + 0;
      result[29]=0;
    }
  else
    {
      //remove trailing spaces
      int spacesAtStart = strspn(output, WHITESPACE);//length of initial portion of str1 which matches str2 ,here 0
      result = output + spacesAtStart;
      int lengthOfNonSpace = strcspn(result, WHITESPACE);//position of where str1 does not match str2
      result[lengthOfNonSpace] = 0;
    }
  //to flush,else time has lots of trailing spaces
  //cout<<flush;//endl;
  return result;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void  MotorWidget::ErrorMessage(char errorMessage[])
{
 
  msgBox = new QMessageBox(QMessageBox::Warning, "WARNING",tr(errorMessage), 0, msgBox);
  msgBox->addButton(tr("OK"), QMessageBox::AcceptRole);
  msgBox->exec();
}

//-------------------------------------------------------------------------------------------------------------------------------------------------
