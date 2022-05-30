#ifndef CONTENT_H
#define CONTENT_H

#include "common_controller.h"
/*
	Segura ponteiros pra funções que implementam as funções concretas
*/
typedef enum {
    CHART,
		CONTROLS
} contents_t;

typedef struct s_content_i *content_i;
typedef struct s_content_i {
	void (*update)(void *instance, controller_event_t incoming_event);
	void (*delete)(void *instance);
} s_content_i;

typedef struct s_content_t *content_t;
typedef struct s_content_t {
	void *instance;
	const s_content_i *interface;
} s_content_t;

content_t content_create(void *instance, content_i interface);
void content_update(content_t content, controller_event_t incoming_event);
void content_delete(content_t content);

#endif