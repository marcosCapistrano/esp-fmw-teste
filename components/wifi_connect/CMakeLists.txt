idf_component_register(SRCS "wifi_connect.c"
                    INCLUDE_DIRS "include"
                    REQUIRES esp_netif esp_event esp_wifi)
