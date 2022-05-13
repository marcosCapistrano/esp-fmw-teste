#ifndef ADC_H
#define ADC_H

#include "esp_adc_cal.h"
#include "driver/adc.h"

#define ADC_UNIT ADC_UNIT_1

typedef struct s_adc_t* adc_t;

typedef struct s_adc_t {
	const char* name;
	adc1_channel_t adc_channel;
	esp_adc_cal_characteristics_t *adc_chars;
} s_adc_t;

adc_t adc_init(const char* name, adc1_channel_t adc_channel, adc_bits_width_t adc_width, adc_atten_t adc_atten);
uint32_t adc_sample(adc_t adc);

#endif