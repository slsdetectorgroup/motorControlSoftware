
// version 1.0, base development, Ian 19/01/09

#include "MySocketTCP.h"
#include "commonDefs.h"
#include "logger.h"

#include <errno.h>
#include <iostream>
#include <math.h>
#include <sstream>
#include <stdio.h>
#include <string>
#include <cstring>


#define DEFAULT_BACKLOG 5

MySocketTCP::~MySocketTCP() {
    /*Disconnect();
    if (socketDescriptor >= 0) {
        close(socketDescriptor);
    }
    file_des = -1;*/
}

MySocketTCP::MySocketTCP(int port_number)
    : is_a_server(1), send_rec_max_size(SEND_REC_MAX_SIZE) { 

    memset(&serverAddress, 0, sizeof(serverAddress));
	memset(&clientAddress, 0, sizeof(clientAddress));
    clientAddress_length = sizeof(clientAddress);
    
    sockfd.fd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd.fd < 0) {
        sockfd.fd = -1;
        std::ostringstream os;
        os << "Can not create socket on port " << port_number;
        throw RuntimeError(os.str());
    } 

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port_number);
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);

    // reuse port
    int val = 1;
    if (setsockopt(sockfd.fd, SOL_SOCKET, SO_REUSEADDR, &val,
                    sizeof(int)) == -1) {
        sockfd.fd = -1;
        std::ostringstream os;
        os << "Can not create socket on port " << port_number << ". setsockopt REUSEADDR failed";
        throw RuntimeError(os.str());
    }

    if (bind(sockfd.fd, reinterpret_cast<sockaddr *>(&serverAddress),sizeof(serverAddress)) < 0) {
        sockfd.fd = -1;
        std::ostringstream os;
        os << "Can not bind socket on port " << port_number;
        throw RuntimeError(os.str());
    } 
    
    listen(sockfd.fd, DEFAULT_BACKLOG);
}

MySocketTCP::MySocketTCP(std::string host_ip_or_name, int port_number)
    : is_a_server(0), send_rec_max_size(SEND_REC_MAX_SIZE) { 

    memset(&serverAddress, 0, sizeof(serverAddress));
	memset(&clientAddress, 0, sizeof(clientAddress));
    clientAddress_length = sizeof(clientAddress);

    struct addrinfo *result;
    if (ConvertHostnameToInternetAddress(host_ip_or_name, &result) == FAIL) {
        sockfd.fd = -1;
        throw RuntimeError("Could not get ip from hostname");
    }
  
    // Set some fields in the serverAddress structure
    sockfd.fd = 0;
    serverAddress.sin_family = static_cast<short>(result->ai_family);
    memcpy(reinterpret_cast<char *>(&serverAddress.sin_addr.s_addr), &(reinterpret_cast<sockaddr_in *>(result->ai_addr))->sin_addr, sizeof(in_addr_t));
    freeaddrinfo(result);
    serverAddress.sin_port = htons(port_number);
}

