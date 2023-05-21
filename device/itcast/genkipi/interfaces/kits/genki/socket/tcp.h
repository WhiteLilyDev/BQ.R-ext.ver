
#ifndef GENKIPI_SOCKET_TCP_H
#define GENKIPI_SOCKET_TCP_H

#include <stdint.h>


int genkisock_socket(int domain, int type, int protocol);

int genkisock_bind(int s, uint8_t* ip, int port);

int genkisock_listen(int sockfd, int backlog);

int genkisock_accept(int s, uint8_t* ip, unsigned int *port);

int genkisock_recv(int s, char* buffer, int len);

#endif //GENKIPI_SOCKET_TCP_H
