#include "screen_manager.h"

#include "esp_log.h"

#include "lvgl.h"
#include "common_controller.h"
#include "manual_mode.h"
#include "menu.h"

static nav_t nav_init(screen_t screen);
static void nav_push(nav_t *nav, screen_t screen);
static void nav_pop(nav_t *nav);

screen_manager_t screen_manager_init(controller_data_t controller_data, QueueHandle_t incoming_queue_commands, QueueHandle_t outgoing_queue_lcd) {
    screen_manager_t screen_manager = malloc(sizeof(s_screen_manager_t));
    screen_manager->main_screen = lv_scr_act();
    screen_manager->current_screen = SCREEN_NONE;
    screen_manager->current_nav = nav_init(MENU);
    screen_manager->incoming_queue_commands = incoming_queue_commands;
    screen_manager->outgoing_queue_lcd = outgoing_queue_lcd;
    screen_manager->controller_data = controller_data;

    screen_manager->menu_obj = screen_menu_init(lv_scr_act(), screen_manager);
    screen_manager->manual_mode_obj = NULL;

    return screen_manager;
}

void screen_manager_update(screen_manager_t screen_manager, controller_data_t controller_data) {
	switch(screen_manager->current_screen) {
        case MENU:
        break;

        case MANUAL_MODE:
            screen_manual_mode_update(screen_manager);
        break;

        case AUTO_MODE:

        break;

        default:
        break;
    }
}

void clear_screen(screen_manager_t screen_manager) {
    lv_obj_clean(screen_manager->main_screen);

    switch (screen_manager->current_screen) {
        case MENU:
            free_screen_menu(screen_manager->menu_obj);
            break;

        default:

            break;
    }
}

void screen_manager_go_back(screen_manager_t screen_manager) {
    nav_t *head = &(screen_manager->current_nav);
    nav_pop(head);
    screen_manager_set_screen(screen_manager, screen_manager->current_nav->screen);
}

void screen_manager_set_screen(screen_manager_t screen_manager, screen_t screen) {
    clear_screen(screen_manager);
    screen_manager->current_screen = SCREEN_NONE;

    nav_t *head = &(screen_manager->current_nav);
    nav_push(head, screen);

    switch (screen) {
        case MENU:
            screen_manager->menu_obj = screen_menu_init(screen_manager->main_screen, screen_manager);
            break;

        case MANUAL_MODE:
            screen_manager->manual_mode_obj = screen_manual_mode_init(screen_manager->main_screen, screen_manager);
        default:

            break;
    }
}

nav_t nav_init(screen_t screen) {
    nav_t nav = malloc(sizeof(s_nav_t));

    nav->screen = screen;
    nav->next = NULL;

    return nav;
}

void nav_push(nav_t *nav, screen_t screen) {
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