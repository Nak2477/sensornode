#ifndef TCP_SOCKET_H
#define TCP_SOCKET_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// TCP socket functions
int create_tcp_connection(const char *hostname, int port);
int setup_server_address(struct sockaddr_in *server_addr, const char *hostname, int port);

#endif // TCP_SOCKET_H