#include "../include/sensor.h"
#include <stdio.h>
#include <stdlib.h>
//#include <uuid/uuid.h>

void console_log(const char *message)
{
    printf("%s\n", message);
}
void debug_log(const char *message)
{
    printf("DEBUG: %s\n", message);
}

sensor_data_t generate_auto_sensor_data(void)
{
    sensor_data_t data;
    data.device_id = Generate_Device_Id();
    data.timestamp = get_current_timestamp(); 
    data.temperature = Random_temperature_sensor();
    return data;
}

sensor_data_t generate_manual_sensor_data(void)
{
    sensor_data_t data;
    data.device_id = Generate_Device_Id();
    data.timestamp = get_current_timestamp();
    data.temperature = 25.0;
    return data;
}

// Generera standard UUID (version 4) set to node001 for demo
char* Generate_Device_Id(void)
{
    char *device_id = malloc(16);
    if (!device_id) {
        return NULL;
    }
    
    snprintf(device_id, 16, "node001");

    return device_id;
}

double Random_temperature_sensor(void)
{
    static double base_temperature = 23.0;
    static int initialized = 0;

    if(!initialized) {
        srand(time(NULL));
        initialized = 1;
    }

    double noise = ((double)rand() / RAND_MAX - 0.5) * 1.0;
    
    double drift = ((double)rand() / RAND_MAX - 0.5) * 0.1;
    base_temperature += drift;

    if (base_temperature < 15.0 || base_temperature > 35.0)
        base_temperature = 20.0;

    return base_temperature + noise;
}

char* get_current_timestamp(void)
{
time_t now = time(NULL);
    char *timestamp = malloc(24);
    if (!timestamp) {
        return NULL;
    }
    
    struct tm *utc_tm = gmtime(&now);
    strftime(timestamp, 24 - 1, "%Y-%m-%dT%H:%M:%SZ", utc_tm);
    
    return timestamp;
}

char* format_json_sensor_data(const sensor_data_t *data)
{
    char *json = malloc(MAX_JSON_SIZE);
    if (!json) {
        return NULL;
    }
    
    snprintf(json, MAX_JSON_SIZE,
        "{\n"
        "    \"device_id\": \"%s\",\n"
        "    \"timestamp\": \"%s\",\n"
        "    \"temperature\": %.1f\n"
        "}",
        data->device_id,
        data->timestamp,
        data->temperature);
    
    return json;
}

int send_sensor_data(const sensor_data_t *data, logger_t log_func)
{

    char *formatted_data = format_json_sensor_data(data);
    if (!formatted_data) {
        log_func("Failed to format sensor data");
        return -1;
    }
    printf("Skickar JSON-data:\n%s\n", formatted_data);
    log_func("Sending data to httpbin.org server");
    
    http_response_t *response = send_http_post_with_response(
        SERVER_HOST, SERVER_PORT, "/post", 
        "Content-Type: application/json",
        formatted_data
    );
    
    if (response)
    { 
        //printf("\n=== SERVER RESPONSE ===\n");
        //printf("HTTP Status: %d\n", response->status_code);
        //if (response->body && strlen(response->body) > 0) {
        //    printf("Response Body:\n%s\n", response->body);
        //}
        //printf("=======================\n\n");
        //
        if (response->status_code >= 200 && response->status_code < 300)
        {
        printf("Data sent (HTTP %d)\n", response->status_code);
        log_func("SUCCESS: Data successfully sent to server");
        free_http_response(response);
        return 0;
        } else
        {
            log_func("ERROR: Server returned error status: ");
            printf("Server error (HTTP %d)\n", response->status_code);
        free_http_response(response);
        return -1;
        }
    } else {
        log_func("ERROR: No response from server");
        return -1;
    }
}

//int retry_unsent_data()

int save_sensor_data_to_file(const char* device_id, double temperature, const char* timestamp)
{    
    FILE *file = fopen("bin/saved_temp.txt", "a");
    
    if (file == NULL) {
        printf("DEBUG: Failed to open file!\n");
        return -1;
    }
        
    fprintf(file, "id:%stemp:%.1ftime:%s\n", device_id, temperature, timestamp);    
    fclose(file);
    
    return 0;
}


