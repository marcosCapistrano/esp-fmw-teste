#ifndef COMMON_H
#define COMMON_H

typedef enum {
    MANUAL,
    AUTO
} controller_mode_t;

typedef enum {
    STAGE_NONE,
    PRE_HEATING,
    START,
    END,
    COOLER,
    OFF,
} stage_t;

#endif