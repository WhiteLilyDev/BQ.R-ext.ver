#include "genki/socket/tcp.h"

//#include <stdio.h>
//#include <string.h>
//#include <unistd.h>
//#include "lwip/sockets.h"
//
////int lwip_socket(int domain, int type, int protocol);
////int lwip_bind(int s, const struct sockaddr *name, socklen_t namelen);
//
//int genkisock_socket(int domain, int type, int protocol) {
//    printf("domain: %d\r\n", domain);
//    printf("type: %d\r\n", type);
//    printf("protocol: %d\r\n", protocol);
//    int ret = socket(domain, type, protocol);
//    printf("ret: %d\r\n", ret);
//    return ret;
//}
//
//int genkisock_bind(int s, uint8_t* ip, int port) {
//
//    struct sockaddr_in addr;
//    socklen_t addr_len = sizeof(addr);
//
//    memset((void *) &addr, 0, addr_len);
//    addr.sin_family = AF_INET;
//    addr.sin_addr.s_addr = (ip[3] << 24) + (ip[2] << 16) + (ip[1] << 8) + ip[0];
//    addr.sin_port = htons(port);
//
//    printf("s: %d\r\n", s);
//    printf("ip: %d.%d.%d.%d\r\n", ip[0], ip[1], ip[2], ip[3]);
//    printf("ip: %ld\r\n", addr.sin_addr.s_addr);
//    printf("port: %d\r\n", port);
//
//    return bind(s, (struct sockaddr*)&addr, addr_len);
//}
//
//int genkisock_listen(int s, int backlog) {
//    return listen(s, backlog);
//}
//
//int genkisock_accept(int s, uint8_t* ip, unsigned int *port) {
//    memset(ip, 0, 4 * sizeof(uint8_t));
//    *port = 0;
//
//    struct sockaddr_in addr;
//    socklen_t addr_len = sizeof(addr);
//
//    int fd = accept(s, (struct sockaddr *) &addr, &addr_len);
//
//    if(fd > 0) {
//        unsigned long ipv = addr.sin_addr.s_addr;
//        *ip++ = (ipv >> 0) & 0xff;
//        *ip++ = (ipv >> 8) & 0xff;
//        *ip++ = (ipv >> 16) & 0xff;
//        *ip++ = (ipv >> 24) & 0xff;
//
//        uint16_t i = addr.sin_port;
//        *port = (((i >> 0) & 0xff) << 8) + ((i >> 8) & 0xff);
//    }
//    return fd;
//}
//
//int genkisock_recv(int s, char* buffer, int len) {
//    printf("len: %d\r\n", len);
//    int ret = recv(s, buffer, len, 0);
//    printf("ret: %d\r\n", ret);
//    return ret;
//}
