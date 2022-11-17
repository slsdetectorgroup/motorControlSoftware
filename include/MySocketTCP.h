
#ifndef MY_SOCKET_TCP_H
#define MY_SOCKET_TCP_H

#define SEND_REC_MAX_SIZE       4096


#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string>
#include <math.h>

class MySocketTCP {

  public:
    MySocketTCP(std::string host_ip_or_name, int port_number); 
    MySocketTCP(int port_number); 
    ~MySocketTCP();

int ConvertHostnameToInternetAddress (std::string hostname, struct addrinfo **res);
    void Disconnect();
    int Connect();     
    ssize_t SendDataOnly(void *buf, ssize_t length);
    ssize_t ReceiveDataOnly(void *buf, ssize_t length);

  private:
    int is_a_server;
    struct sockaddr_in clientAddress, serverAddress;
    socklen_t clientAddress_length;
    int send_rec_max_size;

    class mySocketDescriptors {
      public:

        /** Constructor */
        mySocketDescriptors():fd(-1), newfd(-1){};
        /** Destructor */
        ~mySocketDescriptors() {
          // close TCP server new socket descriptor from accept
          if (newfd >= 0) {
            close(newfd);
          }
          // close socket descriptor
          if (fd >= 0) {
            close(fd);
          }
        }
        /** socket descriptor */
        int fd;
        /** new socket descriptor in TCP server from accept */
        int newfd;
    };
    mySocketDescriptors sockfd;

};
#endif
