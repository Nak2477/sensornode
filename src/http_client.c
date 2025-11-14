#define _GNU_SOURCE
#include "../include/http.h"
#include "../include/tcp.h"
#include <sys/time.h>

char* build_http_request(const char *path, const char *hostname, const char *headers, const char *body)
{
    char *request = malloc(BUFFER_SIZE +1);
    if (!request) {
        return NULL;
    }

    if (!body) {
        body = "";
    }
    int content_length = strlen(body);

    snprintf(request, BUFFER_SIZE,
        "POST %s HTTP/1.1\r\n"
        "Host: %s\r\n"
        "%s\r\n"  // headers (inkl Content-Type)
        "Content-Length: %d\r\n"
        "\r\n"
        "%s", // Body
        path, hostname, 
        headers ? headers : "Content-Type: application/json",
        content_length, 
        body);

    return request;
}

char* receive_http_response(int sockfd)
{
    char *response_buffer = malloc(MAX_RESPONSE_SIZE);
    if (!response_buffer) {
        fprintf(stderr, "ERROR: Kunde inte allokera minne för svar\n");
        return NULL;
    }
    
    ssize_t total_received = 0;
    ssize_t bytes_received;
    
    struct timeval timeout;
    timeout.tv_sec = 5;  // 5 sekunder timeout 
    timeout.tv_usec = 0;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    
    while (total_received < MAX_RESPONSE_SIZE - 1) {
        bytes_received = recv(sockfd, response_buffer + total_received, 
                            MAX_RESPONSE_SIZE - total_received - 1, 0);
        
        if (bytes_received < 0) {
            perror("recv");
            break;
        } else if (bytes_received == 0) {
            break;
        }
        
        total_received += bytes_received;
        
        response_buffer[total_received] = '\0';
        if (strstr(response_buffer, "\r\n\r\n")) {
            break;
        }
    }
    
    response_buffer[total_received] = '\0';
    return response_buffer;
}

http_response_t* parse_http_response(char *response_buffer)
{
    http_response_t *response = malloc(sizeof(http_response_t));
    if (!response) {
        return NULL;
    }
    
    response->status_code = 0;
    response->headers = NULL;
    response->body = NULL;
    response->body_length = 0;
    
    if (!response_buffer || strlen(response_buffer) == 0) {
        return response;
    }
    
    char *body_start = strstr(response_buffer, "\r\n\r\n");
    if (body_start) {
        body_start += 4; // Hoppa över \r\n\r\n
        response->body = strdup(body_start);
        response->body_length = strlen(body_start);
        
        // Kopiera headers (allt före body)
        size_t headers_length = body_start - response_buffer - 4;
        response->headers = malloc(headers_length + 1);
        if (response->headers) {
            strncpy(response->headers, response_buffer, headers_length);
            response->headers[headers_length] = '\0';
        }
    } else {
        response->body = strdup("");
        response->body_length = 0;
        response->headers = strdup(response_buffer);
    }
    
    if (strncmp(response_buffer, "HTTP/", 5) == 0) {
        char *response_copy = strdup(response_buffer);
        char *status_line = strtok(response_copy, "\r\n");
        if (status_line) {
            sscanf(status_line, "HTTP/1.1 %d", &response->status_code);
        }
        free(response_copy);
    }
    
    return response;
}

http_response_t* send_http_post_with_response(const char *hostname, int port, const char *path, const char *headers, const char *body) {
    
    int sockfd = create_tcp_connection(hostname, port);
    if (sockfd < 0) {
        return NULL;
    }
    
    char *request = build_http_request(path, hostname, headers, body);
    if (!request) {
        close(sockfd);
        return NULL;
    }
    
    size_t total_sent = 0;
    size_t request_len = strlen(request);
    while (total_sent < request_len) {
        ssize_t sent = send(sockfd, request + total_sent, request_len - total_sent, 0);
        if (sent < 0) {
            perror("ERROR: Kunde inte skicka data");
            close(sockfd);
            return NULL;
        }
        total_sent += sent;
    }
    free(request);
    
    char *response_buffer = receive_http_response(sockfd);
    close(sockfd);
    
    if (!response_buffer) {
        return NULL;
    }
    
    http_response_t *response = parse_http_response(response_buffer);
    free(response_buffer);
    
    return response;
}

void free_http_response(http_response_t *response) {
    if (response) {
        free(response->headers);
        free(response->body);
        free(response);
    }
}