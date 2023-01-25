// SPDX-License-Identifier: LGPL-3.0-or-other
// Copyright (C) 2021 Contributors to the SLS Detector Package
#include "sls/ServerSocket.h"

#include "logger.h"
#include "commonDefs.h"

#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <unistd.h>

namespace sls {

#define DEFAULT_PACKET_SIZE 1286
#define SOCKET_BUFFER_SIZE  (100 * 1024 * 1024) // 100 MB
#define DEFAULT_BACKLOG     5

ServerSocket::ServerSocket(int port)
    : DataSocket(socket(AF_INET, SOCK_STREAM, 0)), serverPort(port) {

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(static_cast<uint16_t>(port));
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(getSocketId(), reinterpret_cast<sockaddr *>(&serverAddr),
             sizeof(serverAddr)) != 0) {
        close();
        throw RuntimeError(
            std::string("Server ERROR: cannot bind socket with port number ") +
            std::to_string(port) +
            std::string(". Please check if another instance is running."));
    }
    if (listen(getSocketId(), DEFAULT_BACKLOG) != 0) {
        close();
        throw std::runtime_error("Server ERROR: cannot  listen to socket");
    }
}

DataSocket ServerSocket::accept() {
    struct sockaddr_in clientAddr;
    socklen_t addr_size = sizeof clientAddr;
    int newSocket =
        ::accept(getSocketId(), reinterpret_cast<sockaddr *>(&clientAddr), &addr_size);
    if (newSocket == -1) {
        throw RuntimeError("Server ERROR: socket accept failed\n");
    }
    char tc[INET_ADDRSTRLEN]{};
    inet_ntop(AF_INET, &(clientAddr.sin_addr), tc, INET_ADDRSTRLEN);
    return DataSocket(newSocket);
}

} // namespace sls
