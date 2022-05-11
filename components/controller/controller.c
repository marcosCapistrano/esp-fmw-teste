#include "controller.h"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/timers.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "driver/gpio.h"
#include "driver/mcpwm.h"

#define MCPWM_UNIT MCPWM_UNIT_0
#define MCPWM_TIMER_POTENCIA MCPWM_TIMER_0
#define MCPWM_TIMER_CILINDRO MCPWM_TIMER_1
#define MCPWM_TIMER_TURBINA MCPWM_TIMER_2

#define MCPWM_POTENCIA 2
#define MCPWM_CILINDRO 12
#define MCPWM_TURBINA 5

#define MCPWM_FREQ_HZ 4095

static const char *TAG = "CONTROLLER";

controller_data_t controller_data_init();

controller_t controller_init(QueueHandle_t incoming_queue_commands, QueueHandle_t outgoing_queue_lcd) {
    controller_t controller = malloc(sizeof(s_controller_t));

    controller->potencia = pwm_init("POTENCIA", MCPWM_UNIT, MCPWM_TIMER_POTENCIA, MCPWM_IO_SIGNAL_POTENCIA, MCPWM_GPIO_NUM_POTENCIA);
    controller->cilindro = pwm_init("CILINDRO", MCPWM_UNIT, MCPWM_TIMER_CILINDRO, MCPWM_IO_SIGNAL_CILINDRO, MCPWM_GPIO_NUM_CILINDRO);
    controller->turbina = pwm_init("TURBINA", MCPWM_UNIT, MCPWM_TIMER_TURBINA, MCPWM_IO_SIGNAL_TURBINA, MCPWM_GPIO_NUM_TURBINA);

    esp_timer_create_args_t timer_conf = {
        .callback = NULL,
        .arg = NULL,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "TIMER",
        .skip_unhandled_events = true};

    esp_timer_create(&timer_conf, controller->timer_handle);
    esp_timer_start_once(*(controller->timer_handle), portMAX_DELAY);

    controller->controller_data = controller_data_init();
    controller->incoming_queue_commands = incoming_queue_commands;
    controller->outgoing_queue_lcd = outgoing_queue_lcd;

    return controller;
}

void controller_task(void *pvParameters) {
    controller_t controller = (controller_t) pvParameters;
    QueueHandle_t incoming_queue_commands = controller->incoming_queue_commands;

    incoming_data_t incoming_data;  // Segura o evento atual
    BaseType_t xStatus;

    for (;;) {
        xStatus = xQueueReceive(incoming_queue_commands, &incoming_data, portMAX_DELAY);
    }
}

controller_data_t controller_data_init() {
    controller_data_t controller_data = malloc(sizeof(s_controller_data_t));

    controller_data->state = OFF;
    controller_data->mode = MODE_NONE;
    controller_data->stage = STAGE_NONE;


    controller_data->write_potencia = 0;
    controller_data->write_cilindro = 0;
    controller_data->write_turbina = 0;

    controller_data->read_potencia = 0;
    controller_data->read_cilindro = 0;
    controller_data->read_turbina = 0;
    controller_data->read_temp_ar = 0;
    controller_data->read_temp_grao = 0;
    controller_data->read_grad = 0;

    /* Dados que os readers mandaram para o controller, se enviaram alguma receita para fazer */
    controller_data->write_recipe_data = NULL;
    controller_data->write_sensor_data = NULL;

    /* Dados que o controller está salvando da receita atual */
    controller_data->read_recipe_data = NULL;
    controller_data->read_sensor_data = NULL;

    return controller_data;
}