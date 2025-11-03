#include "../include/sensor_client.h"

void default_error_handler(const char *error_message) {
    fprintf(stderr, "ERROR: %s\n", error_message);
}

void send_sensor_data(const sensor_data_t *data, error_handler_t err_handler) {
    // Formatera data som JSON
    char *formatted_data = format_json_sensor_data(data);
    if (!formatted_data) {
        err_handler("Kunde inte formatera sensordata");
        return;
    }
    
    printf("Skickar JSON-data:\n%s\n", formatted_data);
    
    // Skicka HTTP POST och ta emot svar
    http_response_t *response = send_http_post_with_response(
        SERVER_HOST, SERVER_PORT, "/post", 
        "Content-Type: application/json",
        formatted_data
    );
    
    if (response) {
        printf("\n--- HTTP RESPONSE ---\n");
        print_http_response(response);
        free_http_response(response);
    } else {
        err_handler("Kunde inte skicka HTTP POST-förfrågan eller ta emot svar");
    }
    
    free(formatted_data);
}
