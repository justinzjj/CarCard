#include "carcard_model.h"
#include <string.h>
void carcard_init(carcard_model_t *m, const uint32_t km[CARCARD_VEHICLE_COUNT])
{
    static const uint32_t defaults[] = {CARCARD_DEFAULT_KM, CARCARD_POLO_DEFAULT_KM};
    memset(m, 0, sizeof(*m));
    for (unsigned i = 0; i < CARCARD_VEHICLE_COUNT; ++i)
        m->mileage_km[i] = km[i] <= CARCARD_MAX_KM ? km[i] : defaults[i];
}

carcard_action_t carcard_key(carcard_model_t *m, carcard_key_t key)
{
    static const uint32_t place[] = {100000, 10000, 1000, 100, 10, 1};
    if (key == CARCARD_HOLD_OK) {
        if (m->editing) {
            m->editing = false;
        } else {
            m->editing = true;
            m->draft_km = m->mileage_km[m->vehicle];
            m->digit = 0;
        }
        m->save_failed = false;
    } else if (!m->editing) {
        if (key == CARCARD_OK)
            m->vehicle = (carcard_vehicle_t)((m->vehicle + 1) % CARCARD_VEHICLE_COUNT);
        else m->page = (uint8_t)((m->page + 1) % 2);
    } else if (key == CARCARD_OK) {
        if (m->digit == 5) return CARCARD_SAVE;
        ++m->digit;
        m->save_failed = false;
    } else {
        const uint32_t p = place[m->digit];
        const uint32_t old = m->draft_km / p % 10;
        const uint32_t next = (old + (key == CARCARD_UP ? 1 : 9)) % 10;
        m->draft_km = m->draft_km - old * p + next * p;
        m->save_failed = false;
    }
    return CARCARD_REDRAW;
}

void carcard_save_complete(carcard_model_t *m, bool success)
{
    if (!m->editing) return;
    m->save_failed = !success;
    if (success) {
        m->mileage_km[m->vehicle] = m->draft_km;
        m->editing = false;
        m->page = 0;
    }
}
