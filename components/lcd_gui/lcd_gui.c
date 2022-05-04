#include "lcd_gui.h"

#include "time.h"

#include <lvgl.h>
#include <lvgl_esp32_drivers/lvgl_helpers.h>
#include <stdio.h>

#include "common_torrador_controller.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

#define AUSYX_HOR_RES 480
#define AUSYX_VER_RES 320
#define LV_TICK_PERIOD_MS 1

static lv_disp_draw_buf_t disp_buf;
static lv_disp_drv_t disp_drv;
static lv_indev_drv_t indev_drv;
static lv_indev_t *indev;

static SemaphoreHandle_t xGuiSemaphore;
static QueueHandle_t control_event_queue;
static QueueHandle_t state_event_queue;

static lv_obj_t *timer;
static lv_obj_t *temp_grao;
static lv_obj_t *temp_ar;

typedef struct {
    control_event_type_t event_type;
    lv_obj_t *label;
} arc_event_user_data_t;

static void lv_tick_task(void *arg);

static void start_event_cb(lv_event_t *event) {
    xQueueSendToBack(control_event_queue, &(control_event_t){.type = INICIAR}, 0);
}

lv_obj_t *base_container(lv_obj_t *view) {
    lv_obj_t *container = lv_obj_create(view);
    lv_obj_set_style_pad_all(container, 4, 0);
    lv_obj_set_style_pad_right(container, 8, 0);
    lv_obj_set_style_border_width(container, 0, 0);
    lv_obj_set_style_outline_width(container, 0, 0);

    return container;
}

lv_obj_t *base_status_container(lv_obj_t *view, char *label) {
    lv_obj_t *container = base_container(view);
    lv_obj_t *container_label = lv_label_create(container);

    lv_label_set_text(container_label, label);
    lv_obj_set_align(container_label, LV_ALIGN_BOTTOM_MID);

    return container;
}

lv_obj_t *timer_container(lv_obj_t *view) {
    lv_obj_t *container = base_status_container(view, "Timer");
    timer = lv_label_create(container);
    lv_label_set_text(timer, "00:00");
    lv_obj_set_align(timer, LV_ALIGN_CENTER);
    lv_obj_set_style_translate_y(timer, -7, 0);

    lv_obj_set_style_text_font(timer, &lv_font_montserrat_14, 0);

    return container;
}

lv_obj_t *temp_container(lv_obj_t *view, char *label, lv_obj_t *temp_label_obj) {
    lv_obj_t *container = base_status_container(view, label);
    temp_label_obj = lv_label_create(container);

    lv_obj_set_style_text_font(temp_label_obj, &lv_font_montserrat_14, 0);
    lv_label_set_text(temp_label_obj, "0 °C");
    lv_obj_set_align(temp_label_obj, LV_ALIGN_CENTER);
    lv_obj_set_style_translate_y(temp_label_obj, -7, 0);

    return container;
}

void arc_event_cb(lv_event_t *e) {
    lv_obj_t *arc = lv_event_get_target(e);

    arc_event_user_data_t *user_data = lv_event_get_user_data(e);
    lv_obj_t *label = user_data->label;
    control_event_type_t event_type = user_data->event_type;

    control_event_t event = {event_type, (int)lv_arc_get_value(arc)};

    ESP_LOGI("arc_event_cb", "arc_event_cb: %d", event_type);
    xQueueSendToBack(control_event_queue, &event, 100);

    lv_label_set_text_fmt(label, "%d", lv_arc_get_value(arc));
}

lv_obj_t *arc_container(lv_obj_t *view, char *label, char *unit, control_event_type_t event_type) {
    lv_obj_t *container = base_status_container(view, label);
    lv_obj_set_style_bg_color(container, lv_color_make(248, 248, 248), 0);

    lv_obj_t *arc = lv_arc_create(container);
    lv_obj_set_size(arc, 90, 90);
    lv_arc_set_rotation(arc, 135);
    lv_arc_set_bg_angles(arc, 0, 270);
    lv_arc_set_value(arc, 10);
    lv_obj_center(arc);
    lv_obj_set_style_translate_y(arc, -5, 0);

    lv_obj_t *arc_label = lv_label_create(container);
    lv_label_set_text(arc_label, "0");
    lv_obj_set_style_text_font(arc_label, &lv_font_montserrat_14, 0);
    lv_obj_set_style_translate_y(arc_label, -10, 0);
    lv_obj_set_align(arc_label, LV_ALIGN_CENTER);

    lv_obj_t *arc_unit_label = lv_label_create(container);
    lv_label_set_text(arc_unit_label, unit);
    lv_obj_set_style_text_font(arc_unit_label, &lv_font_montserrat_14, 0);
    lv_obj_set_align(arc_unit_label, LV_ALIGN_CENTER);
    lv_obj_set_style_translate_y(arc_unit_label, 10, 0);

    arc_event_user_data_t *user_data = malloc(sizeof(arc_event_user_data_t));
    user_data->event_type = event_type;
    user_data->label = arc_label;

    lv_obj_add_event_cb(arc, arc_event_cb, LV_EVENT_VALUE_CHANGED, user_data);
    lv_event_send(arc, LV_EVENT_VALUE_CHANGED, NULL);

    return container;
}

