cmake_minimum_required(VERSION 3.16)
project(carcard_preview C)
get_filename_component(CARCARD_ROOT "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
set(LV_BUILD_CONF_PATH "${CMAKE_CURRENT_LIST_DIR}/lv_conf.h" CACHE PATH "" FORCE)
set(CONFIG_LV_BUILD_DEMOS OFF CACHE BOOL "" FORCE)
set(CONFIG_LV_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(CONFIG_LV_USE_THORVG_INTERNAL OFF CACHE BOOL "" FORCE)
add_subdirectory("${CARCARD_ROOT}/managed_components/lvgl__lvgl" lvgl)
add_executable(carcard_preview preview.c
    "${CARCARD_ROOT}/main/carcard_model.c"
    "${CARCARD_ROOT}/main/carcard_ui.c"
    "${CARCARD_ROOT}/assets/fonts/carcard_font_14.c"
    "${CARCARD_ROOT}/assets/fonts/carcard_font_16.c"
    "${CARCARD_ROOT}/assets/fonts/carcard_font_20.c"
    "${CARCARD_ROOT}/assets/images/carcard_r36.c"
    "${CARCARD_ROOT}/assets/images/carcard_polo.c")
target_include_directories(carcard_preview PRIVATE
    "${CARCARD_ROOT}/main" "${CARCARD_ROOT}/assets/fonts")
target_link_libraries(carcard_preview PRIVATE lvgl::lvgl m)
target_compile_options(carcard_preview PRIVATE -Wall -Wextra -Werror)
