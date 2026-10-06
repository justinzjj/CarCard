#pragma once
#include "nvs.h"
#include "carcard_model.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct {
    nvs_handle_t handle;
    bool ready;
    bool mileage_valid[CARCARD_VEHICLE_COUNT];
} carcard_store_t;

/* Single app-worker owner. These calls may block; never call from button/LVGL callbacks. */
esp_err_t carcard_store_init(carcard_store_t *store, uint32_t mileage_km[CARCARD_VEHICLE_COUNT]);
esp_err_t carcard_store_save(carcard_store_t *store, carcard_vehicle_t vehicle, uint32_t mileage_km);
