#ifndef SENSOR_CLIENT_H
#define SENSOR_CLIENT_H

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <time.h>
#include <netinet/in.h>

// Include modular headers
#include "tcp_socket.h"
#include "http.h"

#define MAX_JSON_SIZE 512

typedef struct {
    char *device_uuid;
    char *timestamp;
    double temperature;
} sensor_data_t;


// Function pointer type for error handling
typedef void (*error_handler_t)(const char *error_message);

// Data handling functions (handlers.c)
void default_error_handler(const char *error_message);
void send_sensor_data(const sensor_data_t *data, error_handler_t err_handler);

// Data formatting functions (data_formatter.c)
char* format_json_sensor_data(const sensor_data_t *data);
char* get_current_timestamp(void);
char* generate_uuid(void);

// Utility functions (main.c)
void print_usage(const char *program_name);

#endif // SENSOR_CLIENT_H