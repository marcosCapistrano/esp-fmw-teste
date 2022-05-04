#ifndef COMMON_TORRADOR_CONTROLLER_H
#define COMMON_TORRADOR_CONTROLLER_H

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

typedef struct {
    _Bool is_on;
    int64_t timer_value;
} torrador_state_t;

typedef enum {
    POTENCIA,
    CILINDRO,
    TURBINA,
    INICIAR,
    PARAR,
    PONTO_DE_TORRA
} control_event_type_t;

typedef struct {
    control_event_type_t type;
    int value;
} control_event_t;

typedef struct {
    QueueHandle_t control_event_queue;
    QueueHandle_t state_event_queue;
} torrador_controller_params_t;

torrador_controller_params_t *torrador_controller_params_create(QueueHandle_t control_event_queue, QueueHandle_t state_event_queue);

#endif