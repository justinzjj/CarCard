#include "carcard_model.h"
#include "carcard_input.h"
#include "carcard_idle.h"
#include "carcard_store.h"
#include "carcard_ui.h"
#include "bsp_battery.h"
#include "bsp_button.h"
#include "bsp_display.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

static const char *TAG = "carcard";
static QueueHandle_t s_keys;
static carcard_model_t s_model;
static carcard_store_t s_store;
static bool s_storage_ok, s_input_ok, s_battery_ok;
static int s_battery = -1;
static const uint8_t SCREEN_BRIGHTNESS = 85;
typedef struct {
    bsp_btn_t button;
    bsp_btn_ev_t event;
} button_event_t;

/* The BSP callback only enqueues. Storage and UI work run in the app worker. */
static void on_key(bsp_btn_t button, bsp_btn_ev_t event, void *user)
{
    (void)user;
    const button_event_t input = {.button = button, .event = event};
    if (s_keys) (void)xQueueSend(s_keys, &input, 0);
}

static void refresh(void)
{
    if (bsp_lvgl_lock(1000)) {
        carcard_ui_refresh(&s_model, s_battery, s_storage_ok, s_input_ok, s_store.mileage_valid[s_model.vehicle]);
        bsp_lvgl_unlock();
    } else ESP_LOGW(TAG, "UI lock timeout");
}

static void app_worker(void *arg)
{
    (void)arg;
    carcard_idle_t idle;
    carcard_input_t input = {0};
    carcard_idle_init(&idle, (uint64_t)esp_timer_get_time());
    int64_t next_battery = esp_timer_get_time() + 10000000;
    for (;;) {
        button_event_t event;
        bool redraw = false;
        if (xQueueReceive(s_keys, &event, pdMS_TO_TICKS(250)) == pdTRUE) {
            bool wake = carcard_idle_activity(&idle, (uint64_t)esp_timer_get_time());
            if (wake) {
                bsp_display_backlight(SCREEN_BRIGHTNESS);
                ESP_LOGI(TAG, "Screen awake; wake gesture consumed");
                redraw = true;
            }
            carcard_key_t key;
            unsigned count = carcard_input_event(&input, wake, event.button, event.event, &key);
            for (unsigned i = 0; i < count; ++i) {
                if (carcard_key(&s_model, key) == CARCARD_SAVE) {
                    esp_err_t err = carcard_store_save(&s_store, s_model.vehicle, s_model.draft_km);
                    carcard_save_complete(&s_model, err == ESP_OK);
                    if (err == ESP_OK) {
                        ESP_LOGI(TAG, "Vehicle %u mileage saved: %lu km", (unsigned)s_model.vehicle,
                                 (unsigned long)s_model.mileage_km[s_model.vehicle]);
                    } else ESP_LOGE(TAG, "Mileage save failed: %s", esp_err_to_name(err));
                }
                redraw = true;
            }
        }
        int64_t now = esp_timer_get_time();
        if (s_input_ok && carcard_idle_tick(&idle, (uint64_t)now)) {
            bsp_display_backlight(0);
            ESP_LOGI(TAG, "Screen off after 60 seconds of inactivity");
        }
        if (now >= next_battery) {
            int battery = s_battery_ok ? bsp_battery_soc() : -1;
            redraw |= battery != s_battery;
            s_battery = battery;
            next_battery = now + 10000000;
        }
        if (redraw && !idle.dark) refresh();
    }
}

void app_main(void)
{
    ESP_LOGI(TAG, "CarCard R36 and Polo garage starting");
    uint32_t mileage[CARCARD_VEHICLE_COUNT];
    esp_err_t err = carcard_store_init(&s_store, mileage);
    s_storage_ok = s_store.ready;
    if (err != ESP_OK) ESP_LOGW(TAG, "Storage load unavailable or invalid: %s", esp_err_to_name(err));
    carcard_init(&s_model, mileage);
    s_battery_ok = bsp_battery_init() == ESP_OK;
    s_battery = s_battery_ok ? bsp_battery_soc() : -1;
    if (bsp_display_init() != ESP_OK || !bsp_lvgl_init()) {
        ESP_LOGE(TAG, "Display initialization failed");
        return;
    }
    bsp_display_backlight(SCREEN_BRIGHTNESS);
    s_keys = xQueueCreate(32, sizeof(button_event_t));
    s_input_ok = s_keys && bsp_button_init(on_key, NULL) == ESP_OK;
    if (!s_input_ok) ESP_LOGE(TAG, "Button input unavailable");
    if (!bsp_lvgl_lock(1000)) {
        ESP_LOGE(TAG, "Initial UI lock timeout");
        return;
    }
    bool fonts_ok = carcard_ui_verify_fonts();
    bool ui_ok = fonts_ok && carcard_ui_create();
    if (ui_ok) carcard_ui_refresh(&s_model, s_battery, s_storage_ok, s_input_ok, s_store.mileage_valid[s_model.vehicle]);
    bsp_lvgl_unlock();
    if (!ui_ok) {
        ESP_LOGE(TAG, "UI allocation or glyph coverage failed");
        return;
    }
    if (s_keys && xTaskCreate(app_worker, "carcard_worker", 4096, NULL, 4, NULL) != pdPASS) {
        s_input_ok = false;
        refresh();
        ESP_LOGE(TAG, "Application worker could not start");
    }
    ESP_LOGI(TAG, "Ready: fonts=%d storage=%d buttons=%d battery=%d heap=%lu",
             fonts_ok, s_storage_ok, s_input_ok, s_battery_ok,
             (unsigned long)esp_get_free_heap_size());
}
