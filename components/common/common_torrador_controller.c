#include "common_torrador_controller.h"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

torrador_controller_params_t *torrador_controller_params_create(QueueHandle_t control_event_queue, QueueHandle_t state_event_queue) {
    torrador_controller_params_t *params = malloc(sizeof(torrador_controller_params_t));
    params->control_event_queue = control_event_queue;
    params->state_event_queue = state_event_queue;
    return params;
}