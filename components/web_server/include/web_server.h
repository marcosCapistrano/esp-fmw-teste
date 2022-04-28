#ifndef WEB_SERVER_H
#define WEB_SERVER_H
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

void web_server_init(QueueHandle_t queue);

#endif
