#include "driver/mcpwm.h"
#include "esp_log.h"

#include "pwm.h"

#define MCPWM_FREQ_HZ 4095

static const char* TAG = "PWM";

pwm_t pwm_init(const char* name, const mcpwm_unit_t mcpwm_unit, const mcpwm_timer_t timer_num, const mcpwm_io_signals_t io_signal, int gpio_num) {
    pwm_t pwm = malloc(sizeof(pwm_t));

    const mcpwm_config_t pwm_config = {
        .frequency = MCPWM_FREQ_HZ,
        .cmpr_a = 0.0,
        .cmpr_b = 0.0,
        .duty_mode = MCPWM_DUTY_MODE_0,
        .counter_mode = MCPWM_UP_COUNTER};

    pwm->name = name;
    pwm->mcpwm_unit = mcpwm_unit;
    pwm->timer_num = timer_num;

    ESP_ERROR_CHECK(mcpwm_gpio_init(mcpwm_unit, io_signal, gpio_num));
    ESP_ERROR_CHECK(mcpwm_gpio_init(mcpwm_unit, io_signal, gpio_num));
    ESP_ERROR_CHECK(mcpwm_init(mcpwm_unit, timer_num, &pwm_config));

    return pwm;
}

void pwm_set_duty(pwm_t pwm, int value) {
    ESP_LOGI(TAG, "SETTING DUTY %d", value);
    ESP_ERROR_CHECK(mcpwm_set_duty(pwm->mcpwm_unit, pwm->timer_num, MCPWM_GEN_A, value));
}