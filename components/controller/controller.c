#include "controller.h"

#include "adc.h"
#include "driver/gpio.h"
#include "driver/mcpwm.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/timers.h"
#include "pwm.h"

#define MCPWM_FREQ_HZ 4095

static const char *TAG = "CONTROLLER";

static esp_timer_handle_t timer_handle;

controller_data_t controller_data_init();
void pre_heating_task(void *pvParameters);
void torra_task(void *pvParameters);
void cooler_task(void *pvParameters);

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
    controller_data_t controller_data = controller->controller_data;
    QueueHandle_t incoming_queue_commands = controller->incoming_queue_commands;

    TaskHandle_t pre_heating_task_handle;
    TaskHandle_t torra_task_handle;
    TaskHandle_t cooler_task_handle;

    incoming_data_t incoming_data;  // Segura o evento atual
    BaseType_t xStatus;

    for (;;) {
        xStatus = xQueueReceive(incoming_queue_commands, &incoming_data, portMAX_DELAY);

        if (xStatus == pdPASS) {
            if (incoming_data->state != STATE_NONE) {
                switch (incoming_data->state) {
                    case ON:
                        ESP_LOGE(TAG, "ON");
                        controller_data->read_state = ON;
                        break;

                    case OFF:
                        ESP_LOGE(TAG, "OFF");
                        controller_data->read_state = OFF;
                        break;

                    default:;
                }
            }

            if (incoming_data->mode != MODE_NONE) {
                switch (incoming_data->mode) {
                    case MANUAL:
                        ESP_LOGE(TAG, "MANUAL");
                        controller_data->read_mode = MANUAL;
                        break;

                    case AUTO:
                        ESP_LOGE(TAG, "AUTO");
                        controller_data->read_mode = AUTO;
                        break;

                    default:;
                }
            }

            if (incoming_data->stage != STAGE_NONE) {
                switch (incoming_data->stage) {
                    case STAGE_OFF:
                        ESP_LOGE(TAG, "STAGE_OFF");
                        controller_data->read_state = OFF;
                        controller_data->read_mode = MODE_NONE;
                        controller_data->read_stage = STAGE_OFF;
                        break;

                    case PRE_HEATING:
                        controller_data->read_stage = PRE_HEATING;
                        controller_data->read_recipe_data = recipe_data_init();
                        controller_data->read_sensor_data = sensor_data_init();

                        pre_heating_params_t pre_heating_params = malloc(sizeof(s_pre_heating_params_t));
                        pre_heating_params->adc = controller->adc;
                        pre_heating_params->controller_data = controller_data;

                        ESP_LOGE(TAG, "PRE_HEATING");
                        xTaskCreatePinnedToCore(pre_heating_task, "PRE_HEATING", 2048, pre_heating_params, 5, &pre_heating_task_handle, 1);
                        break;

                    case START:
                        ESP_LOGE(TAG, "START");
                        ESP_LOGE(TAG, "Last TEMP: %d", controller_data->read_recipe_data->pre_heating_temp);

                        vTaskDelete(pre_heating_task_handle);
                        controller_data->read_stage = START;

                        torra_params_t torra_params = malloc(sizeof(s_torra_params_t));
                        torra_params->adc = controller->adc;
                        torra_params->controller_data = controller_data;

                        xTaskCreatePinnedToCore(torra_task, "TORRA", 2048, torra_params, 5, &torra_task_handle, 1);
                        break;

                    case COOLER:
                        ESP_LOGE(TAG, "COOLER");

                        vTaskDelete(torra_task_handle);
                        controller_data->read_stage = COOLER;

                        controller_data->start_time = 0;
                        controller_data->elapsed_time = 0;

                        cooler_params_t cooler_params = malloc(sizeof(s_cooler_params_t));
                        cooler_params->controller_data = controller_data;
                        xTaskCreatePinnedToCore(cooler_task, "COOLER", 2048, cooler_params, 5, &cooler_task_handle, 1);
                        break;

                    case END:
                        ESP_LOGE(TAG, "END");

                        vTaskDelete(cooler_task_handle);
                        controller_data->read_stage = END;
                        controller_data->elapsed_time = 0;

                        //FREE MEMORY
                        break;

                    default:;
                }
            }

            if (controller_data->read_state == ON) {
                if (controller_data->read_mode == MANUAL) {
                    if (controller_data->read_stage == PRE_HEATING || controller_data->read_stage == START) {
                        if (incoming_data->write_potencia != -1) {
                            ESP_LOGE("4", "write_value %d", incoming_data->write_potencia);
                            controller_data->read_potencia = incoming_data->write_potencia;
                            ESP_LOGE("5", "read_potencia %d", controller_data->read_potencia);
                            pwm_set_duty(controller->potencia, controller_data->read_potencia);
                        }

                        if (incoming_data->write_cilindro != -1) {
                            ESP_LOGE("CILINDRO", "%d", incoming_data->write_cilindro);
                            controller_data->read_cilindro = incoming_data->write_cilindro;
                            pwm_set_duty(controller->cilindro, controller_data->read_cilindro);
                        }

                        if (incoming_data->write_turbina != -1) {
                            ESP_LOGE("TURBINA", "%d", incoming_data->write_turbina);
                            controller_data->read_turbina = incoming_data->write_turbina;
                            pwm_set_duty(controller->turbina, controller_data->read_turbina);
                        }
                    }
                }
            }

            controller_data->read_temp_ar = adc_sample(controller->adc);
        }
    }
}

