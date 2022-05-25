idf_component_register(SRCS "main.c"
                    INCLUDE_DIRS ".")

idf_build_set_property(C_COMPILE_OPTIONS "-DSQLITE_THREADSAFE=0 -DSQLITE_MUTEX_APPDEF=1" APPEND)
idf_build_get_property(compile C_COMPILE_OPTIONS)
message(STATUS "The Python intepreter is: ${compile}")


# spiffs_create_partition_image(spiffs ../website/build FLASH_IN_PROJECT)