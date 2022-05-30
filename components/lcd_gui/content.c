#include "content.h"
#include "common_controller.h"

#include "stdlib.h"

content_t content_create(void *instance, content_i interface) {
    content_t content = (content_t)malloc(sizeof(s_content_t));
    content->instance = instance;
    content->interface = interface;

    return content;
}

void content_update(content_t content, controller_event_t incoming_event) {
    (content->interface->update)(content->instance, incoming_event);
}

void content_delete(content_t content) {
    (content->interface->delete)(content->instance);
}