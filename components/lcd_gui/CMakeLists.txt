idf_component_register(SRCS "lcd_gui.c" "screen_manager.c" "components.c"  "screens/menu.c" "screens/manual_mode.c" 
                    INCLUDE_DIRS "include"
                    REQUIRES lvgl common)
