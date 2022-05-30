#include "screen.h"
#include "common_controller.h"
#include "stdlib.h"

screen_t screen_create(void *instance, screen_i interface) {
    screen_t screen = (screen_t)malloc(sizeof(s_screen_t));
    screen->instance = instance;
    screen->interface = interface;

    return screen;
}

void screen_update(screen_t screen, controller_event_t incoming_event) {
    (screen->interface->update)(screen->instance, incoming_event);
}

void screen_delete(screen_t screen) {
    (screen->interface->delete)(screen->instance);
}