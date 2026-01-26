#include "display.h"

#include "common/error.h"
#include "common/util.h"
#include "menu/bpm_counter.h"
#include "peripheral/i2c.h"
#include "system/systick.h"

#include "pico_ssd1306/ssd1306.h"

#include <hardware/i2c.h>

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define SSD1306_I2C_ADDRESS (0x3C)

#define HEADER_Y_POS (0U)
#define HEADER_TEXT_SCALE (1U)

#define BPM_Y_POS (20U)
#define BPM_TEXT_SCALE (2U)

#define FOOTER_LINE_Y_POS (54U)
#define MENU_HINT_Y_POS (44U)
#define MENU_HINT_SCALE (1U)

#define SLIDER_STATE_SQUARE_Y_POS (56U)
#define SLIDER_STATE_SQUARE_HEIGHT (8U)
#define SLIDER_STATE_SQUARE_WIDTH (3U)
/* #define SLIDER_STATE_SQUARE_GAP (4U) */
#define SLIDER_STATE_SQUARE_MARGIN (1U)

#define DISPLAY_STRING_SIZE (64U)
#define PLAYER_ID_MAX (4U)
#define PLAYER_ID_SQUARE_Y_POS (2U)
#define PLAYER_ID_SQUARE_SIZE (4U)

#define MENU_HINT_TIMEOUT_MS (5000U)

/* Aligment for drawing text */
enum alignment {
    ALIGN_LEFT,
    ALIGN_CENTRE,
    ALIGN_RIGHT,
};

/**
 * @brief Handle for writing to an SSD1306 compatible display
 *
 * @param config The configuration the display was opened with.
 * @param mode The current display menu mode.
 * @param ssd1306 Pointer to the ssd1306 handle.
 */
struct display_handle {
    i2c_inst_t *i2c;
    /* Requires an empty handle to be initialised via ssd1306_init() */
    ssd1306_t ssd1306;

    enum display_mode mode;
    uint32_t touched_state;
};

/* Statically allocated display handle for returning from display_open() */
static struct display_handle display_handle = {};

display_handle_t display_open(const struct display_config *config)
{
    if (!config) {
        return NULL;
    }

    display_handle_t display = &display_handle;

    if (!i2c_open(&config->i2c)) {
        return NULL;
    }

    if (!ssd1306_init(&display->ssd1306, config->width, config->height, SSD1306_I2C_ADDRESS,
                      config->i2c.instance)) {
        display_close(display);

        return NULL;
    }

    /* Show a blank display. Useful when debugging */
    ssd1306_clear(&display->ssd1306);
    ssd1306_show(&display->ssd1306);

    display->i2c = config->i2c.instance;
    display->mode = DISPLAY_MODE_IDLE;

    return &display_handle;
}

enum error display_close(display_handle_t display)
{
    if (!display) {
        return E_NULL_POINTER;
    }

    ssd1306_clear(&display->ssd1306);
    ssd1306_show(&display->ssd1306);

    /* TODO: De-initialise GPIO */

    return E_SUCCESS;
}

static uint8_t align_x_offset(enum alignment align, uint8_t width, uint8_t scale, uint8_t len)
{
    uint8_t x = 0;
    uint8_t tmp = 0;

    /* Font is 5x8 (WxH) pixels monospace with 1px spacing */
    /* A scale of 2 would make the font 10x16 monospace  with 2px spacing */

    switch (align) {
    case ALIGN_LEFT:
        /* Position already at 0, do nothing */
        break;
    case ALIGN_CENTRE:
        /*
         * X = (Display_Width / 2) -
         *     (scale * (Character_Width + Character_Spacing) * (len / 2))
         *
         * x = 64 - (scale * 6 * (len / 2));
         * x = 64 - (3 * scale * len);
         */
        tmp = (len << 1);
        tmp += len;          /* tmp = 3 * len */
        tmp <<= (scale - 1); /* = tmp = (tmp * scale) (Must be power of 2) */

        x = (width >> 1); /* X = Display_Width / 2 */
        x -= tmp;
        break;
    case ALIGN_RIGHT:
        /* x = display->ssd1306.width - ((5 + 1) * scale * len)) + (1 * scale); */
        tmp = (len << 2);
        tmp += (len << 1);
        tmp <<= (scale - 1); /* Must be power of 2 */

        x = width;
        x -= tmp;
        x += (scale);
        break;
    default:
        break;
    }

    /* Overflowed, reset X position to 0 */
    if (x > width) {
        x = 0;
    }

    return x;
}

static void draw_text(display_handle_t display, enum alignment align, uint32_t y, uint32_t scale,
                      const char *str, size_t size)
{
    const uint8_t x = align_x_offset(align, display->ssd1306.width, scale, size);
    ssd1306_draw_string(&display->ssd1306, x, y, scale, str);
}

static void draw_header(display_handle_t display, const char *str, size_t size)
{
    const uint32_t line_y = 10;

    draw_text(display, ALIGN_CENTRE, HEADER_Y_POS, HEADER_TEXT_SCALE, str, strnlen(str, size));
    ssd1306_draw_line(&display->ssd1306, 0, line_y, display->ssd1306.width, line_y);
}

