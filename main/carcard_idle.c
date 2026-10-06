#include "carcard_idle.h"

void carcard_idle_init(carcard_idle_t *idle, uint64_t now_us)
{
    idle->last_activity_us = now_us;
    idle->dark = false;
}

bool carcard_idle_tick(carcard_idle_t *idle, uint64_t now_us)
{
    if (idle->dark || now_us < idle->last_activity_us ||
        now_us - idle->last_activity_us < CARCARD_IDLE_US) return false;
    idle->dark = true;
    return true;
}

bool carcard_idle_activity(carcard_idle_t *idle, uint64_t now_us)
{
    bool wake = idle->dark;
    idle->last_activity_us = now_us;
    idle->dark = false;
    return wake;
}
