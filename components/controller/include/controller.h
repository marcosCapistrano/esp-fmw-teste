#ifndef CONTROLLER_H
#define CONTROLLER_H

#define MCPWM_UNIT MCPWM_UNIT_0
#define MCPWM_TIMER_POTENCIA MCPWM_TIMER_0
#define MCPWM_TIMER_CILINDRO MCPWM_TIMER_1
#define MCPWM_TIMER_TURBINA MCPWM_TIMER_2

#define MCPWM_GPIO_NUM_POTENCIA 2
#define MCPWM_GPIO_NUM_CILINDRO 12
#define MCPWM_GPIO_NUM_TURBINA 4

#define MCPWM_IO_SIGNAL_POTENCIA MCPWM0A
#define MCPWM_IO_SIGNAL_CILINDRO MCPWM1A
#define MCPWM_IO_SIGNAL_TURBINA MCPWM2A

#define CONTROLLER_MAX_TIME_MINS 25

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_timer.h"

#include "common_controller.h"
#include "pwm.h"
#include "adc.h"

typedef struct s_controller_t {
    QueueHandle_t outgoing_queue_lcd;
    QueueHandle_t outgoing_queue_server;
    QueueHandle_t incoming_queue_commands;
    
    esp_timer_handle_t timer_handle;

    pwm_t potencia;
    pwm_t cilindro;
    pwm_t turbina;
    adc_t adc;

    controller_data_t controller_data;
} s_controller_t;

typedef struct s_controller_t *controller_t;

typedef struct s_pre_heating_params_t {
    controller_data_t controller_data;
    adc_t adc;
} s_pre_heating_params_t;

typedef struct s_pre_heating_params_t *pre_heating_params_t;

typedef struct s_torra_params_t {
    controller_data_t controller_data;

    adc_t adc;
} s_torra_params_t;

typedef struct s_torra_params_t *torra_params_t;

typedef struct s_cooler_params_t {
    controller_data_t controller_data;
} s_cooler_params_t;

typedef struct s_cooler_params_t *cooler_params_t;

controller_t controller_init(QueueHandle_t incoming_queue_commands, QueueHandle_t outgoing_queue_lcd);
void controller_task(void *pvParameters); 

static recipe_data_t recipe_data_init();
static sensor_data_t sensor_data_init();

static recipe_data_node_t recipe_data_node_init(int potencia, int cilindro, int turbina, uint64_t time);
static void push_recipe_data(recipe_data_node_t *data_node, int potencia, int cilindro, int turbina, uint64_t time);


static sensor_data_node_t sensor_data_node_init(int temp_ar, int temp_grao, int delta_ar, int delta_grao, uint64_t time);
static void push_sensor_data(sensor_data_node_t *data_node, int temp_ar, int temp_grao, int delta_ar, int delta_grao, uint64_t time);

#endif
