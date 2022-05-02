#ifndef COMMON_TORRADOR_CONTROLLER_H
#define COMMON_TORRADOR_CONTROLLER_H

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

typedef enum {
	ON,
	PAUSED,
	COOLER,
	OFF
} torrador_state_t;

typedef struct {
	uint8_t potencia;
	uint8_t cilindro;
	uint8_t turbina;
	torrador_state_t state;
} torrador_control_t;

typedef struct {
	QueueHandle_t state_queue;
	QueueHandle_t control_queue;
} torrador_controller_params_t;

torrador_controller_params_t *torrador_controller_params_create(QueueHandle_t state_queue, QueueHandle_t control_queue);

#endif