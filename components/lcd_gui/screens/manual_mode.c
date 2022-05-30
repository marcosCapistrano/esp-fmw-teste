#include "manual_mode.h"

#include "common_controller.h"
#include "components.h"
#include "content_manager.h"
#include "lvgl.h"
#include "screen.h"
#include "screen_manager.h"

screen_manual_t screen_manual_create(lv_obj_t *main_screen, screen_manager_t screen_manager) {
    screen_manual_t screen_manual = (screen_manual_t)malloc(sizeof(s_screen_manual_t));

    screen_manual->header = header_create(main_screen, "MODO MANUAL", screen_manager);

    controller_data_t controller_data = screen_manager->controller_data;
    QueueHandle_t incoming_queue_commands = screen_manager->incoming_queue_commands;
    QueueHandle_t outgoing_queue_lcd = screen_manager->outgoing_queue_lcd;
    content_manager_t content_manager = content_manager_init(controller_data, incoming_queue_commands, outgoing_queue_lcd, screen_manual->header, screen_manager);
    screen_manual->content_manager = content_manager;

    return screen_manual;
}

void manual_update(screen_manual_t screen_manual, controller_event_t incoming_event) {
    content_manager_update(screen_manual->content_manager, incoming_event);
}

void manual_delete(screen_manual_t screen_manual) {
    free(screen_manual);
}

screen_i manual_as_screen = &(s_screen_i){
    .update = (void *)manual_update,
    .delete = (void *)manual_delete
};