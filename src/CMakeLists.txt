# This file was automatically generated for projects
# without default 'CMakeLists.txt' file.

FILE(GLOB_RECURSE app_sources ${CMAKE_SOURCE_DIR}/src/*.*)

idf_component_register(SRCS ${app_sources}
                        INCLUDE_DIRS "."
                        INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/components/esp-who/components/who_detect"
                        INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/components/esp-who/components/who_cam"
                        INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/components/esp-who/components/who_frame_cap"
                        INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/components/esp-who/components/who_frame_lcd_disp"
                        REQUIRES who_cam who_detect who_frame_cap who_frame_lcd_disp esp_lvgl_port esp32_s3_eye)
