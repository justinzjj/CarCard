#pragma once
#include "carcard_model.h"
#include <stdbool.h>

/* Called only by the LVGL owner, or while holding the BSP LVGL lock. */
bool carcard_ui_create(void);
bool carcard_ui_verify_fonts(void);
void carcard_ui_refresh(const carcard_model_t *model, int battery_percent,
                        bool storage_ok, bool input_ok, bool mileage_valid);
