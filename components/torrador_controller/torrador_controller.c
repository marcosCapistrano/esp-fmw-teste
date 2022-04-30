#include "torrador_controller.h"

#include <stdio.h>
#include <stdlib.h>

#include "driver/mcpwm.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#define PWM_A 0
#define PWM_B 4

#define MCPWM_NUM MCPWM_UNIT_0
#define MCPWM_TIMER_NUM MCPWM_TIMER_0

static const char *TAG = "TORRADOR_CONTROLLER";
static _Bool direction = false; 

void peripherals_init(void) {
    const mcpwm_unit_t mcpwm_num = MCPWM_NUM;

    mcpwm_gpio_init(mcpwm_num, MCPWM0A, PWM_A);
    mcpwm_gpio_init(mcpwm_num, MCPWM0B, PWM_B);

		const mcpwm_timer_t timer_num = MCPWM_TIMER_NUM;
		const mcpwm_config_t pwm_config = {
        .frequency = 5000,
        .cmpr_a = 0.0,
        .cmpr_b = 0.0,
        .counter_mode = MCPWM_UP_COUNTER,
        .duty_mode = MCPWM_DUTY_MODE_0,
    };

		mcpwm_init(mcpwm_num, timer_num, &pwm_config);
}

void toggle_fan(_Bool is_on) {
	if(is_on) {
		if(direction) {
		mcpwm_set_signal_high(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A);
		} else {
		mcpwm_set_signal_high(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B);
		}
	} else {
		direction = !direction;
		mcpwm_set_signal_low(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A);
		mcpwm_set_signal_low(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B);
	}
}

void torrador_controller_task(void *params) {
	QueueHandle_t queue = (QueueHandle_t) params;

	for(;;) {
		_Bool rx_is_on;
		if(xQueueReceive(queue, &rx_is_on, 5000 / portTICK_PERIOD_MS)) {
			ESP_LOGI(TAG, "Received: %d", rx_is_on);
			toggle_fan(rx_is_on);
		}
	}
}
