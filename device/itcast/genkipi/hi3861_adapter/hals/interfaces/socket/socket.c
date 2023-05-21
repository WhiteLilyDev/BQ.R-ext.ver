#include "genki/socket/socket.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "lwip/sockets.h"

//int lwip_socket(int domain, int type, int protocol);
//int lwip_bind(int s, const struct sockaddr *name, socklen_t namelen);

int genkisock_socket(int domain, int type, int protocol) {
    return socket(domain, type, protocol);
}

int genkisock_bind(int s, uint8_t* ip, int port) {

    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);

    memset((void *) &addr, 0, addr_len);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = (ip[3] << 24) + (ip[2] << 16) + (ip[1] << 8) + ip[0];
    addr.sin_port = htons(port);

    return bind(s, (struct sockaddr*)&addr, addr_len);
}

int genkisock_listen(int s, int backlog) {
    return listen(s, backlog);
}

int genkisock_accept(int s, uint8_t* ip, unsigned int *port) {
    memset(ip, 0, 4 * sizeof(uint8_t));
    *port = 0;

    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);

    int fd = accept(s, (struct sockaddr *) &addr, &addr_len);

    if(fd > 0) {
        unsigned long ipv = addr.sin_addr.s_addr;
        *ip++ = (ipv >> 0) & 0xff;
        *ip++ = (ipv >> 8) & 0xff;
        *ip++ = (ipv >> 16) & 0xff;
        *ip++ = (ipv >> 24) & 0xff;

        uint16_t i = addr.sin_port;
        *port = (((i >> 0) & 0xff) << 8) + ((i >> 8) & 0xff);
    }
    return fd;
}

int genkisock_recv(int s, char* buffer, int len) {
    int ret = recv(s, buffer, len, 0);
    return ret;
}

int genkisock_connect(int s, uint8_t* ip, int port) {
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);

    memset((void *) &addr, 0, addr_len);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = (ip[3] << 24) + (ip[2] << 16) + (ip[1] << 8) + ip[0];
    addr.sin_port = htons(port);

    return connect(s, (struct sockaddr*)&addr, addr_len);
}

int genkisock_send(int s, char* buffer, int len) {
    return send(s, buffer, len, 0);
}

int genkisock_close(int s) {
    return close(s);
}

int genkisock_shutdown(int s, int how) {
    return shutdown(s, how);
}

int genkisock_sendto(int s, char* buffer, int len, uint8_t* ip, int port) {
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);

    memset((void *) &addr, 0, addr_len);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = (ip[3] << 24) + (ip[2] << 16) + (ip[1] << 8) + ip[0];
    addr.sin_port = htons(port);

    return sendto(s, buffer, len, 0, (struct sockaddr *)&addr, addr_len);
}

int genkisock_recvfrom(int s, char* buffer, int len, uint8_t* ip, unsigned int *port) {
    memset(ip, 0, 4 * sizeof(uint8_t));
    *port = 0;

    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);

    int ret = recvfrom(s, buffer, len, 0, (struct sockaddr *)&addr, addr_len);
    if(ret > 0) {
        unsigned long ipv = addr.sin_addr.s_addr;
        *ip++ = (ipv >> 0) & 0xff;
        *ip++ = (ipv >> 8) & 0xff;
        *ip++ = (ipv >> 16) & 0xff;
        *ip++ = (ipv >> 24) & 0xff;

        uint16_t i = addr.sin_port;
        *port = (((i >> 0) & 0xff) << 8) + ((i >> 8) & 0xff);
    }
    return ret;
}

int genkisock_setsockopt(int s, int level, int option_name, const void *option_value, unsigned int option_len) {
    return setsockopt(s, level, option_name, option_value, option_len);
}