void pre_heating_task(void *pvParameters) {
    pre_heating_params_t pre_heating_params = (pre_heating_params_t)pvParameters;
    controller_data_t controller_data = pre_heating_params->controller_data;
    int *pre_heating_temp = &controller_data->read_recipe_data->pre_heating_temp;

    adc_t adc = pre_heating_params->adc;

    for (;;) {
        *pre_heating_temp = adc_sample(adc);
        *controller_data->read_temp_ar = *pre_heating_temp; // Para que o display consiga ver
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void torra_task(void *pvParameters) {
    torra_params_t torra_params = (torra_params_t)pvParameters;
    controller_data_t controller_data = torra_params->controller_data;

    recipe_data_t read_recipe_data = controller_data->read_recipe_data;
    sensor_data_t read_sensor_data = controller_data->read_sensor_data;

    controller_data->start_time = esp_timer_get_time();
    adc_t adc = torra_params->adc;

    int counter = 0;
    int last_timer_period = controller_data->start_time;

    for (;;) {
        int64_t elapsed_time = esp_timer_get_time() - controller_data->start_time;
        controller_data->elapsed_time = elapsed_time;
        controller_data->read_torra_time = elapsed_time;

        if ((esp_timer_get_time() - last_timer_period) / 10E5 > 30) {
            last_timer_period = esp_timer_get_time();

            ESP_LOGI(TAG, "SALVANDO %d", controller_data->read_potencia);

            read_sensor_data->array_temp_ar[counter] = adc_sample(adc);
            read_sensor_data->array_temp_grao[counter] = adc_sample(adc);
            read_sensor_data->array_grad[counter] = 0;
            read_sensor_data->array_delta_grao[counter] = 0;

            read_recipe_data->array_potencia[counter] = controller_data->read_potencia;
            read_recipe_data->array_cilindro[counter] = controller_data->read_cilindro;
            read_recipe_data->array_turbina[counter] = controller_data->read_turbina;

            ESP_LOGI(TAG, "SALVANDO2 %d", controller_data->read_potencia);
            
            counter++;
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void cooler_task(void *pvParameters) {
    cooler_params_t cooler_params = (cooler_params_t)pvParameters;
    controller_data_t controller_data = cooler_params->controller_data;

    controller_data->start_time = esp_timer_get_time();

    for (;;) {
        int64_t elapsed_time = esp_timer_get_time() - controller_data->start_time;
        controller_data->elapsed_time = elapsed_time;

        controller_data->read_resf_time = elapsed_time;
        vTaskDelay(pdMS_TO_TICKS(100));
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

    controller_data->elapsed_time = 0;

    controller_data->read_torra_time = 0;
    controller_data->read_resf_time = 0;

    /* Dados que os readers mandaram para o controller, se enviaram alguma receita para fazer */
    controller_data->write_recipe_data = 0;
    controller_data->write_sensor_data = 0;

    /* Dados que o controller está salvando da receita atual */
    controller_data->read_recipe_data = 0;
    controller_data->read_sensor_data = 0;

    return controller_data;
}

recipe_data_t recipe_data_init() {
    recipe_data_t recipe_data = malloc(sizeof(s_recipe_data_t));

    recipe_data->pre_heating_temp = 0;

    return recipe_data;
}

sensor_data_t sensor_data_init() {
    sensor_data_t sensor_data = malloc(sizeof(s_sensor_data_t));

    return sensor_data;
}