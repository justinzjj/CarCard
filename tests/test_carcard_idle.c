#include "carcard_idle.h"
#include "carcard_input.h"
#include <assert.h>
#include <stdio.h>

static unsigned event(carcard_idle_t *idle, carcard_input_t *input,
                      bsp_btn_t button, bsp_btn_ev_t ev, uint64_t now,
                      carcard_model_t *model)
{
    carcard_key_t key;
    bool wake = carcard_idle_activity(idle, now);
    unsigned count = carcard_input_event(input, wake, button, ev, &key);
    for (unsigned i = 0; i < count; ++i) carcard_key(model, key);
    return count;
}

int main(void)
{
    carcard_idle_t idle;
    carcard_input_t input = {0};
    carcard_model_t m;
    const uint32_t mileage[] = {150000, 120000};
    carcard_init(&m, mileage);
    carcard_idle_init(&idle, 0);
    assert(!carcard_idle_tick(&idle, 59999999) && !idle.dark);
    assert(carcard_idle_tick(&idle, 60000000) && idle.dark);
    assert(!carcard_idle_tick(&idle, 120000000)); /* one transition only */
    assert(event(&idle, &input, BSP_BTN_UP, BSP_BTN_PRESS, 120000001, &m) == 0);
    assert(!idle.dark && m.vehicle == CARCARD_R36);
    event(&idle, &input, BSP_BTN_UP, BSP_BTN_CLICK, 120100000, &m);
    assert(m.vehicle == CARCARD_R36);
    assert(event(&idle, &input, BSP_BTN_UP, BSP_BTN_PRESS, 121000000, &m) == 1);
    assert(m.vehicle == CARCARD_R36 && m.page == 1);
    assert(!carcard_idle_tick(&idle, 180999999));
    assert(carcard_idle_tick(&idle, 181000000));
    event(&idle, &input, BSP_BTN_OK, BSP_BTN_PRESS, 182000000, &m);
    assert(!idle.dark && m.vehicle == CARCARD_R36 && m.page == 1);
    assert(event(&idle, &input, BSP_BTN_OK, BSP_BTN_CLICK, 182200000, &m) == 0);
    assert(m.vehicle == CARCARD_R36 && m.page == 1);
    event(&idle, &input, BSP_BTN_OK, BSP_BTN_PRESS, 183000000, &m);
    assert(event(&idle, &input, BSP_BTN_OK, BSP_BTN_CLICK, 183200000, &m) == 1);
    assert(m.vehicle == CARCARD_POLO && m.page == 1);
    assert(carcard_idle_tick(&idle, 243200000));
    event(&idle, &input, BSP_BTN_OK, BSP_BTN_PRESS, 244000000, &m);
    event(&idle, &input, BSP_BTN_OK, BSP_BTN_PRESS, 244100000, &m);
    assert(event(&idle, &input, BSP_BTN_OK, BSP_BTN_DOUBLE, 244300000, &m) == 0);
    assert(m.page == 1); /* entire wake gesture is consumed */
    assert(carcard_idle_tick(&idle, 304300000));
    event(&idle, &input, BSP_BTN_OK, BSP_BTN_PRESS, 305000000, &m);
    event(&idle, &input, BSP_BTN_OK, BSP_BTN_LONG, 306000000, &m);
    assert(!m.editing && m.page == 1);
    event(&idle, &input, BSP_BTN_OK, BSP_BTN_PRESS, 307000000, &m);
    event(&idle, &input, BSP_BTN_OK, BSP_BTN_LONG, 308000000, &m);
    assert(m.editing && m.draft_km == 120000);
    event(&idle, &input, BSP_BTN_UP, BSP_BTN_PRESS, 309000000, &m);
    assert(m.draft_km == 220000 && m.digit == 0);
    assert(carcard_idle_tick(&idle, 369000000));
    event(&idle, &input, BSP_BTN_DOWN, BSP_BTN_PRESS, 370000000, &m);
    assert(!idle.dark && m.editing && m.draft_km == 220000 && m.digit == 0);
    assert(m.mileage_km[CARCARD_POLO] == 120000);
    event(&idle, &input, BSP_BTN_DOWN, BSP_BTN_PRESS, 371000000, &m);
    assert(m.draft_km == 120000);
    const uint64_t large = UINT64_C(5000000000000);
    carcard_idle_init(&idle, large);
    assert(!carcard_idle_tick(&idle, large - 1));
    assert(!carcard_idle_tick(&idle, large + CARCARD_IDLE_US - 1));
    assert(carcard_idle_tick(&idle, large + CARCARD_IDLE_US));
    assert(carcard_idle_activity(&idle, large + CARCARD_IDLE_US + 1));
    assert(!carcard_idle_activity(&idle, large + CARCARD_IDLE_US + 2));
    /* Three rapid OK taps have no CLICK/DOUBLE event; END must release suppression. */
    carcard_init(&m, mileage);
    carcard_idle_init(&idle, 0);
    assert(carcard_idle_tick(&idle, 60000000));
    for (int i = 0; i < 3; ++i)
        event(&idle, &input, BSP_BTN_OK, BSP_BTN_PRESS, 61000000 + (uint64_t)i * 50000, &m);
    event(&idle, &input, BSP_BTN_OK, BSP_BTN_END, 61400000, &m);
    assert(m.page == 0 && !m.editing);
    event(&idle, &input, BSP_BTN_OK, BSP_BTN_PRESS, 62000000, &m);
    assert(event(&idle, &input, BSP_BTN_OK, BSP_BTN_CLICK, 62300000, &m) == 1);
    assert(m.vehicle == CARCARD_POLO && m.page == 0);
    event(&idle, &input, BSP_BTN_OK, BSP_BTN_END, 62300001, &m);
    assert(carcard_idle_tick(&idle, 122300001));
    for (int i = 0; i < 3; ++i)
        event(&idle, &input, BSP_BTN_OK, BSP_BTN_PRESS, 123000000 + (uint64_t)i * 50000, &m);
    event(&idle, &input, BSP_BTN_OK, BSP_BTN_END, 123400000, &m);
    event(&idle, &input, BSP_BTN_OK, BSP_BTN_PRESS, 124000000, &m);
    event(&idle, &input, BSP_BTN_OK, BSP_BTN_LONG, 124500000, &m);
    assert(m.editing && m.draft_km == 120000);
    puts("60-second boundary, activity reset, wake gesture suppression and draft preservation: PASS");
    return 0;
}
