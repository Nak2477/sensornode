#include "../include/sensor_client.h"
#include <time.h>

// Generera slumpmässig temperatur (-20 till +40°C)
double generate_random_temperature() {
    return -20.0 + (rand() % 6000) / 100.0;  // -20.00 till +39.99°C
}

void print_usage(const char *program_name) {
    printf("Användning: %s [temperatur|--random]\n", program_name);
    printf("  temperatur  - Temperaturen att skicka (standard: 23.5)\n");
    printf("  --random, -r - Generera slumpmässig temperatur (-20 till +40°C)\n");
    printf("  --help, -h  - Visa denna hjälp\n");
    printf("\nExempel:\n");
    printf("  %s 25.3\n", program_name);
    printf("  %s --random\n", program_name);
    printf("  %s --help\n", program_name);
}

int main(int argc, char **argv) {

    srand(time(NULL));
    
    // Parse kommandoradsargument
    double temperature = 23.5;  // Standard värde
    
    if (argc > 1) {
        if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
            print_usage(argv[0]);
            return 0;
        } else if (strcmp(argv[1], "--random") == 0 || strcmp(argv[1], "-r") == 0) {
            temperature = generate_random_temperature();
            printf("Genererad slumpmässig temperatur: %.2f°C\n", temperature);
        } else {
            temperature = atof(argv[1]);
        }
    }
    
    // Skapa sensordata
    sensor_data_t sensor_data;
    sensor_data.device_uuid = generate_uuid();
    sensor_data.timestamp = get_current_timestamp();
    sensor_data.temperature = temperature;
    
    if (!sensor_data.device_uuid || !sensor_data.timestamp) {
        fprintf(stderr, "ERROR: Kunde inte skapa sensordata\n");
        return 1;
    }
    
    // Skicka data
    send_sensor_data(&sensor_data, default_error_handler);
    
    // Rensa upp minne
    free(sensor_data.device_uuid);
    if (sensor_data.timestamp) {
        free(sensor_data.timestamp);
    }
    
    return 0;
}