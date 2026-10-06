#include "carcard_ui.h"
#include "carcard_config.h"
#include "lvgl.h"
#include "carcard_glyphs.h"
#include <stdio.h>

LV_FONT_DECLARE(carcard_font_14);
LV_FONT_DECLARE(carcard_font_16);
LV_FONT_DECLARE(carcard_font_20);
LV_IMAGE_DECLARE(carcard_r36);
LV_IMAGE_DECLARE(carcard_polo);

#define BG 0x101A2B
#define PANEL 0x1B2A40
#define INK 0xEAF0F5
#define DIM 0xA1B3C9
#define ACCENT 0x89DCC9
#define WARN 0xFFB57F

static lv_obj_t *s_screen;

static lv_obj_t *label(const char *text, int x, int y, int width,
                       const lv_font_t *font, uint32_t color)
{
    lv_obj_t *obj = lv_label_create(s_screen);
    lv_obj_set_pos(obj, x, y);
    lv_obj_set_width(obj, width);
    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
    lv_obj_set_style_text_font(obj, font, 0);
    lv_obj_set_style_text_color(obj, lv_color_hex(color), 0);
    lv_label_set_text(obj, text);
    return obj;
}

static void block(int x, int y, int w, int h, uint32_t color)
{
    lv_obj_t *obj = lv_obj_create(s_screen);
    lv_obj_remove_style_all(obj);
    lv_obj_set_pos(obj, x, y);
    lv_obj_set_size(obj, w, h);
    lv_obj_set_style_bg_color(obj, lv_color_hex(color), 0);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
}

bool carcard_ui_verify_fonts(void)
{
    const lv_font_t *fonts[] = {&carcard_font_14, &carcard_font_16, &carcard_font_20};
    for (unsigned f = 0; f < sizeof(fonts) / sizeof(fonts[0]); ++f) {
        for (unsigned i = 0; i < sizeof(carcard_glyphs) / sizeof(carcard_glyphs[0]); ++i) {
            lv_font_glyph_dsc_t glyph = {0};
            if (!lv_font_get_glyph_dsc(fonts[f], &glyph, carcard_glyphs[i], 0)
                || glyph.is_placeholder) return false;
        }
    }
    return true;
}

bool carcard_ui_create(void)
{
    s_screen = lv_obj_create(NULL);
    if (!s_screen) return false;
    lv_obj_remove_style_all(s_screen);
    lv_obj_set_style_bg_color(s_screen, lv_color_hex(BG), 0);
    lv_obj_set_style_bg_opa(s_screen, LV_OPA_COVER, 0);
    lv_obj_remove_flag(s_screen, LV_OBJ_FLAG_SCROLLABLE);
    lv_screen_load(s_screen);
    return true;
}

