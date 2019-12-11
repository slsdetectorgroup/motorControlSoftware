#include "Client.h"
#include "MySocketTCP.h"
#include "commonDefs.h"

#include <cstring>


void Client::Connect(std::string hostname) {
    sock = new MySocketTCP(hostname.c_str(), PORT_NO);
    if (sock) {
        if (sock->Connect() < 0) {
            throw RuntimeError("Could not connect to socket");
        }
    }
}



std::string Client::SendCommand(int argc, char arg[]) {
  char args[TCP_PACKET_LENGTH];
  memset(args, 0, sizeof(args));
  strcpy(args, arg);
  
  sock->SendDataOnly(&argc, sizeof(argc));
  sock->SendDataOnly(args, sizeof(args));

  int ret = OK; 
  sock->ReceiveDataOnly(&ret, sizeof(ret));

  char message[TCP_PACKET_LENGTH];
  memset(message, 0, sizeof(message));
  sock->ReceiveDataOnly(message, sizeof(message));

  if (ret == FAIL) {
    throw std::runtime_error(message);
  }
  return string(message);
}


void Client::Disconnect() {
  sock->Disconnect();
  delete sock;
}
