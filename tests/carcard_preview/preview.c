#include "carcard_ui.h"
#include "lvgl.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

LV_FONT_DECLARE(carcard_font_14);
static uint16_t framebuffer[240 * 320];
static uint16_t draw_buffer[240 * 40];

static void flush(lv_display_t *display, const lv_area_t *area, uint8_t *pixels)
{
    unsigned width = (unsigned)(area->x2 - area->x1 + 1);
    for (int y = area->y1; y <= area->y2; ++y) {
        memcpy(framebuffer + y * 240 + area->x1,
               pixels + (y - area->y1) * width * 2, width * 2);
    }
    lv_display_flush_ready(display);
}

static uint32_t next_codepoint(const unsigned char **p)
{
    uint32_t c = *(*p)++;
    if (c < 128) return c;
    unsigned remaining = c < 0xE0 ? 1 : c < 0xF0 ? 2 : 3;
    c &= remaining == 1 ? 0x1F : remaining == 2 ? 0x0F : 7;
    while (remaining--) c = (c << 6) | (*(*p)++ & 0x3F);
    return c;
}

static void audit(lv_obj_t *parent)
{
    for (uint32_t i = 0; i < lv_obj_get_child_count(parent); ++i) {
        lv_obj_t *obj = lv_obj_get_child(parent, (int32_t)i);
        if (lv_obj_check_type(obj, &lv_label_class)) {
            const lv_font_t *font = lv_obj_get_style_text_font(obj, LV_PART_MAIN);
            const char *text = lv_label_get_text(obj);
            const unsigned char *p = (const unsigned char *)text;
            while (*p) {
                uint32_t c = next_codepoint(&p);
                lv_font_glyph_dsc_t glyph = {0};
                assert(lv_font_get_glyph_dsc(font, &glyph, c, 0) && !glyph.is_placeholder);
            }
            lv_point_t size;
            lv_text_get_size(&size, text, font, 0, 0, LV_COORD_MAX, LV_TEXT_FLAG_NONE);
            if (size.x > lv_obj_get_width(obj)) {
                fprintf(stderr, "Clipped text width: %s (%ld > %ld)\n", text,
                        (long)size.x, (long)lv_obj_get_width(obj));
                exit(1);
            }
            lv_area_t area;
            lv_obj_get_coords(obj, &area);
            if (area.x1 < 0 || area.y1 < 0 || area.x2 >= 240 || area.y2 >= 320) {
                fprintf(stderr, "Text outside display: %s at %ld,%ld..%ld,%ld\n", text,
                        (long)area.x1, (long)area.y1, (long)area.x2, (long)area.y2);
                exit(1);
            }
        }
        audit(obj);
    }
}

static void render(const char *path, carcard_model_t *model, int battery,
                   bool storage, bool input, bool valid)
{
    carcard_ui_refresh(model, battery, storage, input, valid);
    lv_obj_update_layout(lv_screen_active());
    audit(lv_screen_active());
    lv_refr_now(NULL);
    FILE *file = fopen(path, "wb");
    assert(file);
    fprintf(file, "P6\n240 320\n255\n");
    for (unsigned i = 0; i < 240 * 320; ++i) {
        uint16_t c = framebuffer[i];
        unsigned char rgb[] = {(unsigned char)((c >> 11) * 255 / 31),
                               (unsigned char)(((c >> 5) & 63) * 255 / 63),
                               (unsigned char)((c & 31) * 255 / 31)};
        assert(fwrite(rgb, 1, 3, file) == 3);
    }
    fclose(file);
}

int main(void)
{
    lv_init();
    lv_display_t *display = lv_display_create(240, 320);
    assert(display);
    lv_display_set_color_format(display, LV_COLOR_FORMAT_RGB565);
    lv_display_set_buffers(display, draw_buffer, NULL, sizeof(draw_buffer), LV_DISPLAY_RENDER_MODE_PARTIAL);
    lv_display_set_flush_cb(display, flush);
    assert(carcard_ui_verify_fonts());
    lv_font_glyph_dsc_t missing = {0};
    assert(!lv_font_get_glyph_dsc(&carcard_font_14, &missing, 0x9F98, 0) || missing.is_placeholder);
    assert(carcard_ui_create());
    carcard_model_t m;
    const uint32_t km[] = {150000, 120000};
    carcard_init(&m, km);
    render("home.ppm", &m, 100, true, true, true);
    carcard_key(&m, CARCARD_DOWN);
    render("specs.ppm", &m, 85, true, true, true);
    carcard_key(&m, CARCARD_OK);
    assert(m.vehicle == CARCARD_POLO && m.page == 1);
    render("polo-specs.ppm", &m, 100, true, true, true);
    carcard_key(&m, CARCARD_UP);
    render("polo-home.ppm", &m, 100, true, true, true);
    carcard_key(&m, CARCARD_HOLD_OK);
    render("polo-editor.ppm", &m, 100, true, true, true);
    carcard_key(&m, CARCARD_HOLD_OK);
    carcard_key(&m, CARCARD_OK);
    carcard_key(&m, CARCARD_HOLD_OK);
    render("editor.ppm", &m, -1, true, true, true);
    m.digit = 5;
    m.save_failed = true;
    render("save-failure.ppm", &m, 0, true, true, true);
    carcard_key(&m, CARCARD_HOLD_OK);
    render("storage-failure.ppm", &m, -1, false, true, true);
    render("input-failure.ppm", &m, -1, true, false, true);
    render("invalid-mileage.ppm", &m, -1, true, true, false);
    carcard_ui_refresh(&m, 85, true, true, true);
    lv_obj_update_layout(lv_screen_active());
    lv_refr_now(NULL);
    lv_mem_monitor_t before, after;
    lv_mem_monitor(&before);
    for (int i = 0; i < 300; ++i) {
        carcard_key(&m, CARCARD_UP);
        if (i % 2 == 0) carcard_key(&m, CARCARD_OK);
        carcard_ui_refresh(&m, 85, true, true, true);
        lv_obj_update_layout(lv_screen_active());
        audit(lv_screen_active());
        lv_refr_now(NULL);
        /* TLSF split-block metadata changes in early allocations. Compare two
         * settled checkpoints of the same screen, after 100 warmup transitions. */
        if (i == 99) lv_mem_monitor(&before);
    }
    lv_mem_monitor(&after);
    assert(after.free_size >= before.free_size);
    assert(after.free_biggest_size >= 2048);
    printf("LVGL rendered pages, active-font coverage, negative glyph, text bounds and 300 page/vehicle transitions: PASS\n");
    printf("LVGL 24 KB pool: free=%lu largest=%lu max_used=%lu bytes\n",
           (unsigned long)after.free_size, (unsigned long)after.free_biggest_size,
           (unsigned long)after.max_used);
    lv_deinit();
    return 0;
}