static void draw_bpm(display_handle_t display, uint16_t bpm)
{
    char str[DISPLAY_STRING_SIZE] = {};
    snprintf(str, DISPLAY_STRING_SIZE, "%u BPM", MIN(bpm, 9999));

    draw_text(display, ALIGN_CENTRE, BPM_Y_POS, BPM_TEXT_SCALE, str,
              strnlen(str, DISPLAY_STRING_SIZE));
}

static enum error draw_player_leds(display_handle_t display, uint8_t player_id)
{
    if (player_id > PLAYER_ID_MAX) {
        return E_INVALID_INPUT;
    }

    /* x = 64 - ((player_id - 1) * 3) - 2; */
    /* x = 65 - (3 * player_id) */
    uint8_t tmp = player_id;
    tmp <<= 1;
    tmp += player_id;
    uint8_t x = (65 - tmp);

    /* Add 1 if x is an even number */
    if (!(bool)(player_id & 0x01)) {
        x += 1;
    }

    /* TODO: Explain what I'm doing here. */
    for (uint8_t id = 0; id < player_id; id++) {
        ssd1306_draw_square(&display->ssd1306, x, PLAYER_ID_SQUARE_Y_POS, PLAYER_ID_SQUARE_SIZE,
                            PLAYER_ID_SQUARE_SIZE);
        /* ssd1306_draw_square(&display->ssd1306, x, FOOTER_LINE_Y_POS, PLAYER_ID_SQUARE_SIZE,
                            PLAYER_ID_SQUARE_SIZE); */
        x += 5;
    }

    return E_SUCCESS;
}

enum error display_ioctl_touched(display_handle_t display, uint32_t touched_state)
{
    if (!display) {
        return E_NULL_POINTER;
    }

    display->touched_state = touched_state;

    return E_SUCCESS;
}

/**
 * @brief Draw the state of the sliders as boxes at the bottom of the display
 *
 * The touched state of the sliders is set via display_ioctl_touched().
 *
 * ------------------------------------
 * |                                  |
 * |                                  |
 * |                                  |
 * |             DISPLAY              |
 * |                                  |
 * |                                  |
 * |                                  |
 * | ■ ■ ■■■■    ■■■■    ■ ■ ■■    ■■ |
 * ------------------------------------
 *   10101111000011110000101011000011 (0xAF0F 0AC3)
 */
static void draw_slider_state(display_handle_t display, uint32_t touched_state)
{
    ssd1306_draw_line(&display->ssd1306, 0, FOOTER_LINE_Y_POS, display->ssd1306.width,
                      FOOTER_LINE_Y_POS);

    for (uint8_t electrode = 0; electrode < UINT32_WIDTH; electrode++) {
        if ((bool)(touched_state & (1 << electrode))) {
            /*
             * Because the LSB of the electrodes touched state is on the
             * right-hand side, the squares have to be drawn from right to
             * left.
             *
             * X = Display Width - Square Width - Electrode * (Margin + Width)
             *
             * TODO: RHS has a margin only. Document an example image
             */
            ssd1306_draw_square(
                &display->ssd1306,
                display->ssd1306.width - SLIDER_STATE_SQUARE_WIDTH - (electrode << 2),
                SLIDER_STATE_SQUARE_Y_POS, SLIDER_STATE_SQUARE_WIDTH, SLIDER_STATE_SQUARE_HEIGHT);
        }
    }
}

static void draw_menu_hint(display_handle_t display)
{
    const char str[] = "Hold STA+SEL for Menu";

    draw_text(display, ALIGN_CENTRE, MENU_HINT_Y_POS, MENU_HINT_SCALE, str, strlen(str));
}

static void draw_idle_screen(display_handle_t display)
{
    char str[DISPLAY_STRING_SIZE] = {};
    /* snprintf(str, DISPLAY_STRING_SIZE, "%s Mode", usb_mode_to_string(get_usb_mode())); */
    snprintf(str, DISPLAY_STRING_SIZE, "%s Mode", "Keyboard");

    /* const uint8_t player_id = player_id_read(); */
    const uint8_t player_id = 4;
    draw_player_leds(display, player_id);

    draw_header(display, str, DISPLAY_STRING_SIZE);

    /* TODO: Remove debug bpm */
    /* const uint16_t bpm = bpm_counter_read(); */
    draw_bpm(display, 300);

    /* Only draw the menu hint on system start-up */
    if (systick_now_ms() < MENU_HINT_TIMEOUT_MS) {
        draw_menu_hint(display);
    }

    draw_slider_state(display, display->touched_state);
}

static void draw_menu_screen(display_const_handle_t display)
{
    (void)display;
}

enum error display_ioctl_mode(display_handle_t display, enum display_mode mode)
{
    if (!display) {
        return E_NULL_POINTER;
    }

    if (mode != DISPLAY_MODE_IDLE && mode != DISPLAY_MODE_MENU) {
        return E_INVALID_INPUT;
    }

    display->mode = mode;

    return E_SUCCESS;
}

enum error display_update(display_handle_t display)
{
    if (!display) {
        return E_NULL_POINTER;
    }

    ssd1306_clear(&display->ssd1306);
    enum error err = E_SUCCESS;

    if (display->mode == DISPLAY_MODE_IDLE) {
        draw_idle_screen(display);
    }
    else if (display->mode == DISPLAY_MODE_MENU) {
        draw_menu_screen(display);
    }
    else {
        err = E_INVALID_INPUT;
    }

    ssd1306_show(&display->ssd1306);

    return err;
}
