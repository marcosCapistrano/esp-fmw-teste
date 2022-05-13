#include "adc.h"

#include "driver/adc.h"
#include "driver/gpio.h"
#include "esp_adc_cal.h"
#include "esp_log.h"

#define DEFAULT_VREF 1100
#define NO_OF_SAMPLES 64

static const char* TAG = "ADC";

static void print_char_val_type(esp_adc_cal_value_t val_type) {
    if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP) {
        ESP_LOGI(TAG, "Characterized using Two Point Value");
    } else if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF) {
        ESP_LOGI(TAG, "Characterized using eFuse Vref");
    } else {
        ESP_LOGI(TAG, "Characterized using Default Vref");
    }
}

adc_t adc_init(const char* name, adc1_channel_t adc_channel, adc_bits_width_t adc_width, adc_atten_t adc_atten) {
    adc_t adc = malloc(sizeof(s_adc_t));

    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_TP) == ESP_OK) {
        ESP_LOGI(TAG, "eFuse Two Point: Supported\n");
    } else {
        ESP_LOGI(TAG, "eFuse Two Point: NOT supported\n");
    }
    // Check Vref is burned into eFuse
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_VREF) == ESP_OK) {
        ESP_LOGI(TAG, "eFuse Vref: Supported\n");
    } else {
        ESP_LOGI(TAG, "eFuse Vref: NOT supported\n");
    }

    adc1_config_width(adc_width);
    adc1_config_channel_atten(adc_channel, adc_atten);

    // Characterize ADC
    adc->adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(ADC_UNIT, adc_atten, adc_width, DEFAULT_VREF, adc->adc_chars);
    print_char_val_type(val_type);

    return adc;
}

uint32_t adc_sample(adc_t adc) {
    uint32_t adc_reading = 0;

    for (int i = 0; i < NO_OF_SAMPLES; i++) {
        adc_reading += adc1_get_raw(adc->adc_channel);
    }

    adc_reading /= NO_OF_SAMPLES;

    uint32_t voltage = esp_adc_cal_raw_to_voltage(adc_reading, adc->adc_chars);

    printf("Raw: %d\tVoltage: %dmV\n", adc_reading, voltage);
    return adc_reading;
}