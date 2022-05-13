idf_component_register(SRCS "controller.c" "pwm.c" "adc.c"
                    INCLUDE_DIRS "include"
                    REQUIRES common esp_timer driver esp_adc_cal)
