#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include "common_torrador_controller.h"

torrador_controller_params_t *torrador_controller_params_create(QueueHandle_t state_queue, QueueHandle_t control_queue) {
		torrador_controller_params_t *params = malloc(sizeof(torrador_controller_params_t));
		params->state_queue = state_queue;
		params->control_queue = control_queue;
		return params;
}