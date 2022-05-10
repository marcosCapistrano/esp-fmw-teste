#ifndef COMMON_PARAMS_H
#define COMMON_PARAMS_H

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "controller.h"
#include "common.h"

typedef enum {
    STAGE,
    INPUT
} event_t;

// --------- INPUT
typedef enum {
    INPUT_NONE,
    POTENCIA_INPUT,
    CILINDRO_INPUT,
    TURBINA_INPUT
} input_t;

typedef struct s_input_event_t *input_event_t;

typedef struct s_input_event_t {
    event_t event;
    stage_t stage;
    input_t input;
    int value;
} s_input_event_t;

 // --------- OUTPUT
typedef enum {
    POTENCIA_OUTPUT,
    CILINDRO_OUTPUT,
    TURBINA_OUTPUT,
    TIMER,
    TEMP_GRAO,
    TEMP_AR,
} output_t;

typedef struct output_event *output_event_t;

struct output_event {
    event_t event;
    stage_t stage;
    output_t output;
    int value;

    /*
    De 30 em 30 segundos (programável), será disparado um evento "definitivo",
    chamado assim pois serão esses que serão salvos no banco de dados.
    */
    _Bool definitive;
};

typedef struct controller* controller_t;

typedef struct controller_params_t {
    QueueHandle_t input_control_queue;
    QueueHandle_t output_notify_queue;
    controller_t controller;
} s_controller_params_t;

typedef s_controller_params_t *controller_params_t;


controller_params_t controller_params_init(QueueHandle_t input_control_queue, QueueHandle_t output_notify_queue, controller_t controller);

#endif