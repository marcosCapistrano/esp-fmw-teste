#include "torrador_controller.h"

#include <stdio.h>
#include <stdlib.h>

#include "common_torrador_controller.h"
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

static const char *TAG = "TORRADOR_CONTROLLER";

typedef struct {
    torrador_state_t *state;
    QueueHandle_t state_event_queue;
} notify_cb_params_t;

void peripherals_init(void) {
    ESP_ERROR_CHECK(mcpwm_gpio_init(MCPWM_UNIT, MCPWM0A, MCPWM_POTENCIA));
    ESP_ERROR_CHECK(mcpwm_gpio_init(MCPWM_UNIT, MCPWM1A, MCPWM_CILINDRO));
    ESP_ERROR_CHECK(mcpwm_gpio_init(MCPWM_UNIT, MCPWM2A, MCPWM_TURBINA));

    const mcpwm_config_t pwm_config = {
        .frequency = MCPWM_FREQ_HZ,
        .cmpr_a = 0.0,
        .cmpr_b = 0.0,
        .duty_mode = MCPWM_DUTY_MODE_0,
        .counter_mode = MCPWM_UP_COUNTER};

    ESP_ERROR_CHECK(mcpwm_init(MCPWM_UNIT, MCPWM_TIMER_POTENCIA, &pwm_config));
    ESP_ERROR_CHECK(mcpwm_init(MCPWM_UNIT, MCPWM_TIMER_CILINDRO, &pwm_config));
    ESP_ERROR_CHECK(mcpwm_init(MCPWM_UNIT, MCPWM_TIMER_TURBINA, &pwm_config));
    ESP_LOGI(TAG, "Started peripherals");
}

void set_pwm(control_event_type_t event_type, int value) {
    switch (event_type) {
        case POTENCIA:
            ESP_LOGI(TAG, "POTENCIA: %d", value);
            mcpwm_set_duty(MCPWM_UNIT, MCPWM_TIMER_POTENCIA, MCPWM_GEN_A, value);
            break;

        case CILINDRO:

            ESP_LOGI(TAG, "CILINDRO: %d", value);
            mcpwm_set_duty(MCPWM_UNIT, MCPWM_TIMER_CILINDRO, MCPWM_GEN_A, value);
            break;

        case TURBINA:
            ESP_LOGI(TAG, "TURBINA: %d", value);
            mcpwm_set_duty(MCPWM_UNIT, MCPWM_TIMER_TURBINA, MCPWM_GEN_A, value);
            break;

        default:
            break;
    }
}

static void notify_listeners_cb(TimerHandle_t timer) {
    notify_cb_params_t *params = (notify_cb_params_t *)pvTimerGetTimerID(timer);
    torrador_state_t *state = params->state;
    QueueHandle_t state_event_queue = params->state_event_queue;

    xQueueOverwrite(state_event_queue, &state);
}

void torrador_controller_task(void *pvParameters) {
    torrador_controller_params_t *params = (torrador_controller_params_t *)pvParameters;
    QueueHandle_t control_event_queue = params->control_event_queue;  // Queue com os eventos que chegaram da IHM/WEB
    QueueHandle_t state_event_queue = params->state_event_queue;

    torrador_state_t torrador_state = {
        .is_on = false,
        .timer_value = 0};

    notify_cb_params_t notify_cb_params = {
        .state = &torrador_state,
        .state_event_queue = state_event_queue};

    control_event_t control_event;  // Segura o evento atual
    BaseType_t xStatus;

    int64_t time_start = 0;
    esp_timer_handle_t timer_handle;
    esp_timer_create_args_t timer_conf = {
        .callback = NULL,
        .arg = NULL,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "TIMER",
        .skip_unhandled_events = true};

    esp_timer_create(&timer_conf, &timer_handle);
    esp_timer_start_once(timer_handle, portMAX_DELAY);

    for (;;) {
        if (torrador_state.is_on) {
            int64_t time_now = esp_timer_get_time() - time_start;
            torrador_state.timer_value = time_now;
            ESP_LOGI(TAG, "%d", (int)torrador_state.timer_value);
        }

        xStatus = xQueueReceive(control_event_queue, &control_event, pdMS_TO_TICKS(250));

        if (xStatus == pdPASS) {
            switch (control_event.type) {
                case POTENCIA:
                case CILINDRO:
                case TURBINA:
                    set_pwm(control_event.type, control_event.value);
                    break;

                case INICIAR:
                    ESP_LOGI(TAG, "INICIAR");
                    torrador_state.is_on = true;
                    time_start = esp_timer_get_time();
                    timer_handle = xTimerCreate("TIMER", pdMS_TO_TICKS(1000), pdTRUE, &notify_cb_params, notify_listeners_cb);
                    xTimerStart(timer_handle, portMAX_DELAY);
                    break;

                case PARAR:
                    torrador_state.is_on = false;
                    ESP_LOGI(TAG, "PARAR");
                    break;

                case PONTO_DE_TORRA:
                    ESP_LOGI(TAG, "PONTO_DE_TORRA");
                    break;

                default:
                    ESP_LOGI(TAG, "Evento de controle desconhecido");
                    break;
            }
        }
    }
}
