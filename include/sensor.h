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

#include "tcp.h"
#include "http.h"


#define MAX_JSON_SIZE 512

typedef struct {
    char *device_id;
    char *timestamp;
    double temperature;
} sensor_data_t;


int load_measurement_interval(void);

// Function pointer type for sensor data generation
typedef sensor_data_t (*sensor_generator_t)(void);

sensor_data_t generate_auto_sensor_data(void);
sensor_data_t generate_manual_sensor_data(void);

char* Generate_Device_Id(void);
double Random_temperature_sensor(void);
char* get_current_timestamp(void);


char* format_json_sensor_data(const sensor_data_t *data);

int send_sensor_data(const sensor_data_t *data);
int save_sensor_data_to_file(const char* device_id, double temperature, const char* timestamp);

//int clear_sent_data();


// Utility functions (main.c)
void print_usage(const char *program_name);

#endif // SENSOR_CLIENT_H