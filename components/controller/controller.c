#include "controller.h"

#include "driver/gpio.h"
#include "driver/mcpwm.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/timers.h"

#include "pwm.h"
#include "adc.h"

#define MCPWM_FREQ_HZ 4095

static const char *TAG = "CONTROLLER";

static esp_timer_handle_t timer_handle;

controller_data_t controller_data_init();
void pre_heating_task(void *pvParameters);

controller_t controller_init(QueueHandle_t incoming_queue_commands, QueueHandle_t outgoing_queue_lcd) {
    controller_t controller = malloc(sizeof(s_controller_t));

    controller->potencia = pwm_init("POTENCIA", MCPWM_UNIT, MCPWM_TIMER_POTENCIA, MCPWM_IO_SIGNAL_POTENCIA, MCPWM_GPIO_NUM_POTENCIA);
    controller->cilindro = pwm_init("CILINDRO", MCPWM_UNIT, MCPWM_TIMER_CILINDRO, MCPWM_IO_SIGNAL_CILINDRO, MCPWM_GPIO_NUM_CILINDRO);
    controller->turbina = pwm_init("TURBINA", MCPWM_UNIT, MCPWM_TIMER_TURBINA, MCPWM_IO_SIGNAL_TURBINA, MCPWM_GPIO_NUM_TURBINA);
    controller->adc = adc_init("ADC TEMP", ADC_CHANNEL_6, ADC_WIDTH_BIT_12, ADC_ATTEN_DB_0);
    
    esp_timer_create_args_t timer_conf = {
        .callback = NULL,
        .arg = NULL,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "TIMER",
        .skip_unhandled_events = true};

    esp_timer_create(&timer_conf, &timer_handle);
    esp_timer_start_once(timer_handle, portMAX_DELAY);

    controller->controller_data = controller_data_init();
    controller->incoming_queue_commands = incoming_queue_commands;
    controller->outgoing_queue_lcd = outgoing_queue_lcd;
    controller->timer_handle = timer_handle;

    return controller;
}

void controller_task(void *pvParameters) {
    controller_t controller = (controller_t)pvParameters;
    // controller_data_t controller_data = controller->controller_data;
    QueueHandle_t incoming_queue_commands = controller->incoming_queue_commands;
    pre_heating_params_t pre_heating_params = malloc(sizeof(s_pre_heating_params_t));
    pre_heating_params->adc = &(controller->adc);
    pre_heating_params->controller_data = controller->controller_data;

    TaskHandle_t *pre_heating_task_handle;

    incoming_data_t incoming_data;  // Segura o evento atual
    BaseType_t xStatus;

    for (;;) {
        xStatus = xQueueReceive(incoming_queue_commands, &incoming_data, portMAX_DELAY);

        if (xStatus == pdPASS) {
            if (incoming_data->state != STATE_NONE) {
                switch (incoming_data->state) {
                    case ON:
                        ESP_LOGE(TAG, "ON");
                        controller->controller_data->read_state = ON;
                        break;

                    case OFF:
                        ESP_LOGE(TAG, "OFF");
                        controller->controller_data->read_state = OFF;
                        break;

                    default:;
                }
            }

            if (incoming_data->mode != MODE_NONE) {
                switch (incoming_data->mode) {
                    case MANUAL:
                        ESP_LOGE(TAG, "MANUAL");
                        controller->controller_data->read_mode = MANUAL;
                        break;

                    case AUTO:
                        ESP_LOGE(TAG, "AUTO");
                        controller->controller_data->read_mode = AUTO;
                        break;

                    default:;
                }
            }

            if (incoming_data->stage != STAGE_NONE) {
                switch (incoming_data->stage) {
                    case STAGE_OFF:
                        ESP_LOGE(TAG, "STAGE_OFF");
                        controller->controller_data->read_state = OFF;
                        controller->controller_data->read_mode = MODE_NONE;
                        controller->controller_data->read_stage = STAGE_OFF;
                        break;

                    case PRE_HEATING:
                        ESP_LOGE(TAG, "PRE_HEATING");
                        controller->controller_data->read_stage = PRE_HEATING;
                        xTaskCreate(pre_heating_task, "PRE_HEATING", 2048, pre_heating_params, 5, &pre_heating_task_handle);
                        break;

                    case START:
                        ESP_LOGE(TAG, "START");
                        controller->controller_data->read_stage = START;
                        break;

                    case END:
                        ESP_LOGE(TAG, "END");
                        controller->controller_data->read_stage = END;
                        break;

                    case COOLER:
                        ESP_LOGE(TAG, "COOLER");
                        controller->controller_data->read_stage = COOLER;
                        break;

                    default:;
                }
            }

            if (controller->controller_data->read_state == ON) {
                if (controller->controller_data->read_mode == MANUAL) {
                    if (controller->controller_data->read_stage == PRE_HEATING || controller->controller_data->read_stage == START) {
                        if (incoming_data->write_potencia != -1) {
                            ESP_LOGE("POTENCIA", "%d", incoming_data->write_potencia);
                            controller->controller_data->read_potencia = incoming_data->write_potencia;
                            pwm_set_duty(controller->potencia, controller->controller_data->read_potencia);
                        }

                        if (incoming_data->write_cilindro != -1) {
                            ESP_LOGE("CILINDRO", "%d", incoming_data->write_cilindro);
                            controller->controller_data->read_cilindro = incoming_data->write_cilindro;
                            pwm_set_duty(controller->cilindro, controller->controller_data->read_cilindro);
                        }

                        if (incoming_data->write_turbina != -1) {
                            ESP_LOGE("TURBINA", "%d", incoming_data->write_turbina);
                            controller->controller_data->read_turbina = incoming_data->write_turbina;
                            pwm_set_duty(controller->turbina, controller->controller_data->read_turbina);
                        }
                    }
                }
            }
        }
    }
}

void pre_heating_task(void *pvParameters) {
    pre_heating_params_t pre_heating_params = (pre_heating_params_t)pvParameters;
    controller_data_t controller_data = pre_heating_params->controller_data;
    adc_t *adc = pre_heating_params->adc;

    for(;;) {
        adc_sample(*adc);

        vTaskDelay(1000);
    }
}

controller_data_t controller_data_init() {
    controller_data_t controller_data = malloc(sizeof(s_controller_data_t));

    controller_data->read_state = OFF;
    controller_data->read_mode = MODE_NONE;
    controller_data->read_stage = STAGE_OFF;

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