#include "common_params.h"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

controller_params_t controller_params_init(QueueHandle_t input_control_queue, QueueHandle_t output_notify_queue, controller_t controller) {
    controller_params_t params = malloc(sizeof(controller_params_t));

    params->input_control_queue = input_control_queue;
    params->output_notify_queue = output_notify_queue;
    params->controller = controller;
    return params;
}