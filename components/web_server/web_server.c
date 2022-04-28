#include "web_server.h"

#include <stdio.h>

#include "esp_http_server.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "mdns.h"

static const char* TAG = "WEB_SERVER";

QueueHandle_t queue_to_controller;
static bool is_on = false;

esp_err_t on_default_url(httpd_req_t *req) {
    httpd_resp_sendstr(req, "Hello, world!");
    return ESP_OK;
}

esp_err_t on_fan_url(httpd_req_t *req) {
    is_on = !is_on;
		ESP_LOGI(TAG, "Toggling fan to %d", is_on);
    long ok = xQueueSend((QueueHandle_t) queue_to_controller, &is_on, 10 / portTICK_PERIOD_MS);
    if (ok) {
        httpd_resp_sendstr(req, "OK");
    } else {
        httpd_resp_sendstr(req, "ERROR");
    }
    return ESP_OK;
}

void mdns_service_init(void) {
    mdns_init();
    mdns_hostname_set("ausyx");
    mdns_instance_name_set("torrador");
}

void web_server_init(QueueHandle_t queue) {
    queue_to_controller = queue;
    httpd_handle_t server = NULL;
    httpd_config_t server_cfg = HTTPD_DEFAULT_CONFIG();

    httpd_start(&server, &server_cfg);

    httpd_uri_t fan_url = {
        .uri = "/fan",
        .method = HTTP_GET,
        .handler = on_fan_url};

    httpd_uri_t default_url = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = on_default_url};

    httpd_register_uri_handler(server, &fan_url);
    httpd_register_uri_handler(server, &default_url);
    mdns_service_init();
}