lv_obj_t *button_container(lv_obj_t *view, char *label) {
    lv_obj_t *button = lv_btn_create(view);
    lv_obj_t *button_label = lv_label_create(button);

    lv_label_set_text(button_label, label);
    lv_obj_center(button_label);

    return button;
}

static void draw_event_cb(lv_event_t *e) {
    lv_obj_draw_part_dsc_t *dsc = lv_event_get_param(e);
    if (dsc->part == LV_PART_TICKS && dsc->id == LV_CHART_AXIS_PRIMARY_X) {
        const char *time[] = {"00:00", "00:30", "01:00", "01:30", "02:00", "02:30", "03:00", "03:30", "04:00", "04:30", "05:00", "05:30", "06:00", "06:30", "07:00", "07:30", "08:00", "08:30", "09:00", "09:30", "10:00", "10:30", "11:00", "11:30", "12:00", "12:30", "13:00", "13:30", "14:00", "14:30", "15:00", "15:30", "16:00", "16:30", "17:00", "17:30", "18:00", "18:30", "19:00", "19:30", "20:00", "20:30", "21:00", "21:30", "22:00", "22:30", "23:00", "23:30"};
        lv_snprintf(dsc->text, sizeof(dsc->text), "%s", time[dsc->value]);
    }
}

void create_demo() {
    /*Create a Tab view object*/
    lv_obj_t *tabview;
    tabview = lv_tabview_create(lv_scr_act(), LV_DIR_TOP, 45);

    /*Add 3 tabs (the tabs are page (lv_page) and can be scrolled*/
    lv_obj_t *tab1 = lv_tabview_add_tab(tabview, "Acionamento");
    lv_obj_set_style_pad_all(tab1, 12, 0);
    lv_obj_t *tab2 = lv_tabview_add_tab(tabview, "Grafico");
    lv_obj_set_style_pad_all(tab2, 0, 0);
    lv_obj_t *tab3 = lv_tabview_add_tab(tabview, "Receitas");

    lv_obj_t *screen_container = lv_obj_create(tab1);
    lv_obj_set_size(screen_container, 456, 251);
    lv_obj_set_style_bg_color(screen_container, lv_color_make(248, 248, 248), 0);
    lv_obj_set_style_border_width(screen_container, 0, 0);

    static lv_coord_t column_dsc[] = {144, 144, 144, LV_GRID_TEMPLATE_LAST};                /*2 columns with 100 and 400 ps width*/
    static lv_coord_t row_dsc[] = {0.3 * 227, 0.5 * 227, 0.2 * 227, LV_GRID_TEMPLATE_LAST}; /*3 100 px tall rows*/
    lv_obj_set_grid_dsc_array(screen_container, column_dsc, row_dsc);
    lv_obj_set_style_pad_all(screen_container, 0, 0);
    lv_obj_set_style_pad_row(screen_container, 12, 0);
    lv_obj_set_style_pad_column(screen_container, 12, 0);

    lv_obj_t *timer = timer_container(screen_container);
    lv_obj_t *temp_container_grao = temp_container(screen_container, "Temp. Grao", temp_grao);
    lv_obj_t *temp_container_ar = temp_container(screen_container, "Temp. Ar", temp_ar);

    lv_obj_set_grid_cell(timer, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
    lv_obj_set_grid_cell(temp_container_grao, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
    lv_obj_set_grid_cell(temp_container_ar, LV_GRID_ALIGN_STRETCH, 2, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
    lv_obj_set_grid_align(screen_container, LV_GRID_ALIGN_SPACE_BETWEEN, LV_GRID_ALIGN_SPACE_BETWEEN);
    lv_obj_set_layout(screen_container, LV_LAYOUT_GRID);

    lv_obj_t *arc1 = arc_container(screen_container, "Potencia", "%", POTENCIA);
    lv_obj_t *arc2 = arc_container(screen_container, "Cilindro", "RPM", CILINDRO);
    lv_obj_t *arc3 = arc_container(screen_container, "Turbina", "%", TURBINA);

    lv_obj_set_grid_cell(arc1, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 1, 1);
    lv_obj_set_grid_cell(arc2, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 1, 1);
    lv_obj_set_grid_cell(arc3, LV_GRID_ALIGN_STRETCH, 2, 1, LV_GRID_ALIGN_STRETCH, 1, 1);

    lv_obj_t *button_start = button_container(screen_container, "Iniciar");
    lv_obj_add_event_cb(button_start, start_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t *button_pause = button_container(screen_container, "Pausar");
    lv_obj_t *button_cooler = button_container(screen_container, "Resfriador");

    lv_obj_set_grid_cell(button_start, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 2, 1);
    lv_obj_set_grid_cell(button_pause, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 2, 1);
    lv_obj_set_grid_cell(button_cooler, LV_GRID_ALIGN_STRETCH, 2, 1, LV_GRID_ALIGN_STRETCH, 2, 1);

    lv_obj_t *chart_container = lv_obj_create(tab2);
    lv_obj_set_size(chart_container, 480, 275);
    lv_obj_set_style_bg_color(chart_container, lv_color_make(248, 248, 248), 0);
    lv_obj_set_style_border_width(chart_container, 0, 0);

    lv_obj_t *chart = lv_chart_create(chart_container);
    lv_obj_set_size(chart, 400, 220);
    lv_obj_center(chart);
    lv_obj_set_style_bg_color(chart, lv_color_make(248, 248, 248), 0);
    lv_obj_set_style_border_width(chart, 0, 0);
    lv_chart_set_type(chart, LV_CHART_TYPE_LINE);
    lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, 0, 300);
    lv_chart_set_range(chart, LV_CHART_AXIS_SECONDARY_Y, -100, 100);
    lv_chart_set_point_count(chart, 12);
    lv_obj_add_event_cb(chart, draw_event_cb, LV_EVENT_DRAW_PART_BEGIN, NULL);

    lv_chart_set_axis_tick(chart, LV_CHART_AXIS_PRIMARY_X, 10, 5, 12, 2, true, 40);
    lv_chart_set_axis_tick(chart, LV_CHART_AXIS_PRIMARY_Y, 10, 5, 6, 5, true, 50);
    lv_chart_set_axis_tick(chart, LV_CHART_AXIS_SECONDARY_Y, 10, 5, 6, 2, true, 50);

    // lv_chart_set_zoom_x(chart, 800);

    lv_chart_series_t *ser1 = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_RED), LV_CHART_AXIS_PRIMARY_Y);
    lv_chart_series_t *ser2 = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_GREEN), LV_CHART_AXIS_SECONDARY_Y);

    lv_chart_set_next_value(chart, ser1, 180);
    lv_chart_set_next_value(chart, ser1, 160);
    lv_chart_set_next_value(chart, ser1, 140);
    lv_chart_set_next_value(chart, ser1, 120);
    lv_chart_set_next_value(chart, ser1, 100);
    lv_chart_set_next_value(chart, ser1, 100);
    lv_chart_set_next_value(chart, ser1, 110);
    lv_chart_set_next_value(chart, ser1, 115);
    lv_chart_set_next_value(chart, ser1, 140);
    lv_chart_set_next_value(chart, ser1, 170);
    lv_chart_set_next_value(chart, ser1, 210);
    lv_chart_set_next_value(chart, ser1, 250);

    ser2->y_points[0] = 0;
    ser2->y_points[1] = -20;
    ser2->y_points[2] = -20;
    ser2->y_points[3] = -20;
    ser2->y_points[4] = -20;
    ser2->y_points[5] = 0;
    ser2->y_points[6] = 10;
    ser2->y_points[7] = 15;
    ser2->y_points[8] = 25;
    ser2->y_points[9] = 30;
    ser2->y_points[10] = 40;
    ser2->y_points[11] = 40;

    lv_chart_refresh(chart);
}

