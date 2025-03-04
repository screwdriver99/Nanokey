#ifndef KEYMAP_H
#define KEYMAP_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "types.h"

#define NOKC 0x00

// HID KB BITFIELD VALUES FOR MOD KEYS
#define RGUIB 0x80
#define RALTB 0x40
#define RSHFB 0x20
#define RCTLB 0x10
#define LGUIB 0x08
#define LALTB 0x04
#define LSHFB 0x02
#define LCTLB 0x01

// HID KB KEYCODES FOR MOD KEYS
#define RGUI 0xe7
#define RALT 0xe6
#define RSHF 0xe5
#define RCTL 0xe4
#define LGUI 0xe3
#define LALT 0xe2
#define LSHF 0xe1
#define LCTL 0xe0

// KEYCODE MODE
#define KCMODE_WIN 0x00
#define KCMODE_MAC 0x01

    static const uint8_t keycodes[][MATRIX_W][2] = {
        // ( WIN | MAC )
        {
            // ROW 1
            {0x29, 0x29},  // ESC
            {0x3a, 0x3a},  // F1
            {0x3b, 0x3b},  // F2
            {0x3c, 0x3c},  // F3
            {0x3d, 0x3d},  // F4
            {0x3e, 0x3e},  // F5
            {0x3f, 0x3f},  // F6
            {0x40, 0x40},  // F7
            {0x41, 0x41},  // F8
            {0x42, 0x42},  // F9
            {0x43, 0x43},  // F10
            {0x44, 0x44},  // F11
            {0x45, 0x45},  // F12
            {NOKC, NOKC},  // ghost
            {NOKC, NOKC},  // M1
            {NOKC, NOKC},  // M2
            {NOKC, NOKC},  // M3
        },
        {
            // ROW 2
            {0x35, 0x35},  // `
            {0x1e, 0x1e},  // 1
            {0x1f, 0x1f},  // 2
            {0x20, 0x20},  // 3
            {0x21, 0x21},  // 4
            {0x22, 0x22},  // 5
            {0x23, 0x23},  // 6
            {0x24, 0x24},  // 7
            {0x25, 0x25},  // 8
            {0x26, 0x26},  // 9
            {0x27, 0x27},  // 0
            {0x2d, 0x2d},  // -
            {0x2e, 0x2e},  // =
            {0x2a, 0x2a},  // BACKSPACE
            {0x49, 0x49},  // INS
            {0x4a, 0x4a},  // HOME
            {0x4b, 0x4b},  // PGUP
        },
        {
            // ROW 3
            {0x2b, 0x2b},  // TAB
            {0x14, 0x14},  // Q
            {0x1a, 0x1a},  // W
            {0x08, 0x08},  // E
            {0x15, 0x15},  // R
            {0x17, 0x17},  // T
            {0x1c, 0x1c},  // Y
            {0x18, 0x18},  // U
            {0x0c, 0x0c},  // I
            {0x12, 0x12},  // O
            {0x13, 0x13},  // P
            {0x2f, 0x2f},  // [
            {0x30, 0x30},  // ]
            {0x31, 0x31},  // BACKSLASH
            {0x4c, 0x4c},  // DEL
            {0x4d, 0x4d},  // END
            {0x4e, 0x4e},  // PGDWN
        },
        {
            // ROW 4
            {0x39, 0x39},  // CAPS
            {0x04, 0x04},  // A
            {0x16, 0x16},  // S
            {0x07, 0x07},  // D
            {0x09, 0x09},  // F
            {0x0a, 0x0a},  // G
            {0x0b, 0x0b},  // H
            {0x0d, 0x0d},  // J
            {0x0e, 0x0e},  // K
            {0x0f, 0x0f},  // L
            {0x33, 0x33},  // ;
            {0x34, 0x34},  // '
            {NOKC, NOKC},  // not used
            {0x28, 0x28},  // ENTER
            {NOKC, NOKC},  // not used
            {NOKC, NOKC},  // not used
            {NOKC, NOKC},  // not used
        },
        {
            // ROW 5
            {LSHF, LSHF},  // LSHIFT
            {NOKC, NOKC},  // ghost
            {0x1d, 0x1d},  // Z
            {0x1b, 0x1b},  // X
            {0x06, 0x06},  // C
            {0x19, 0x19},  // V
            {0x05, 0x05},  // B
            {0x11, 0x11},  // N
            {0x10, 0x10},  // M
            {0x36, 0x36},  // ,
            {0x37, 0x37},  // .
            {0x38, 0x38},  // /
            {NOKC, NOKC},  // ghost
            {RSHF, RSHF},  // RSHIFT
            {NOKC, NOKC},  // not used
            {0x52, 0x52},  // UARROW
            {NOKC, NOKC},  // not used
        },
        {
            // ROW 6
            {LCTL, LCTL},  // LCTRL
            {LGUI, LALT},  // LGUI | LALT
            {LALT, LGUI},  // LALT | LGUI
            {NOKC, NOKC},  // ghost
            {NOKC, NOKC},  // ghost
            {NOKC, NOKC},  // ghost
            {0x2c, 0x2c},  // SPACE
            {NOKC, NOKC},  // ghost
            {NOKC, NOKC},  // ghost
            {NOKC, NOKC},  // ghost
            {RALT, RGUI},  // RALT | RGUI
            {RGUI, RALT},  // RGUI | RALT
            {NOKC, NOKC},  // FN   (place 5,12)
            {RCTL, RCTL},  // RCTRL
            {0x50, 0x50},  // LARROW
            {0x51, 0x51},  // DARROW
            {0x4f, 0x4f},  // RARROW
        },
    };

    void addKeyCode(report_keyboard_t* report, uint8_t kc);

    uint8_t keytobf(uint8_t kc);

    KBShortcut getShortcut(keymap* km, bool* fn, uint8_t kcmode);

#ifdef __cplusplus
}
#endif

#endif /* KEYMAP_H */
