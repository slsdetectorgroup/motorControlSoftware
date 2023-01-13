// SPDX-License-Identifier: LGPL-3.0-or-other
// Copyright (C) 2021 Contributors to the SLS Detector Package
#include "sls/ClientSocket.h"
#include "logger.h"
#include "commonDefs.h"

#include <arpa/inet.h>
#include <cassert>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <unistd.h>
namespace sls {

ClientSocket::ClientSocket(const std::string &host,
                           uint16_t port)
    : DataSocket(socket(AF_INET, SOCK_STREAM, 0)) {

    struct addrinfo hints, *result;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags |= AI_CANONNAME;

    if (getaddrinfo(host.c_str(), nullptr, &hints, &result) != 0) {
        std::string msg = "ClientSocket cannot decode host:" + host +
                          " on port " + std::to_string(port) + "\n";
        throw RuntimeError(msg);
    }

    // TODO! Erik, results could have multiple entries do we need to loop
    // through them? struct sockaddr_in serverAddr {};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);

    memcpy(reinterpret_cast<char *>(&serverAddr.sin_addr.s_addr),
           &(reinterpret_cast<sockaddr_in *>(result->ai_addr))->sin_addr,
           sizeof(in_addr_t));
    if (::connect(getSocketId(), reinterpret_cast<sockaddr *>(&serverAddr),
                  sizeof(serverAddr)) != 0) {
        freeaddrinfo(result);
        std::string msg = "ClientSocket: Cannot connect to server:" + 
            host + " on port " + std::to_string(port) +
                          "\n";
        throw RuntimeError(msg);
    }
    freeaddrinfo(result);
}

ClientSocket::ClientSocket(struct sockaddr_in addr)
    : DataSocket(socket(AF_INET, SOCK_STREAM, 0)) {

    if (::connect(getSocketId(), reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) != 0) {
        char address[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &addr.sin_addr, address, INET_ADDRSTRLEN);
        std::string msg = "ClientSocket: Cannot connect to server:" +
                          std::string(address) + " on port " +
                          std::to_string(addr.sin_port) + "\n";
        throw RuntimeError(msg);
    }
}

}; // namespace sls
