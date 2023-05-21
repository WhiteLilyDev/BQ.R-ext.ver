
#ifndef GENKIPI_SOCKET_H
#define GENKIPI_SOCKET_H

#include <stdint.h>


int genkisock_socket(int domain, int type, int protocol);

int genkisock_bind(int s, uint8_t* ip, int port);

int genkisock_listen(int sockfd, int backlog);

int genkisock_accept(int s, uint8_t* ip, unsigned int *port);

int genkisock_recv(int s, char* buffer, int len);

int genkisock_connect(int s, uint8_t* ip, int port);

int genkisock_send(int s, char* buffer, int len);

int genkisock_close(int s);

int genkisock_shutdown(int s, int how);

int genkisock_sendto(int s, char* buffer, int len, uint8_t* ip, int port);

int genkisock_recvfrom(int s, char* buffer, int len, uint8_t* ip, unsigned int *port);

int genkisock_setsockopt(int s, int level, int option_name, const void *option_value, unsigned int option_len);

#endif //GENKIPI_SOCKET_H
