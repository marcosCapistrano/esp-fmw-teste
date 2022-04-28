#include "wifi_connect.h"

#include <stdio.h>

#include "esp_err.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_wifi.h"

#include "freertos/event_groups.h"

#define WIFI_SSID CONFIG_WIFI_SSID
#define WIFI_PASS CONFIG_WIFI_PASSWORD
#define WIFI_MAXIMUM_RETRY CONFIG_WIFI_MAXIMUM_RETRY
#define WIFI_MAXIMUM_TIMEOUT CONFIG_WIFI_MAXIMUM_TIMEOUT

static const char* TAG = "WIFI";
static bool initialized = false;
static bool connected = false;

static EventGroupHandle_t wifi_events;
static const int CONNECTED_BIT = BIT0;
static const int DISCONNECTED_BIT = BIT1;

static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    if (event_base == WIFI_EVENT) {
        switch (event_id) {
            case WIFI_EVENT_STA_START: {
                initialized = true;
                break;
            }

            /*
            No caso de desconexão, tentamos reconectar.
            E também, é recomendado fechar todos os sockets e recria-los se necessário.
            */
            case WIFI_EVENT_STA_DISCONNECTED: {
                ESP_LOGW(TAG, "Wi-Fi desconectado!");
                xEventGroupSetBits(wifi_events, DISCONNECTED_BIT);
                connected = false;
                break;
            }
        }
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ESP_LOGI(TAG, "Wi-Fi conectado!");
        xEventGroupSetBits(wifi_events, CONNECTED_BIT);
        connected = true;
    }
}

void wifi_init(void) {
    ESP_LOGI(TAG, "Inicializando driver Wi-Fi...");
    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &event_handler,
                                                        NULL,
                                                        NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &event_handler,
                                                        NULL,
                                                        NULL));

    wifi_config_t wifi_cfg = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
        }};

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_cfg));
    ESP_ERROR_CHECK(esp_wifi_start());
}

esp_err_t wifi_connect() {
    wifi_events = xEventGroupCreate();
    esp_wifi_connect();
    EventBits_t result = xEventGroupWaitBits(wifi_events, CONNECTED_BIT | DISCONNECTED_BIT, pdTRUE, pdFALSE, WIFI_MAXIMUM_TIMEOUT / portTICK_PERIOD_MS);
    if (result == CONNECTED_BIT) {
        return ESP_OK;
    }

    return -1;
}

void wifi_task(void* params) {
    ESP_LOGI(TAG, "Inicializando task de Wi-Fi");
    unsigned char wifi_retry_num = 0;

    for (;;) {
        if (initialized && !connected) {
            if (wifi_retry_num < WIFI_MAXIMUM_RETRY) {
                ESP_LOGI(TAG, "Tentando conexão ao AP, tentativas: [%d/%d]", wifi_retry_num, WIFI_MAXIMUM_RETRY);
                wifi_retry_num++;
                esp_err_t connection = wifi_connect();
                if (connection == ESP_OK) {
                    connected = true;
                    wifi_retry_num = 0;
                } else {
                    ESP_LOGE(TAG, "Conexão ao AP falhou!");
                }
            } else {
                ESP_LOGE(TAG, "Tentando novamente em 10 segundos...");

                vTaskDelay(10000 / portTICK_PERIOD_MS);
                wifi_retry_num = 0;
            }
        }

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}