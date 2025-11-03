#define _GNU_SOURCE
#include "../include/http.h"
#include "../include/tcp_socket.h"
#include <sys/time.h>

// Hjälpfunktion för att bygga HTTP-förfrågan
char* build_http_request(const char *path, const char *hostname, 
                        const char *headers, const char *body) {
    char *request = malloc(BUFFER_SIZE);
    if (!request) {
        return NULL;
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

// Hjälpfunktion för att ta emot HTTP-svar
char* receive_http_response(int sockfd) {
    char *response_buffer = malloc(MAX_RESPONSE_SIZE);
    if (!response_buffer) {
        fprintf(stderr, "ERROR: Kunde inte allokera minne för svar\n");
        return NULL;
    }
    
    ssize_t total_received = 0;
    ssize_t bytes_received;
    
    // Sätt en timeout för recv
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

// Hjälpfunktion för att parsa HTTP-svar
http_response_t* parse_http_response(char *response_buffer) {
    http_response_t *response = malloc(sizeof(http_response_t));
    if (!response) {
        return NULL;
    }
    
    // Initiera värden
    response->status_code = 0;
    response->headers = NULL;
    response->body = NULL;
    response->body_length = 0;
    
    // Kontrollera att vi har ett svar
    if (!response_buffer || strlen(response_buffer) == 0) {
        return response;
    }
    
    // Hitta början av body (efter tomma raden) FÖRST
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
    
    // Extrahera statuskod från en kopia
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

// Skriv ut HTTP-svar
void print_http_response(const http_response_t *response) {
    if (!response) {
        printf("Inget svar mottaget\n");
        return;
    }
    
    printf("HTTP Status: %d\n", response->status_code);
    printf("Svarslängd: %zu bytes\n", response->body_length);
    printf("\nHeaders:\n%s\n", response->headers ? response->headers : "Inga headers");
    printf("\nBody:\n%s\n", response->body ? response->body : "Ingen body");
}

// Frigör minnet för HTTP-svar
void free_http_response(http_response_t *response) {
    if (response) {
        free(response->headers);
        free(response->body);
        free(response);
    }
}

// Skicka HTTP POST och ta emot svar
http_response_t* send_http_post_with_response(const char *hostname, int port, const char *path, 
                                            const char *headers, const char *body) {
    // Skapa TCP-anslutning
    int sockfd = create_tcp_connection(hostname, port);
    if (sockfd < 0) {
        return NULL;
    }
    
    // Bygg HTTP POST-förfrågan
    char *request = build_http_request(path, hostname, headers, body);
    if (!request) {
        close(sockfd);
        return NULL;
    }
    
    // Skicka förfrågan
    ssize_t sent = send(sockfd, request, strlen(request), 0);
    free(request);
    
    if (sent < 0) {
        perror("ERROR: Kunde inte skicka data");
        close(sockfd);
        return NULL;
    }
    
    // Ta emot svar
    char *response_buffer = receive_http_response(sockfd);
    close(sockfd);
    
    if (!response_buffer) {
        return NULL;
    }
    
    // Parsa HTTP-svar
    http_response_t *response = parse_http_response(response_buffer);
    free(response_buffer);
    
    return response;
}