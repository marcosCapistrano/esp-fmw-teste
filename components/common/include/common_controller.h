#ifndef COMMON_CONTROLLER_H
#define COMMON_CONTROLLER_H

#define CONTROLLER_MAX_TIME_MINS 25
#include <time.h>

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
    END,
    STAGE_NONE,
} controller_stage_t;


// Valores que serão usados para salvar as receitas e mostrar gráfico nas páginas
typedef struct s_recipe_data_t {
    int pre_heating_temp;
    int array_pot[CONTROLLER_MAX_TIME_MINS * 2 + 2];
    int array_cilindro[CONTROLLER_MAX_TIME_MINS * 2 + 2];
    int array_turbina[CONTROLLER_MAX_TIME_MINS * 2 + 2];
} s_recipe_data_t;

typedef struct s_recipe_data_t *recipe_data_t;

typedef struct s_sensor_data_t {
    int array_temp_ar[CONTROLLER_MAX_TIME_MINS * 2 + 2];
    int array_temp_grao[CONTROLLER_MAX_TIME_MINS * 2 + 2];
    int array_grad[CONTROLLER_MAX_TIME_MINS * 2 + 2];
    int array_delta_grao[CONTROLLER_MAX_TIME_MINS * 2 + 2];
} s_sensor_data_t;

typedef struct s_sensor_data_t *sensor_data_t;

typedef struct s_controller_data_t {
    controller_state_t read_state;
    controller_mode_t read_mode;
    controller_stage_t read_stage;

    controller_state_t write_state;
    controller_mode_t write_mode;
    controller_stage_t write_stage;

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
    int read_grad;

    recipe_data_t read_recipe_data;
    sensor_data_t read_sensor_data;
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

    recipe_data_t write_recipe_data;
    recipe_data_t write_sensor_data;

    int write_potencia;
    int write_cilindro;
    int write_turbina;
} s_incoming_data_t;

typedef struct s_incoming_data_t *incoming_data_t;

/*
    Estrutura de dados utilizada pelo controller para enviar notificações para os readers
*/

typedef struct s_outgoing_data_t {
    time_t timestamp;
    const char *message;
} s_outgoing_data_t;

typedef struct s_outgoing_data_t *outgoing_data_t;

const char *controller_stage_to_string(controller_stage_t stage);
const char *controller_stage_to_string_verb(controller_stage_t stage);

incoming_data_t incoming_data_init();

#endif