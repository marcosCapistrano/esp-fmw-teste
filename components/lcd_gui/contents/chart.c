#include "chart.h"

#include "components.h"
#include "lvgl.h"
#include "content.h"

content_chart_t content_chart_create(lv_obj_t *content_area, content_manager_t content_manager) {
    content_manager->current_content_type = CHART;

    content_chart_t content_chart = (content_chart_t)malloc(sizeof(s_content_chart_t));

    content_chart->chart_obj = chart_create(content_area);
    return content_chart;
}

void chart_update(content_chart_t content_chart) {
}

void chart_delete(content_chart_t content_chart) {
    // free(content_chart->chart_obj);
    // free(content_chart);
}

content_i chart_as_content = &(s_content_i){
    .update = (void *)chart_update,
    .delete = (void *)chart_delete};