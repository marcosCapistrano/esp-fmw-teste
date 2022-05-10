#ifndef CONTROLLER_H
#define CONTROLLER_H

#define MCPWM_UNIT MCPWM_UNIT_0
#define MCPWM_TIMER_POTENCIA MCPWM_TIMER_0
#define MCPWM_TIMER_CILINDRO MCPWM_TIMER_1
#define MCPWM_TIMER_TURBINA MCPWM_TIMER_2

#define MCPWM_GPIO_NUM_POTENCIA 2
#define MCPWM_GPIO_NUM_CILINDRO 12
#define MCPWM_GPIO_NUM_TURBINA 5

#define MCPWM_IO_SIGNAL_POTENCIA MCPWM0A
#define MCPWM_IO_SIGNAL_CILINDRO MCPWM1A
#define MCPWM_IO_SIGNAL_TURBINA MCPWM2A

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_timer.h"

#include "common_params.h"
#include "common.h"
#include "pwm.h"


struct controller {
    esp_timer_handle_t* timer_handle;

    pwm_t potencia;
    pwm_t cilindro;
    pwm_t turbina;

    controller_mode_t mode;
    stage_t stage;
    int64_t start_time;
    int64_t current_time;
};
typedef struct controller* controller_t;

controller_t controller_init();
void controller_task(void *pvParameters); 

#endif
