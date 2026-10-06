#include "carcard_store.h"
#include "nvs_flash.h"

/* Keep the R36 key unchanged for existing installations. */
static const char *const keys[] = {"mileage_km", "polo_km"};
static const uint32_t defaults[] = {CARCARD_DEFAULT_KM, CARCARD_POLO_DEFAULT_KM};

esp_err_t carcard_store_init(carcard_store_t *store, uint32_t km[CARCARD_VEHICLE_COUNT])
{
    store->ready = false;
    for (unsigned i = 0; i < CARCARD_VEHICLE_COUNT; ++i) {
        store->mileage_valid[i] = true;
        km[i] = defaults[i];
    }
    esp_err_t err = nvs_flash_init();
    if (err != ESP_OK) return err; /* Never erase pre-existing records to recover. */
    err = nvs_open("carcard", NVS_READWRITE, &store->handle);
    if (err != ESP_OK) return err;
    store->ready = true;
    esp_err_t first_error = ESP_OK;
    for (unsigned i = 0; i < CARCARD_VEHICLE_COUNT; ++i) {
        uint32_t saved = defaults[i];
        err = nvs_get_u32(store->handle, keys[i], &saved);
        if (err == ESP_ERR_NVS_NOT_FOUND) continue;
        if (err != ESP_OK || saved > CARCARD_MAX_KM) {
            store->mileage_valid[i] = false;
            if (first_error == ESP_OK) first_error = err != ESP_OK ? err : ESP_ERR_INVALID_ARG;
        } else km[i] = saved;
    }
    return first_error;
}
esp_err_t carcard_store_save(carcard_store_t *store, carcard_vehicle_t vehicle, uint32_t km)
{
    if (!store->ready) return ESP_ERR_INVALID_STATE;
    if ((unsigned)vehicle >= CARCARD_VEHICLE_COUNT || km > CARCARD_MAX_KM) return ESP_ERR_INVALID_ARG;
    esp_err_t err = nvs_set_u32(store->handle, keys[vehicle], km);
    if (err == ESP_OK) err = nvs_commit(store->handle);
    if (err == ESP_OK) store->mileage_valid[vehicle] = true;
    return err;
}