int MySocketTCP::ConvertHostnameToInternetAddress (std::string hostname, struct addrinfo **res) {
    // criteria in selecting socket address structures returned by res
    struct addrinfo hints;
    memset (&hints, 0, sizeof (hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    // get host info into res
    int errcode = getaddrinfo (hostname.c_str(), NULL, &hints, res);
    if (errcode != 0) {
        LOG(logERROR) << "Could not convert " << hostname << " to IP : " << gai_strerror(errcode);
        return FAIL;
    }
    if (*res == NULL) {
        LOG(logERROR) << "Could not convert " << hostname << " to IP : gettaddrinfo returned null";
        return FAIL;
    } 
    return OK;
}

void MySocketTCP::Disconnect() {
    if (is_a_server) {
        if (sockfd.newfd >= 0) {
            close(sockfd.newfd);
            sockfd.newfd = -1;
        }
        return;
    }
    if (sockfd.fd >= 0) {
        close(sockfd.fd);
        sockfd.fd = -1;
    }
}

int MySocketTCP::Connect() {
	if (sockfd.newfd > 0) 
        return sockfd.newfd;

    if (is_a_server) {
        if (sockfd.fd > 0) {
            if ((sockfd.newfd = accept(sockfd.fd,reinterpret_cast<sockaddr *>(&clientAddress), &clientAddress_length)) < 0) {
                LOG(logERROR) << "Connection refused";
                switch (errno) {
                case EWOULDBLOCK:
                    printf("ewouldblock eagain\n");
                    break;
                case EBADF:
                    printf("ebadf\n");
                    break;
                case ECONNABORTED:
                    printf("econnaborted\n");
                    break;
                case EFAULT:
                    printf("efault\n");
                    break;
                case EINTR:
                    printf("eintr\n");
                    break;
                case EINVAL:
                    printf("einval\n");
                    break;
                case EMFILE:
                    printf("emfile\n");
                    break;
                case ENFILE:
                    printf("enfile\n");
                    break;
                case ENOTSOCK:
                    printf("enotsock\n");
                    break;
                case EOPNOTSUPP:
                    printf("eOPNOTSUPP\n");
                    break;
                case ENOBUFS:
                    printf("ENOBUFS\n");
                    break;
                case ENOMEM:
                    printf("ENOMEM\n");
                    break;
                case ENOSR:
                    printf("ENOSR\n");
                    break;
                case EPROTO:
                    printf("EPROTO\n");
                    break;
                default:
                    printf("unknown error\n");
                }
            } else {
                LOG(logDEBUG) << "client connected " << sockfd.newfd;
            }
        }
        LOG(logDEBUG) << "fd " << sockfd.newfd;
        return sockfd.newfd;
    } 
    
    // client
    if (sockfd.fd <= 0)
        sockfd.fd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd.fd < 0) {
        throw RuntimeError("Cannot create socket");
    } else {
        if (connect(sockfd.fd, reinterpret_cast<sockaddr *>(&serverAddress),sizeof(serverAddress))<0) {
            throw RuntimeError("Cannot connect to socket");
        } 
    }
    return sockfd.fd;
}



ssize_t MySocketTCP::SendDataOnly(void *buf, ssize_t length) {
    if (buf == NULL) 
        return -1;

    int tcpfd = (is_a_server ? sockfd.newfd : sockfd.fd);
    if (tcpfd < 0) 
        return -1;

    if (length == 0) {
        return 0;
    }

    ssize_t total_sent = 0;
    while (length > 0) {
        ssize_t nsending =
            (length > send_rec_max_size) ? send_rec_max_size : length;
        ssize_t nsent =
            write(tcpfd, reinterpret_cast<char*>(buf) + total_sent, nsending);
        if (is_a_server && nsent < 0) {
            LOG(logERROR) << "Could not write to socket. Possible client socket crash";
            break;
        }
        if (nsent <= 0)
            break;
        length -= nsent;
        total_sent += nsent;
    }
    LOG(logDEBUG) << "sent " << total_sent << "bytes";
    return total_sent;
}


ssize_t MySocketTCP::ReceiveDataOnly(void *buf, ssize_t length) {
    if (buf == NULL) 
        return -1;

    int tcpfd = (is_a_server ? sockfd.newfd : sockfd.fd);
    if (tcpfd < 0) 
        return -1;

    if (length == 0) {
        return 0;
    }


    ssize_t total_received = 0;
    while (length > 0) {
        ssize_t nreceiving =
            (length > send_rec_max_size) ? send_rec_max_size : length;
        ssize_t nreceived = read(tcpfd,reinterpret_cast<char*>(buf) + total_received, nreceiving);
        if (nreceived <= 0) {
            if (!total_received) {
                LOG(logERROR) << "Possible socket crash?";
                return -1;
            }
            break;
        }
        length -= nreceived;
        total_received += nreceived;
    }
    LOG(logDEBUG) << "received " << total_received << "bytes";
    return total_received;
}

