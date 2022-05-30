idf_component_register(SRCS "screen.c" "content.c" "lcd_gui.c" "screen_manager.c" "content_manager.c" "components.c"  "screens/menu.c" "screens/manual_mode.c" "contents/chart.c" "contents/controls.c"
                    INCLUDE_DIRS "include"
                    REQUIRES lvgl common)
