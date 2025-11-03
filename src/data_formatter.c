#include "../include/sensor_client.h"
#include <uuid/uuid.h>


// Formatera sensordata som JSON
char* format_json_sensor_data(const sensor_data_t *data) {
    char *json = malloc(MAX_JSON_SIZE);
    if (!json) {
        return NULL;
    }
    
    snprintf(json, MAX_JSON_SIZE,
        "{\n"
        "    \"device\": \"%s\",\n"
        "    \"time\": \"%s\",\n"
        "    \"temperature\": \"%.1f°C\"\n"
        "}",
        data->device_uuid,
        data->timestamp,
        data->temperature);
    
    return json;
}

char* get_current_timestamp(void) {
    time_t now = time(NULL);
    char *timestamp = malloc(24);
    if (!timestamp) {
        return NULL;
    }
    
    struct tm *utc_tm = gmtime(&now);
    strftime(timestamp, 24 - 1, "%Y-%m-%dT%H:%M:%SZ", utc_tm);
    
    return timestamp;
}


// Generera standard UUID (version 4)
char* generate_uuid(void) {
    char *uuid_str = malloc(48);
    if (!uuid_str) {
        return NULL;
    }
    
    // Initiera random seed om det inte gjorts
    static int seeded = 0;
    if (!seeded) {
        srand(time(NULL));
        seeded = 1;
    }
    
    snprintf(uuid_str, 50, "sensor-%08x-%04x-%04x-%04x-%08x%04x",
             rand(), rand() & 0xFFFF, rand() & 0xFFFF, 
             rand() & 0xFFFF, rand(), rand() & 0xFFFF);
    
    return uuid_str;
}