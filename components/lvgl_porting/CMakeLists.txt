file(GLOB_RECURSE SOURCE ./*.c)

idf_component_register(SRCS ${SOURCE}
                       INCLUDE_DIRS "."
                       REQUIRES lvgl lvgl_esp32_drivers
                       )
