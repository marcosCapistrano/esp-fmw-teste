#ifndef SCREEN_H
#define SCREEN_H

#include "common_controller.h"

/*
	Segura ponteiros pra funções que implementam as funções concretas
*/
typedef enum {
    MENU,
    MANUAL_MODE,
    AUTO_MODE,
} screens_t;

typedef struct s_screen_i *screen_i;
typedef struct s_screen_i {
	void (*update)(void *instance, controller_event_t incoming_event);
	void (*delete)(void *instance);
} s_screen_i;

typedef struct s_screen_t *screen_t;
typedef struct s_screen_t {
	void *instance;
	const s_screen_i *interface;
} s_screen_t;

screen_t screen_create(void *instance, screen_i interface);
void screen_delete(screen_t screen);
void screen_update(screen_t screen, controller_event_t incoming_event);

#endif