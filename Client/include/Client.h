#pragma once

class MySocketTCP;

#include <string>

class Client {

 public:
  void Connect(std::string hostname);
  std::string SendCommand(int argc, char arg[]);
  void Disconnect();

 private:
  MySocketTCP *sock;
};


