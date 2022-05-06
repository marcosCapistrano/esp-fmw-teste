#include "controller.h"

#include <stdio.h>
#include <stdlib.h>

#include "common_params.h"
#include "driver/adc.h"
#include "driver/gpio.h"
#include "driver/mcpwm.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/timers.h"

#define MCPWM_UNIT MCPWM_UNIT_0
#define MCPWM_TIMER_POTENCIA MCPWM_TIMER_0
#define MCPWM_TIMER_CILINDRO MCPWM_TIMER_1
#define MCPWM_TIMER_TURBINA MCPWM_TIMER_2

#define MCPWM_POTENCIA 2
#define MCPWM_CILINDRO 12
#define MCPWM_TURBINA 5

#define MCPWM_FREQ_HZ 4095

static const char *TAG = "CONTROLLER";

controller_t controller_init() {
    controller_t controller = malloc(sizeof(controller_t));

    controller->potencia = pwm_init("POTENCIA", MCPWM_UNIT, MCPWM_TIMER_POTENCIA, MCPWM_IO_SIGNAL_POTENCIA, MCPWM_GPIO_NUM_POTENCIA);
    controller->cilindro = pwm_init("CILINDRO", MCPWM_UNIT, MCPWM_TIMER_CILINDRO, MCPWM_IO_SIGNAL_CILINDRO, MCPWM_GPIO_NUM_CILINDRO);
    controller->turbina = pwm_init("TURBINA", MCPWM_UNIT, MCPWM_TIMER_TURBINA, MCPWM_IO_SIGNAL_TURBINA, MCPWM_GPIO_NUM_TURBINA);

    controller->stage = OFF;
    controller->start_time = 0;
    esp_timer_create_args_t timer_conf = {
        .callback = NULL,
        .arg = NULL,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "TIMER",
        .skip_unhandled_events = true};

    esp_timer_create(&timer_conf, controller->timer_handle);
    esp_timer_start_once(&controller->timer_handle, portMAX_DELAY);
    return controller;
}

void controller_task(void *pvParameters) {
    controller_params_t params = (controller_params_t)pvParameters;
    QueueHandle_t input_control_queue = params->input_control_queue;  // Queue com os eventos que chegaram da IHM/WEB
    QueueHandle_t output_notify_queue = params->output_notify_queue;  // Queue para avisar IHM/WEB de mudanças no estado

    input_event_t input_control_event;  // Segura o evento atual
    BaseType_t xStatus;

    controller_t controller = controller_init();

    for (;;) {
        xStatus = xQueueReceive(input_control_queue, &input_control_event, pdMS_TO_TICKS(250));

        if (xStatus == pdPASS) {
            if (input_control_event->event == STAGE) {
            ESP_LOGI(TAG, "Recebido evento de estágio!");
                switch (input_control_event->stage) {
                    case OFF:

                        break;

                    case PRE_HEATING:

                        break;

                    case START:

                        break;

                    case END:

                        break;

                    case COOLER:

                        break;
                    
                    default:

                    break;
                }
                // case POTENCIA:
                //     pwm_set_duty(controller->potencia, control_event.value);
                //     break;
                // case CILINDRO:
                //     pwm_set_duty(controller->cilindro, control_event.value);
                //     break;
                // case TURBINA:
                //     pwm_set_duty(controller->turbina, control_event.value);
                //     break;

                // case INICIAR:
                //     torrador_state.on = !torrador_state.on;
                //     if (torrador_state.on) {
                //         time_start = esp_timer_get_time();
                //     } else {
                //         torrador_state.timer_value = 0;
                //     }
                //     break;

                // case RESFRIADOR:
                //     torrador_state.cooler_on = !torrador_state.cooler_on;
                //     break;

                // case PONTO_DE_TORRA:
                //     ESP_LOGI(TAG, "PONTO_DE_TORRA");
                //     break;

                // default:
                //     ESP_LOGI(TAG, "Evento de controle desconhecido");
                //     break;
            } else {
                ESP_LOGI("OI", "OI!!");
            }
        }
    }
}