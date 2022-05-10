idf_component_register(SRCS "controller.c" "pwm.c"
                    INCLUDE_DIRS "include"
                    REQUIRES common esp_timer driver)
