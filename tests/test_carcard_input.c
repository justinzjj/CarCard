#include "carcard_input.h"
#include <assert.h>
#include <stdio.h>

static void apply(carcard_model_t *m, bsp_btn_t b, bsp_btn_ev_t ev)
{
    carcard_key_t key;
    unsigned count = carcard_translate_key(b, ev, &key);
    for (unsigned i = 0; i < count; ++i) carcard_key(m, key);
}

int main(void)
{
    carcard_key_t key;
    assert(carcard_translate_key(BSP_BTN_UP, BSP_BTN_PRESS, &key) == 1 && key == CARCARD_UP);
    assert(carcard_translate_key(BSP_BTN_UP, BSP_BTN_CLICK, &key) == 0);
    assert(carcard_translate_key(BSP_BTN_UP, BSP_BTN_DOUBLE, &key) == 0);
    assert(carcard_translate_key(BSP_BTN_DOWN, BSP_BTN_PRESS, &key) == 1 && key == CARCARD_DOWN);
    assert(carcard_translate_key(BSP_BTN_OK, BSP_BTN_PRESS, &key) == 0);
    assert(carcard_translate_key(BSP_BTN_OK, BSP_BTN_CLICK, &key) == 1 && key == CARCARD_OK);
    assert(carcard_translate_key(BSP_BTN_OK, BSP_BTN_DOUBLE, &key) == 2 && key == CARCARD_OK);
    assert(carcard_translate_key(BSP_BTN_OK, BSP_BTN_LONG, &key) == 1 && key == CARCARD_HOLD_OK);
    assert(carcard_translate_key((bsp_btn_t)99, BSP_BTN_PRESS, &key) == 0);
    carcard_model_t m;
    const uint32_t km[] = {150000, 120000};
    carcard_init(&m, km);
    apply(&m, BSP_BTN_OK, BSP_BTN_PRESS);
    apply(&m, BSP_BTN_OK, BSP_BTN_LONG);
    assert(m.editing && m.digit == 0);
    apply(&m, BSP_BTN_UP, BSP_BTN_PRESS);
    apply(&m, BSP_BTN_UP, BSP_BTN_PRESS);
    apply(&m, BSP_BTN_UP, BSP_BTN_DOUBLE);
    assert(m.draft_km == 350000);
    apply(&m, BSP_BTN_DOWN, BSP_BTN_PRESS);
    apply(&m, BSP_BTN_DOWN, BSP_BTN_PRESS);
    apply(&m, BSP_BTN_DOWN, BSP_BTN_DOUBLE);
    assert(m.draft_km == 150000);
    apply(&m, BSP_BTN_OK, BSP_BTN_PRESS);
    apply(&m, BSP_BTN_OK, BSP_BTN_PRESS);
    apply(&m, BSP_BTN_OK, BSP_BTN_DOUBLE);
    assert(m.editing && m.digit == 2);
    apply(&m, BSP_BTN_OK, BSP_BTN_PRESS);
    apply(&m, BSP_BTN_OK, BSP_BTN_LONG);
    assert(!m.editing && m.mileage_km[CARCARD_R36] == 150000);
    apply(&m, BSP_BTN_UP, BSP_BTN_PRESS);
    apply(&m, BSP_BTN_UP, BSP_BTN_CLICK);
    assert(m.vehicle == CARCARD_R36 && m.page == 1);
    apply(&m, BSP_BTN_DOWN, BSP_BTN_PRESS);
    assert(m.vehicle == CARCARD_R36 && m.page == 0);
    apply(&m, BSP_BTN_OK, BSP_BTN_CLICK);
    assert(m.vehicle == CARCARD_POLO && m.page == 0);
    apply(&m, BSP_BTN_OK, BSP_BTN_DOUBLE);
    assert(m.vehicle == CARCARD_POLO && m.page == 0);
    puts("BSP press/click/double/long translation and rapid-tap regression tests: PASS");
    return 0;
}
