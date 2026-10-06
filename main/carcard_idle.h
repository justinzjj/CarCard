#pragma once
#include <stdbool.h>
#include <stdint.h>

#define CARCARD_IDLE_US UINT64_C(60000000)
typedef struct {
    uint64_t last_activity_us;
    bool dark;
} carcard_idle_t;

/* Caller supplies monotonic microseconds. Returns true only on a transition. */
void carcard_idle_init(carcard_idle_t *idle, uint64_t now_us);
bool carcard_idle_tick(carcard_idle_t *idle, uint64_t now_us);
bool carcard_idle_activity(carcard_idle_t *idle, uint64_t now_us);
