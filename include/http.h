#ifndef HTTP_H
#define HTTP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>

#define SERVER_HOST "httpbin.org"
#define SERVER_PORT 80

#define BUFFER_SIZE 1024
#define MAX_RESPONSE_SIZE 4096

// HTTP response structure
typedef struct {
    int status_code;
    char *headers;
    char *body;
    size_t body_length;
} http_response_t;

// HTTP request functions
char* build_http_request(const char *path, const char *hostname, 
                        const char *headers, const char *body);
char* receive_http_response(int sockfd);
http_response_t* parse_http_response(char *response_buffer);
void print_http_response(const http_response_t *response);
void free_http_response(http_response_t *response);

// High-level HTTP client functions
http_response_t* send_http_post_with_response(const char *hostname, int port, const char *path, 
                                            const char *headers, const char *body);

#endif // HTTP_H