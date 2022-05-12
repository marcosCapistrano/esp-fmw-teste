#include "common_controller.h"

const char *controller_stage_to_string(controller_stage_t stage) {
    switch (stage) {
        case PRE_HEATING:
            return "PRE_HEATING";
        case START:
            return "START";
            break;
        case END:
            return "END";
            break;
        case COOLER:
            return "COOLER";
            break;

        default:
            return "UNKNOWN";
            break;
    }
}