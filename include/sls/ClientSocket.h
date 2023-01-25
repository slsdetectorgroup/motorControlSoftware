// SPDX-License-Identifier: LGPL-3.0-or-other
// Copyright (C) 2021 Contributors to the SLS Detector Package
#pragma once
#include "sls/DataSocket.h"
#include <netdb.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>

namespace sls {

class ClientSocket : public DataSocket {
  public:
    ClientSocket(const std::string &hostname,
                 uint16_t port_number);
    ClientSocket(struct sockaddr_in addr);

  private:
    struct sockaddr_in serverAddr;
};

} // namespace sls