void lcd_gui_init() {
    xGuiSemaphore = xSemaphoreCreateMutex();
    lv_init();
    lvgl_driver_init();

    lv_color_t *buf1 = heap_caps_malloc(DISP_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);

    int32_t size_in_px = DISP_BUF_SIZE;

    lv_disp_draw_buf_init(&disp_buf, buf1, NULL, size_in_px);

    lv_disp_drv_init(&disp_drv);
    disp_drv.draw_buf = &disp_buf;
    disp_drv.flush_cb = disp_driver_flush;
    disp_drv.hor_res = AUSYX_HOR_RES;
    disp_drv.ver_res = AUSYX_VER_RES;
    lv_disp_drv_register(&disp_drv);

    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = touch_driver_read;
    indev = lv_indev_drv_register(&indev_drv);

    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &lv_tick_task,
        .name = "lv_tick_task",
    };

    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, pdMS_TO_TICKS(1)));

    create_demo();
}

void lcd_update_task(void *pvParameters) {
    BaseType_t xStatus;
    torrador_state_t *state;

    for (;;) {
        xStatus = xQueuePeek(state_event_queue, &state, 0);
        
        int64_t timer_in_seconds = state->timer_value / 1000000;
        int seconds = timer_in_seconds % 60;
        int minutes = (timer_in_seconds / 60) % 60;

        lv_label_set_text_fmt(timer, "%d:%d", minutes, seconds);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void lcd_gui_task(void *pvParameters) {
    torrador_controller_params_t *params = (torrador_controller_params_t *)pvParameters;
    state_event_queue = params->state_event_queue;
    control_event_queue = params->control_event_queue;  // Queue com os eventos que chegaram da IHM/WEB

    lcd_gui_init();
    xTaskCreatePinnedToCore(lcd_update_task, "lcd_update_task", 4096, NULL, 1, NULL, 1);

    for (;;) {
        if (pdTRUE == xSemaphoreTake(xGuiSemaphore, portMAX_DELAY)) {
            lv_timer_handler();
            xSemaphoreGive(xGuiSemaphore);
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

static void lv_tick_task(void *arg) {
    (void)arg;

    lv_tick_inc(LV_TICK_PERIOD_MS);
}