#ifndef TORRARDOR_CONTROLLER_H
#define TORRADOR_CONTROLLER_H

void peripherals_init(void);
void toggle_fan(_Bool is_on);
void torrador_controller_task(void *params); 

#endif