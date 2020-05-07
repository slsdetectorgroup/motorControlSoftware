#pragma once

class MySocketTCP;

#include <string>

class Client {
public:
  Client(std::string hostname);
  std::string SendCommand(int nCommand, std::string command);
  int AddOtherParameters(int nArg, char args[]);
  std::string useSystemCommand(std::string command);

private:
  std::string hostname;
};
