#include "carcard_store.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static esp_err_t flash_err, open_err, get_err[CARCARD_VEHICLE_COUNT], set_err, commit_err;
static uint32_t durable_km[CARCARD_VEHICLE_COUNT];
static unsigned writes, commits;
static unsigned index_for(const char *key)
{
    if (strcmp(key, "mileage_km") == 0) return CARCARD_R36; /* legacy compatibility */
    assert(strcmp(key, "polo_km") == 0);
    return CARCARD_POLO;
}
esp_err_t nvs_flash_init(void) { return flash_err; }
esp_err_t nvs_open(const char *name, int mode, nvs_handle_t *h)
{
    assert(strcmp(name, "carcard") == 0 && mode == NVS_READWRITE);
    *h = 7;
    return open_err;
}
esp_err_t nvs_get_u32(nvs_handle_t h, const char *key, uint32_t *km)
{
    assert(h == 7);
    unsigned i = index_for(key);
    *km = durable_km[i];
    return get_err[i];
}
esp_err_t nvs_set_u32(nvs_handle_t h, const char *key, uint32_t km)
{
    assert(h == 7);
    ++writes;
    /* IDF 5.5.3 setters write immediately; commit does not provide rollback. */
    if (set_err == ESP_OK) durable_km[index_for(key)] = km;
    return set_err;
}
esp_err_t nvs_commit(nvs_handle_t h)
{
    assert(h == 7);
    ++commits;
    return commit_err;
}
int main(void)
{
    carcard_store_t s, rebooted;
    uint32_t km[CARCARD_VEHICLE_COUNT];
    get_err[0] = get_err[1] = ESP_ERR_NVS_NOT_FOUND;
    assert(carcard_store_init(&s, km) == ESP_OK && s.ready);
    assert(s.mileage_valid[0] && s.mileage_valid[1]);
    assert(km[0] == 150000 && km[1] == 120000 && writes == 0 && commits == 0);
    get_err[0] = ESP_OK;
    durable_km[0] = 123456;
    assert(carcard_store_init(&s, km) == ESP_OK && km[0] == 123456 && km[1] == 120000);
    assert(writes == 0); /* loading an old R36 never rewrites the record */
    assert(carcard_store_save(&s, CARCARD_POLO, 120001) == ESP_OK);
    get_err[1] = ESP_OK;
    assert(carcard_store_init(&rebooted, km) == ESP_OK && km[0] == 123456 && km[1] == 120001);
    assert(carcard_store_save(&s, CARCARD_R36, 150001) == ESP_OK);
    assert(carcard_store_init(&rebooted, km) == ESP_OK && km[0] == 150001 && km[1] == 120001);
    set_err = ESP_FAIL;
    unsigned old_commits = commits;
    assert(carcard_store_save(&s, CARCARD_POLO, 42) == ESP_FAIL && commits == old_commits);
    assert(durable_km[0] == 150001 && durable_km[1] == 120001);
    set_err = ESP_OK;
    commit_err = ESP_FAIL;
    assert(carcard_store_save(&s, CARCARD_POLO, 42) == ESP_FAIL && durable_km[1] == 42);
    assert(carcard_store_init(&rebooted, km) == ESP_OK && km[1] == 42);
    commit_err = ESP_OK;
    assert(carcard_store_save(&s, CARCARD_POLO, 0) == ESP_OK);
    assert(carcard_store_init(&rebooted, km) == ESP_OK && km[1] == 0 && km[0] == 150001);
    durable_km[0] = 1000000;
    assert(carcard_store_init(&s, km) == ESP_ERR_INVALID_ARG && s.ready);
    assert(!s.mileage_valid[0] && s.mileage_valid[1] && km[0] == 150000 && km[1] == 0);
    assert(carcard_store_save(&s, CARCARD_R36, 456789) == ESP_OK && s.mileage_valid[0]);
    get_err[1] = ESP_FAIL;
    assert(carcard_store_init(&s, km) == ESP_FAIL && s.ready && s.mileage_valid[0] && !s.mileage_valid[1]);
    assert(km[0] == 456789 && km[1] == 120000);
    get_err[0] = ESP_FAIL;
    get_err[1] = ESP_OK;
    assert(carcard_store_init(&s, km) == ESP_FAIL && !s.mileage_valid[0] && s.mileage_valid[1]);
    assert(km[1] == 0); /* one failed read must not skip the other vehicle */
    get_err[0] = ESP_OK;
    unsigned old_writes = writes;
    assert(carcard_store_save(&s, CARCARD_POLO, 1000000) == ESP_ERR_INVALID_ARG && writes == old_writes);
    assert(carcard_store_save(&s, (carcard_vehicle_t)-1, 0) == ESP_ERR_INVALID_ARG);
    assert(carcard_store_save(&s, CARCARD_VEHICLE_COUNT, 0) == ESP_ERR_INVALID_ARG && writes == old_writes);
    open_err = ESP_FAIL;
    assert(carcard_store_init(&s, km) == ESP_FAIL && !s.ready && km[0] == 150000 && km[1] == 120000);
    assert(carcard_store_save(&s, CARCARD_R36, 0) == ESP_ERR_INVALID_STATE && writes == old_writes);
    open_err = ESP_OK;
    flash_err = ESP_FAIL;
    assert(carcard_store_init(&s, km) == ESP_FAIL && !s.ready);
    assert(carcard_store_save(&s, CARCARD_POLO, 0) == ESP_ERR_INVALID_STATE);
    puts("NVS legacy R36 compatibility, independent vehicle records and error/restart tests: PASS");
    return 0;
}
