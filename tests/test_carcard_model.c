#include "carcard_model.h"
#include <assert.h>
#include <stdio.h>

int main(void)
{
    carcard_model_t m;
    uint32_t km[CARCARD_VEHICLE_COUNT] = {150000, 120000};
    carcard_init(&m, km);
    assert(m.mileage_km[CARCARD_R36] == 150000 && m.page == 0 && !m.editing);
    carcard_key(&m, CARCARD_UP);
    assert(m.vehicle == CARCARD_R36 && m.page == 1);
    carcard_key(&m, CARCARD_UP);
    assert(m.vehicle == CARCARD_R36 && m.page == 0);
    carcard_key(&m, CARCARD_DOWN);
    assert(m.vehicle == CARCARD_R36 && m.page == 1);
    carcard_key(&m, CARCARD_DOWN);
    assert(m.vehicle == CARCARD_R36 && m.page == 0);
    carcard_key(&m, CARCARD_OK);
    assert(m.vehicle == CARCARD_POLO && m.page == 0);
    carcard_key(&m, CARCARD_OK);
    assert(m.vehicle == CARCARD_R36 && m.page == 0);
    carcard_key(&m, CARCARD_UP);
    carcard_key(&m, CARCARD_OK);
    assert(m.vehicle == CARCARD_POLO && m.page == 1);
    carcard_key(&m, CARCARD_OK);
    assert(m.vehicle == CARCARD_R36 && m.page == 1);
    carcard_key(&m, CARCARD_HOLD_OK);
    assert(m.editing && m.digit == 0 && m.draft_km == 150000);
    carcard_key(&m, CARCARD_DOWN);
    assert(m.draft_km == 50000 && m.mileage_km[CARCARD_R36] == 150000);
    carcard_key(&m, CARCARD_DOWN);
    assert(m.draft_km == 950000); /* digit wrap, no arithmetic underflow */
    carcard_key(&m, CARCARD_UP);
    assert(m.draft_km == 50000);
    carcard_key(&m, CARCARD_HOLD_OK);
    assert(!m.editing && m.mileage_km[CARCARD_R36] == 150000); /* cancel */
    carcard_key(&m, CARCARD_HOLD_OK);
    for (int i = 0; i < 5; ++i) assert(carcard_key(&m, CARCARD_OK) == CARCARD_REDRAW);
    assert(m.digit == 5);
    carcard_key(&m, CARCARD_UP);
    assert(m.draft_km == 150001);
    assert(carcard_key(&m, CARCARD_OK) == CARCARD_SAVE);
    assert(m.mileage_km[CARCARD_R36] == 150000 && m.editing); /* display update waits for storage acknowledgement */
    carcard_save_complete(&m, false);
    assert(m.save_failed && m.editing && m.mileage_km[CARCARD_R36] == 150000);
    assert(carcard_key(&m, CARCARD_OK) == CARCARD_SAVE); /* retry */
    carcard_save_complete(&m, true);
    assert(!m.editing && !m.save_failed && m.mileage_km[CARCARD_R36] == 150001 && m.page == 0);
    km[0] = km[1] = 1000000;
    carcard_init(&m, km);
    assert(m.mileage_km[CARCARD_R36] == CARCARD_DEFAULT_KM && m.mileage_km[CARCARD_POLO] == CARCARD_POLO_DEFAULT_KM);
    km[0] = km[1] = CARCARD_MAX_KM;
    carcard_init(&m, km);
    carcard_key(&m, CARCARD_HOLD_OK);
    for (int i = 0; i < 6; ++i) {
        carcard_key(&m, CARCARD_UP);
        carcard_key(&m, CARCARD_OK);
    }
    assert(m.draft_km == 0); /* all digit boundaries */
    carcard_save_complete(&m, true);
    assert(m.mileage_km[CARCARD_R36] == 0);
    km[0] = km[1] = 0;
    carcard_init(&m, km); /* valid saved zero survives reboot */
    assert(m.mileage_km[CARCARD_R36] == 0);
    km[0] = 150001;
    km[1] = 120000;
    carcard_init(&m, km);
    carcard_key(&m, CARCARD_OK);
    carcard_key(&m, CARCARD_UP);
    assert(m.vehicle == CARCARD_POLO && m.page == 1);
    carcard_key(&m, CARCARD_HOLD_OK);
    assert(m.editing && m.draft_km == 120000);
    for (int i = 0; i < 5; ++i) carcard_key(&m, CARCARD_OK);
    carcard_key(&m, CARCARD_UP);
    assert(m.vehicle == CARCARD_POLO && m.draft_km == 120001);
    assert(carcard_key(&m, CARCARD_OK) == CARCARD_SAVE);
    carcard_save_complete(&m, false);
    assert(m.editing && m.save_failed && m.mileage_km[CARCARD_POLO] == 120000);
    carcard_save_complete(&m, true);
    assert(m.mileage_km[CARCARD_POLO] == 120001 && m.mileage_km[CARCARD_R36] == 150001);
    carcard_key(&m, CARCARD_OK);
    carcard_key(&m, CARCARD_HOLD_OK);
    assert(m.draft_km == 150001);
    carcard_key(&m, CARCARD_UP);
    carcard_key(&m, CARCARD_HOLD_OK);
    assert(m.mileage_km[CARCARD_R36] == 150001 && m.mileage_km[CARCARD_POLO] == 120001);
    puts("CarCard two-car navigation, independent mileage editing, cancel and save tests: PASS");
    return 0;
}
