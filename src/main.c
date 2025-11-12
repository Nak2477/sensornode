#include "../include/sensor.h"
#include <time.h>


void print_usage(const char *program_name)
{
    printf("Användning: %s [options]\n", program_name);
    printf("  --continuous, -c - Kontinuerliga mätningar\n");
    printf("  --single [temp]  - En enskild mätning (standard: sensor)\n");
    printf("  --help, -h       - Visa denna hjälp\n");
    printf("\nExempel:\n");
    printf("  %s --continuous     # Kör kontinuerligt\n", program_name);
    printf("  %s --single 25.3    # En mätning med 25.3°C\n", program_name);
    printf("  %s --single         # En mätning med sensorsimulering\n", program_name);
}

int main(int argc, char **argv)
{
    srand(time(NULL));
    
    int measurement_interval = load_measurement_interval();

    int continuous_mode = 0;  // Default: single measurement
    double manual_temperature = 0.0;
    int use_manual_temp = 0;
    
    if (argc > 1) {
        if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
            print_usage(argv[0]);
            return 0;
        } else if (strcmp(argv[1], "--continuous") == 0 || strcmp(argv[1], "-c") == 0) {
            continuous_mode = 1;
        } else if (strcmp(argv[1], "--single") == 0) {
            if (argc > 2) {
                manual_temperature = atof(argv[2]);
                use_manual_temp = 1;
            }
        } else {
            manual_temperature = atof(argv[1]);
            use_manual_temp = 1;
        }
    }
    
    printf("=== SensorNode Starting ===\n");
    if (continuous_mode) {
        printf("Mode: Continuous measurements every %d seconds\n", measurement_interval);
        printf("Press Ctrl+C to stop\n\n");
    } else {
        printf("Mode: Single measurement\n\n");
    }

    do {
        // Choose sensor data generator using function pointer
        sensor_generator_t data_generator;
        sensor_data_t sensor_data;
        
        if (use_manual_temp)
        {
            sensor_data = generate_auto_sensor_data();
            sensor_data.temperature = manual_temperature;
        } else
        {
            data_generator = generate_auto_sensor_data;
            sensor_data = data_generator();
        }
           
        if (!sensor_data.device_id || !sensor_data.timestamp)
        {
            fprintf(stderr, "ERROR: Kunde inte skapa sensordata\n");
            return 1;
        }
        
        printf("Measurement: %.1f°C at %s\n", sensor_data.temperature, sensor_data.timestamp);
        
        
        if (send_sensor_data(&sensor_data, console_log) == 0)
        {
            printf("Data sent to server\n");
        } else 
        {
            printf("Network error - saving data locally\n");
            save_sensor_data_to_file(sensor_data.device_id, sensor_data.temperature, sensor_data.timestamp);
        }

        free(sensor_data.device_id);
        if (sensor_data.timestamp) {
            free(sensor_data.timestamp);
        }
        
        if (continuous_mode) {
            printf("Waiting %d seconds for next measurement...\n", measurement_interval);
            sleep(measurement_interval);
        }
        
    } while (continuous_mode);
    
    return 0;
}