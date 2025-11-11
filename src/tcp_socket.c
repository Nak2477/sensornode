#include "../include/tcp.h"

// using gethostbyname instead of of getaddrinfo()
int setup_server_address(struct sockaddr_in *server_addr, const char *hostname, int port)
{
    struct hostent *server;
    server = gethostbyname(hostname);
    
    if (server == NULL) {
        fprintf(stderr, "ERROR: Kunde inte hitta host %s\n", hostname);
        return -1;
    }
    
    memset(server_addr, 0, sizeof(*server_addr));
    server_addr->sin_family = AF_INET;
    server_addr->sin_port = htons(port);
    memcpy(&server_addr->sin_addr.s_addr, server->h_addr_list[0], server->h_length);
    
    return 0;
}

int create_tcp_connection(const char *hostname, int port)
{
    int sockfd;
    struct sockaddr_in server_addr;
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("ERROR: Kunde inte skapa socket");
        return -1;
    }
    
    if (setup_server_address(&server_addr, hostname, port) < 0) {
        close(sockfd);
        return -1;
    }
    
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) != 0) {
        perror("ERROR: Kunde inte ansluta");
        close(sockfd);
        return -1;
    }
    
    printf("Ansluten till %s:%d\n", hostname, port);
    return sockfd;
}