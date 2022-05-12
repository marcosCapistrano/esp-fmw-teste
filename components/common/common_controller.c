#include "common_controller.h"

const char *controller_stage_to_string(controller_stage_t stage) {
    switch (stage) {
        case STAGE_OFF:
            return "DESLIGADO";
            break;
        case PRE_HEATING:
            return "PRE-AQUECENDO";
        case START:
            return "TORRA EM ANDAMENTO";
            break;
        case END:
            return "FINALIZANDO";
            break;
        case COOLER:
            return "RESFRIANDO";
            break;

        default:
            return "UNKNOWN";
            break;
    }
}

const char *controller_stage_to_string_verb(controller_stage_t stage) {
    switch (stage) {
        case STAGE_OFF:
            return "DESLIGAR";
            break;
            
        case PRE_HEATING:
            return "PRE-AQUECER";
        case START:
            return "INICIAR";
            break;
        case END:
            return "FINALIZAR";
            break;
        case COOLER:
            return "RESFRIADOR";
            break;

        default:
            return "---";
            break;
    }
}

incoming_data_t incoming_data_init() {
    incoming_data_t incoming_data = malloc(sizeof(s_incoming_data_t));

    incoming_data->reader_type = READER_NONE;
    incoming_data->sender_id = -1;

    incoming_data->state = STATE_NONE;
    incoming_data->mode = MODE_NONE;
    incoming_data->stage = STAGE_NONE;

    incoming_data->write_recipe_data = NULL;
    incoming_data->write_sensor_data = NULL;

    incoming_data->write_potencia = -1;
    incoming_data->write_cilindro = -1;
    incoming_data->write_turbina = -1;

    return incoming_data;
}