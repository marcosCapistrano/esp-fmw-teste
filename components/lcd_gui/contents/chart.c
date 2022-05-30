#include "chart.h"

#include "components.h"
#include "lvgl.h"
#include "content.h"

content_chart_t content_chart_create(lv_obj_t *content_area, content_manager_t content_manager) {
    content_manager->current_content_type = CHART;

    content_chart_t content_chart = (content_chart_t)malloc(sizeof(s_content_chart_t));

    content_chart->chart_obj = chart_create(content_area);
    content_chart->content_manager = content_manager;
    return content_chart;
}

void chart_update(content_chart_t content_chart, controller_event_t incoming_event) {
    controller_data_t controller_data = content_chart->content_manager->controller_data;

    if(incoming_event == STAGE_EVENT && controller_data->read_stage == STAGE_OFF) {
        chart_reset(content_chart->chart_obj);
    }

    if(incoming_event == SENSOR_VALUE_EVENT) {
        switch(controller_data->read_stage) {
            case PRE_HEATING:
                chart_draw_pre_heating(content_chart->chart_obj, controller_data->read_temp_grao);
            break;

            case START:
                chart_draw_start(content_chart->chart_obj, controller_data);
            break;

            default: ;
        }
    }
}

void chart_delete(content_chart_t content_chart) {
    // free(content_chart->chart_obj);
    // free(content_chart);
}

content_i chart_as_content = &(s_content_i){
    .update = (void *)chart_update,
    .delete = (void *)chart_delete};