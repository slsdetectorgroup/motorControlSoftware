  /********************************************//**
 * @file MotorControlClient.h
 * @short connects/disconnects to server and sends the command to server
 * @author Dhanya
 ***********************************************/
#ifndef Motor_Control_Client_H
#define Motor_Control_Client_H

#include "MySocketTCP.h"

/**
  *@short connects/disconnects to server and sends the command to server
  */
class MotorControlClient
{
 public:

  /**Connect to server
         @param host_ip_or_name hostname or ipname of server to connect to
	 returns 1 if it conencts, else 0
   */
  int Connect(char* host_ip_or_name);

  /**Sends the command to server
	 @param argc number of arguments in command
	 @param arg[] arguments of command concatenated 
	 returns the message after executing command
   */
  char* SendCommand(int argc, char arg[]);

  /**Disconnects from server
   */
  void Disconnect();


 private:

  /**Socket reference
   */
  MySocketTCP *sock;

  /**After executing command, 0 for success and -1 for failure
   */
  int ret;

  /**After executing command,message received
   */
  char message[255];
};

#endif
