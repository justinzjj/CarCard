#pragma once
#include <stdbool.h>
#include <stdint.h>

#define CARCARD_DEFAULT_KM 150000u
#define CARCARD_POLO_DEFAULT_KM 120000u
#define CARCARD_MAX_KM 999999u
typedef enum { CARCARD_R36, CARCARD_POLO, CARCARD_VEHICLE_COUNT } carcard_vehicle_t;
/* This release has only the owner's R36 and Polo; adding vehicles is not exposed.
 * A common-vehicle information catalog is planned for the next version. */
typedef enum { CARCARD_UP, CARCARD_DOWN, CARCARD_OK, CARCARD_HOLD_OK } carcard_key_t;
typedef enum { CARCARD_REDRAW, CARCARD_SAVE } carcard_action_t;
typedef struct {
    uint32_t mileage_km[CARCARD_VEHICLE_COUNT];
    uint32_t draft_km;
    carcard_vehicle_t vehicle;
    uint8_t page;
    uint8_t digit;
    bool editing;
    bool save_failed;
} carcard_model_t;

void carcard_init(carcard_model_t *model, const uint32_t mileage_km[CARCARD_VEHICLE_COUNT]);
carcard_action_t carcard_key(carcard_model_t *model, carcard_key_t key);
void carcard_save_complete(carcard_model_t *model, bool success);
