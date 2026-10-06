#include "carcard_input.h"
unsigned carcard_input_event(carcard_input_t *input, bool waking,
                            bsp_btn_t button, bsp_btn_ev_t event, carcard_key_t *key)
{
    if (waking) {
        input->swallow_ok = button == BSP_BTN_OK && event == BSP_BTN_PRESS;
        return 0;
    }
    if (input->swallow_ok && button == BSP_BTN_OK) {
        if (event == BSP_BTN_CLICK || event == BSP_BTN_DOUBLE ||
            event == BSP_BTN_LONG || event == BSP_BTN_END)
            input->swallow_ok = false;
        return 0;
    }
    return carcard_translate_key(button, event, key);
}

unsigned carcard_translate_key(bsp_btn_t button, bsp_btn_ev_t event, carcard_key_t *key)
{
    if (button == BSP_BTN_UP || button == BSP_BTN_DOWN) {
        if (event != BSP_BTN_PRESS) return 0;
        *key = button == BSP_BTN_UP ? CARCARD_UP : CARCARD_DOWN;
        return 1;
    }
    if (button == BSP_BTN_OK) {
        if (event == BSP_BTN_LONG) {
            *key = CARCARD_HOLD_OK;
            return 1;
        }
        if (event == BSP_BTN_CLICK || event == BSP_BTN_DOUBLE) {
            *key = CARCARD_OK;
            return event == BSP_BTN_DOUBLE ? 2 : 1;
        }
    }
    return 0;
}
