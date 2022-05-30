#include "screen_manager.h"

#include "esp_log.h"
#include "lvgl.h"
#include "menu.h"
#include "manual_mode.h"
#include "screen.h"

#define INITIAL_SCREEN MENU

screen_manager_t screen_manager_init(controller_data_t controller_data, QueueHandle_t incoming_queue_commands, QueueHandle_t outgoing_queue_lcd) {
    screen_manager_t screen_manager = malloc(sizeof(s_screen_manager_t));
    screen_manager->main_screen = lv_scr_act();
    screen_manager->incoming_queue_commands = incoming_queue_commands;
    screen_manager->outgoing_queue_lcd = outgoing_queue_lcd;
    screen_manager->controller_data = controller_data;

    screen_manager->navigation = nav_init();
    screen_manager->current_screen = 0;
    screen_manager_set_screen(screen_manager, INITIAL_SCREEN);

    return screen_manager;
}

void clear_screen(screen_manager_t screen_manager) {
    if(screen_manager->current_screen != 0) {
    lv_obj_clean(screen_manager->main_screen);
    screen_delete(screen_manager->current_screen);
    }
}

void screen_manager_go_back(screen_manager_t screen_manager) {
    nav_t *head = &(screen_manager->navigation);
    nav_pop(head);

    screen_manager_set_screen(screen_manager, screen_manager->navigation->screen);
}

void screen_manager_set_screen(screen_manager_t screen_manager, screens_t screen_type) {
    clear_screen(screen_manager);

    nav_t *head = &(screen_manager->navigation);
    nav_push(head, screen_type);

    switch (screen_type) {
        case MENU: {
            screen_menu_t screen_menu = screen_menu_create(screen_manager->main_screen, screen_manager);
            screen_manager->current_screen = screen_create(screen_menu, menu_as_screen);
            break;
        }

        case MANUAL_MODE: {
           screen_manual_t screen_manual = screen_manual_create(screen_manager->main_screen, screen_manager);
           screen_manager->current_screen = screen_create(screen_manual, manual_as_screen); 
        }

        default:

            break;
    }
}

void screen_manager_update(screen_manager_t screen_manager, controller_event_t incoming_event) {
    screen_update(screen_manager->current_screen, incoming_event);
}

nav_t nav_init() {
    nav_t nav = malloc(sizeof(s_nav_t));

    nav->screen = INITIAL_SCREEN;
    nav->next = NULL;

    return nav;
}

void nav_push(nav_t *nav, screens_t screen) {
    nav_t new_head = (nav_t)malloc(sizeof(s_nav_t));

    new_head->screen = screen;
    new_head->next = *nav;

    *nav = new_head;
}

void nav_pop(nav_t *nav) {
    nav_t temp_head = *nav;

    *nav = temp_head->next;
    free(temp_head);
}