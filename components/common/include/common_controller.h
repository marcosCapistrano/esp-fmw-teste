#ifndef COMMON_CONTROLLER_H
#define COMMON_CONTROLLER_H

#define CONTROLLER_MAX_TIME_MINS 25
#include <time.h>

typedef enum {
    STATE_EVENT,
    STAGE_EVENT,
    MODE_EVENT,
    TIMER_VALUE_EVENT,
    SENSOR_VALUE_EVENT,
    ACTUATOR_VALUE_EVENT,
} controller_event_t;

typedef enum {
    STATE_NONE,
    ON,
    OFF
} controller_state_t;

typedef enum {
    MODE_NONE,
    MANUAL,
    AUTO
} controller_mode_t;

typedef enum {
    STAGE_OFF,
    PRE_HEATING,
    START,
    COOLER,
    STAGE_NONE,
} controller_stage_t;

typedef enum {
    SAVE,
    ERASE,
    CHOICE_NONE
} controller_choice_t;

typedef struct s_recipe_data_node_t *recipe_data_node_t;
typedef struct s_recipe_data_node_t {
    int potencia;
    int cilindro;
    int turbina;
    int64_t time;

    recipe_data_node_t next;
} s_recipe_data_node_t;

typedef struct s_recipe_data_t *recipe_data_t;
typedef struct s_recipe_data_t {
    int pre_heating_temp;
    recipe_data_node_t data;
} s_recipe_data_t;

typedef struct s_sensor_data_node_t *sensor_data_node_t;
typedef struct s_sensor_data_node_t {
    int temp_ar;
    int temp_grao;
    int delta_ar;
    int delta_grao;
    int64_t time;

    sensor_data_node_t next;
} s_sensor_data_node_t;

typedef struct s_sensor_data_t *sensor_data_t;
typedef struct s_sensor_data_t {
    sensor_data_node_t data;
} s_sensor_data_t;

typedef struct s_controller_data_t {
    controller_state_t read_state;
    controller_mode_t read_mode;
    controller_stage_t read_stage;

    controller_state_t write_state;
    controller_mode_t write_mode;
    controller_stage_t write_stage;
    controller_choice_t write_choice;

    recipe_data_t write_recipe_data;
    sensor_data_t write_sensor_data;

    int write_potencia;
    int write_cilindro;
    int write_turbina;

    int read_potencia;
    int read_cilindro;
    int read_turbina;
    int read_temp_ar;
    int read_temp_grao;
    int read_delta_ar;
    int read_delta_grao;

    uint64_t read_torra_time;
    uint64_t read_resf_time;

    recipe_data_t read_recipe_data;
    sensor_data_t read_sensor_data;

    uint64_t start_time;
    uint64_t elapsed_time;
} s_controller_data_t;

typedef struct s_controller_data_t *controller_data_t;

typedef enum {
    READER_NONE,
    LCD,
    SERVER
} controller_reader_t;

/*
    Estrutura de dados utilizada pelos readers para enviar comandos para o controller
*/

typedef struct s_incoming_data_t {
    controller_reader_t reader_type;
    int sender_id;

    controller_state_t state;
    controller_mode_t mode;
    controller_stage_t stage;
    controller_choice_t choice;

    recipe_data_t write_recipe_data;
    recipe_data_t write_sensor_data;

    int write_potencia;
    int write_cilindro;
    int write_turbina;
} s_incoming_data_t;

typedef struct s_incoming_data_t *incoming_data_t;

/*
    Estrutura de dados utilizada pelo controller para enviar notifica????es para os readers
*/

typedef struct s_outgoing_data_t *outgoing_data_t;
typedef struct s_outgoing_data_t {
} s_outgoing_data_t;

const char *controller_stage_to_string(controller_stage_t stage);
const char *controller_stage_to_string_verb(controller_stage_t stage);

incoming_data_t incoming_data_init();

#endif