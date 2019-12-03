#include "Client.h"

#include "commonDefs.h"
#include <iostream>
#include <cstring>
using namespace std;


//-------------------------------------------------------------------------------------------------------------------------------------------------


int Client::Connect(char* host_ip_or_name)
{
#ifdef VERY_VERBOSE
  cout<< "\nInitializing socket interface\n";
#endif
  
  // creates the socket
  sock = new MySocketTCP(host_ip_or_name,PORT_NO);
  
  // If it connects to server, return 1, else 0
  if(sock)
    if(sock->Connect()>=0)
      {
#ifdef VERY_VERBOSE
	cout<<"\n--- connected\n";
#endif
	return 1;
      }

  return 0;
}


//-------------------------------------------------------------------------------------------------------------------------------------------------


char* Client::SendCommand(int argc, char arg[])
{
  char args[255];
  strcpy(args,arg);
  
#ifdef VERY_VERBOSE
  cout<<"\nSending "<<args<<" to socket interface\n";
#endif
  // sends the number of arguments in command line
  sock->SendDataOnly(&argc, sizeof(argc));
#ifdef VERY_VERBOS
  cout<<"--- send argc: "<<argc<<endl;
#endif

  // sends the arguments
  sock->SendDataOnly(args, sizeof(args));
#ifdef VERY_VERBOSE
  cout<<"--- send args: "<<args<<endl;
#endif

  // receives the ret from server
  sock->ReceiveDataOnly(&ret, sizeof(ret));
#ifdef VERY_VERBOSE
  cout<<"--- receive worked?: "<<ret<<endl;
#endif

  // recceives the message from the server
  sock->ReceiveDataOnly(message, sizeof(message));
#ifdef VERY_VERBOSE
  cout<<"--- received: "<<message<<endl;
#endif

  return message;

}
//-------------------------------------------------------------------------------------------------------------------------------------------------


void Client::Disconnect()
{
  // disconnects socket
  sock->Disconnect();

#ifdef VERY_VERBOSE
  cout<<"--- disconnected\n\n";
#endif

  // deletes sock
  delete sock;

#ifdef VERY_VERBOSE
  cout<<"\nClosing socket interface\n\n";
#endif

}


//-------------------------------------------------------------------------------------------------------------------------------------------------
