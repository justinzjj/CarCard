#pragma once
#include "bsp_button.h"
#include "carcard_model.h"

typedef struct {
    bool swallow_ok;
} carcard_input_t;

/* Return the number of logical actions. No I/O, allocation or blocking. */
unsigned carcard_translate_key(bsp_btn_t button, bsp_btn_ev_t event, carcard_key_t *key);

/* Consume the wake gesture, including delayed OK click/double/long events. */
unsigned carcard_input_event(carcard_input_t *input, bool waking,
                            bsp_btn_t button, bsp_btn_ev_t event, carcard_key_t *key);
