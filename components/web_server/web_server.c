#include "web_server.h"

#include <stdio.h>

#include "common_params.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include "esp_spiffs.h"
#include "freertos/FreeRTOS.h"
#include "mdns.h"

static const char *TAG = "WEB_SERVER";

static bool is_on = false;

esp_err_t on_default_url(httpd_req_t *req) {
    esp_vfs_spiffs_conf_t esp_vfs_spiffs_conf = {
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = 7,
        .format_if_mount_failed = true,
    };

    esp_vfs_spiffs_register(&esp_vfs_spiffs_conf);

    char path[600];
    if (strcmp(req->uri, "/") == 0) {
        strcpy(path, "/spiffs/index.html");
    } else {
        sprintf(path, "/spiffs%s", req->uri);
    }

    char *ext = strrchr(path, '.');
    if (strcmp(ext, ".js") == 0) {
        httpd_resp_set_type(req, "text/javascript");
    } else if (strcmp(ext, ".css") == 0) {
        httpd_resp_set_type(req, "text/css");
    } else if (strcmp(ext, ".png") == 0) {
        httpd_resp_set_type(req, "image/png");
    } else if (strcmp(ext, ".ico") == 0) {
        httpd_resp_set_type(req, "image/x-icon");
    }

    FILE *file = fopen(path, "r");
    if (file == NULL) {
        httpd_resp_send_404(req);
        esp_vfs_spiffs_unregister(NULL);
        return ESP_OK;
    }

    char line_read[256];
    while (fgets(line_read, sizeof(line_read), file)) {
        httpd_resp_sendstr_chunk(req, line_read);
    }
    httpd_resp_sendstr_chunk(req, NULL);

    esp_vfs_spiffs_unregister(NULL);
    return ESP_OK;
}

esp_err_t on_fan_url(httpd_req_t *req) {
    is_on = !is_on;
    ESP_LOGI(TAG, "Toggling fan to %d", is_on);
    // long ok = xQueueSend((QueueHandle_t)queue_to_controller, &is_on, 10 / portTICK_PERIOD_MS);
    // if (ok) {
    //     httpd_resp_sendstr(req, "OK");
    // } else {
    //     httpd_resp_sendstr(req, "ERROR");
    // }
    return ESP_OK;
}

// #define WS_MAX_SIZE 1024
// static int client_session_id;

// esp_err_t on_web_socket_url(httpd_req_t *req) {
//     client_session_id = httpd_req_to_sockfd(req);
//     if (req->method == HTTP_GET) return ESP_OK;

//     httpd_ws_frame_t ws_pkt;
//     memset(&ws_pkt, 0 sizeof(httpd_ws_frame_t));
//     ws_pkt.type = HTTPD_WS_TYPE_TEXT;
//     ws_pkt.payload = malloc(WS_MAX_SIZE);

//     return ESP_OK;
// }

void mdns_service_init(void) {
    mdns_init();
    mdns_hostname_set("ausyx");
    mdns_instance_name_set("torrador");
}

void web_server_init() {
    httpd_handle_t server = NULL;
    httpd_config_t server_cfg = HTTPD_DEFAULT_CONFIG();
    server_cfg.uri_match_fn = httpd_uri_match_wildcard;

    httpd_start(&server, &server_cfg);

    httpd_uri_t fan_url = {
        .uri = "/fan",
        .method = HTTP_GET,
        .handler = on_fan_url};

    httpd_uri_t default_url = {
        .uri = "/*",
        .method = HTTP_GET,
        .handler = on_default_url};

    // httpd_uri_t web_socket_url = {
    //     .uri = "/ws",
    //     .method = HTTP_GET,
    //     .handler = on_web_socket_url,
    //     .is_websocket = true,
    // };

    httpd_register_uri_handler(server, &fan_url);
    httpd_register_uri_handler(server, &default_url);
    // httpd_register_uri_handler(server, &web_socket_url);
    mdns_service_init();
}

void web_server_task(void *pvParameters) {
    controller_params_t params = (controller_params_t)pvParameters;

    for (;;) {
    }
}