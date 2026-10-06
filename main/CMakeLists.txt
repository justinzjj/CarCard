idf_component_register(
    SRCS "main.c"
         "carcard_model.c"
         "carcard_input.c"
         "carcard_idle.c"
         "carcard_store.c"
         "carcard_ui.c"
         "../assets/fonts/carcard_font_14.c"
         "../assets/fonts/carcard_font_16.c"
         "../assets/fonts/carcard_font_20.c"
         "../assets/images/carcard_r36.c"
         "../assets/images/carcard_polo.c"
    INCLUDE_DIRS "." "../assets/fonts"
    REQUIRES bsp esp_timer nvs_flash
)