static void home(const carcard_model_t *m)
{
    label(m->vehicle == CARCARD_R36 ? "R36" : "POLO", 12, 31, 210, &lv_font_montserrat_40, INK);
    label(m->vehicle == CARCARD_R36 ? "大众 · 银色旅行版" : "大众 · 银色四门两厢", 14, 80, 212, &carcard_font_16, DIM);
    block(54, 103, 132, 22, 0x285A92);
    lv_obj_t *plate = label(m->vehicle == CARCARD_R36 ? CARCARD_R36_PLATE : CARCARD_POLO_PLATE,
                            54, 104, 132, &carcard_font_16, INK);
    lv_obj_set_style_text_align(plate, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_t *image = lv_image_create(s_screen);
    lv_image_set_src(image, m->vehicle == CARCARD_R36 ? &carcard_r36 : &carcard_polo);
    lv_obj_set_pos(image, 12, 126);
    block(14, 227, 212, 51, PANEL);
    label("总里程", 22, 227, 72, &carcard_font_14, DIM);
    label("长按确认编辑", 120, 227, 98, &carcard_font_14, DIM);
    char km[24];
    const uint32_t mileage = m->mileage_km[m->vehicle];
    if (mileage >= 1000) {
        snprintf(km, sizeof(km), "%lu,%03lu", (unsigned long)(mileage / 1000),
                 (unsigned long)(mileage % 1000));
    } else {
        snprintf(km, sizeof(km), "%lu", (unsigned long)mileage);
    }
    label(km, 22, 249, 145, &lv_font_montserrat_20, INK);
    label("km", 185, 249, 34, &carcard_font_16, ACCENT);
}

static void specs(const carcard_model_t *m)
{
    label("原厂参数", 14, 39, 180, &carcard_font_20, INK);
    label(m->vehicle == CARCARD_R36 ? "PASSAT R36 / VARIANT" : "POLO · 2008 / 9N3",
          14, 68, 212, &carcard_font_14, DIM);
    const char *names[] = {"排量", "发动机", "马力", "功率", "变速箱", "驱动"};
    const char *values[CARCARD_VEHICLE_COUNT][6] = {
        {"3.6 L", "VR6 FSI", "300 PS", "220 kW", "6 速 DSG", "4MOTION 四驱"},
        {"1.4 L", "EA113 L4", "86 PS", "63 kW", "5 速手动", "前轮驱动"},
    };
    for (unsigned i = 0; i < 6; ++i) {
        const int y = 96 + (int)i * 30;
        label(names[i], 14, y + 2, 56, &carcard_font_14, DIM);
        label(values[m->vehicle][i], 88, y, 138, &carcard_font_16, INK);
        if (i < 5) block(14, y + 26, 212, 1, PANEL);
    }
}

static void editor(const carcard_model_t *m)
{
    label("更新总里程", 14, 42, 210, &carcard_font_20, INK);
    label(m->vehicle == CARCARD_R36 ? "R36 · 手动记录 km" : "POLO · 手动记录 km",
          14, 79, 212, &carcard_font_16, DIM);
    char digits[8];
    snprintf(digits, sizeof(digits), "%06lu", (unsigned long)m->draft_km);
    for (unsigned i = 0; i < 6; ++i) {
        int x = 14 + (int)i * 36;
        block(x, 122, 32, 58, i == m->digit ? ACCENT : PANEL);
        char digit[2] = {digits[i], '\0'};
        lv_obj_t *obj = label(digit, x, 136, 32, &lv_font_montserrat_28,
                              i == m->digit ? BG : INK);
        lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, 0);
    }
    label("上键加一  下键减一", 14, 200, 212, &carcard_font_16, DIM);
    label(m->digit == 5 ? "确认保存  长按取消" : "确认下一位  长按取消",
          14, 230, 212, &carcard_font_16, INK);
    if (m->save_failed) label("保存失败，请重试", 14, 267, 212, &carcard_font_16, WARN);
}

void carcard_ui_refresh(const carcard_model_t *m, int battery,
                        bool storage_ok, bool input_ok, bool mileage_valid)
{
    if (!s_screen) return;
    lv_obj_clean(s_screen); /* All references belong to this worker; no UI timers. */
    label("我的车库", 14, 8, 114, &carcard_font_14, ACCENT);
    char power[20];
    if (battery >= 0 && battery <= 100) snprintf(power, sizeof(power), "电量 %d%%", battery);
    else snprintf(power, sizeof(power), "电量 --");
    lv_obj_t *bat = label(power, 155, 8, 70, &carcard_font_14, DIM);
    lv_obj_set_style_text_align(bat, LV_TEXT_ALIGN_RIGHT, 0);
    if (m->editing) {
        editor(m);
    } else {
        if (m->page == 0) home(m); else specs(m);
        char position[48];
        snprintf(position, sizeof(position), "%s  %u/%u · %s",
                 m->vehicle == CARCARD_R36 ? "R36" : "POLO",
                 (unsigned)m->vehicle + 1, (unsigned)CARCARD_VEHICLE_COUNT,
                 m->page == 0 ? "名片" : "参数");
        const char *status = position;
        if (!mileage_valid) status = "里程读取异常";
        if (!storage_ok) status = "存储不可用";
        if (!input_ok) status = "按键不可用";
        lv_obj_t *page = label(status, 14, 280, 212, &carcard_font_14,
                               storage_ok && input_ok && mileage_valid ? DIM : WARN);
        lv_obj_set_style_text_align(page, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_t *hint = label("上下切页  确认换车", 14, 300, 212, &carcard_font_14, DIM);
        lv_obj_set_style_text_align(hint, LV_TEXT_ALIGN_CENTER, 0);
    }
}
