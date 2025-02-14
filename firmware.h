#ifndef FIRMWARE_H
#define FIRMWARE_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "hal.h"
#include "types.h"

#define DRIVER_ADDR_1 0x77
#define DRIVER_ADDR_2 0x74
#define DRIVER_UNUSED 0x00

// battery configuration
#define BATTERY_FULL 4100
#define BATTERY_LOW 3500
#define BATTERY_SHD 3350

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

#define MATRIX_W 17
#define MATRIX_H 6

    static const rgbledconf ledconf[][MATRIX_W] = {
        {
            // ROW 1 [DRV 1]
            {0x80, 0x60, 0x70, DRIVER_ADDR_1},  // ESC
            {0x61, 0x71, 0x81, DRIVER_ADDR_1},  // F1
            {0x72, 0x82, 0x62, DRIVER_ADDR_1},  // F2
            {0x83, 0x63, 0x73, DRIVER_ADDR_1},  // F3
            {0x64, 0x74, 0x84, DRIVER_ADDR_1},  // F4
            {0x75, 0x85, 0x65, DRIVER_ADDR_1},  // F5
            {0x86, 0x66, 0x76, DRIVER_ADDR_1},  // F6
            {0x67, 0x77, 0x87, DRIVER_ADDR_1},  // F7
            {0x78, 0x88, 0x68, DRIVER_ADDR_1},  // F8
            {0x89, 0x69, 0x79, DRIVER_ADDR_1},  // F9
            {0x6a, 0x7a, 0x8a, DRIVER_ADDR_1},  // F10
            {0x7b, 0x8b, 0x6b, DRIVER_ADDR_1},  // F11
            {0x8c, 0x6c, 0x7c, DRIVER_ADDR_1},  // F12
            {0x6d, 0x7d, 0x8d, DRIVER_UNUSED},  // ghost
            {0x7e, 0x8e, 0x6e, DRIVER_ADDR_1},  // SCREENSHOT
            {0x8f, 0x6f, 0x7f, DRIVER_ADDR_1},  // MIC
            {0x01, 0x11, 0x21, DRIVER_ADDR_2},  // LIGHT_EFFECT
        },
        {
            // ROW 2 [DRV 1]
            {0x20, 0x00, 0x10, DRIVER_ADDR_1},  // `
            {0x01, 0x11, 0x21, DRIVER_ADDR_1},  // 1
            {0x12, 0x22, 0x02, DRIVER_ADDR_1},  // 2
            {0x23, 0x03, 0x13, DRIVER_ADDR_1},  // 3
            {0x04, 0x14, 0x24, DRIVER_ADDR_1},  // 4
            {0x15, 0x25, 0x05, DRIVER_ADDR_1},  // 5
            {0x26, 0x06, 0x16, DRIVER_ADDR_1},  // 6
            {0x07, 0x17, 0x27, DRIVER_ADDR_1},  // 7
            {0x18, 0x28, 0x08, DRIVER_ADDR_1},  // 8
            {0x29, 0x09, 0x19, DRIVER_ADDR_1},  // 9
            {0x0a, 0x1a, 0x2a, DRIVER_ADDR_1},  // 0
            {0x1b, 0x2b, 0x0b, DRIVER_ADDR_1},  //-
            {0x2c, 0x0c, 0x1c, DRIVER_ADDR_1},  //=
            {0x0d, 0x1d, 0x2d, DRIVER_ADDR_1},  // BACKSPACE
            {0x1e, 0x2e, 0x0e, DRIVER_ADDR_1},  // INS
            {0x2f, 0x0f, 0x1f, DRIVER_ADDR_1},  // HOME
            {0x61, 0x71, 0x81, DRIVER_ADDR_2},  // PGUP
        },
        {
            // ROW 3 [DRV 1]
            {0x50, 0x30, 0x40, DRIVER_ADDR_1},  // TAB
            {0x31, 0x41, 0x51, DRIVER_ADDR_1},  // Q
            {0x42, 0x52, 0x32, DRIVER_ADDR_1},  // W
            {0x53, 0x33, 0x43, DRIVER_ADDR_1},  // E
            {0x34, 0x44, 0x54, DRIVER_ADDR_1},  // R
            {0x45, 0x55, 0x35, DRIVER_ADDR_1},  // T
            {0x56, 0x36, 0x46, DRIVER_ADDR_1},  // Y
            {0x37, 0x47, 0x57, DRIVER_ADDR_1},  // U
            {0x48, 0x58, 0x38, DRIVER_ADDR_1},  // I
            {0x59, 0x39, 0x49, DRIVER_ADDR_1},  // O
            {0x3a, 0x4a, 0x5a, DRIVER_ADDR_1},  // P
            {0x4b, 0x5b, 0x3b, DRIVER_ADDR_1},  // [
            {0x5c, 0x3c, 0x4c, DRIVER_ADDR_1},  // ]
            {0x3d, 0x4d, 0x5d, DRIVER_ADDR_1},  // BACKSLASH
            {0x4e, 0x5e, 0x3e, DRIVER_ADDR_1},  // DEL
            {0x5f, 0x3f, 0x4f, DRIVER_ADDR_1},  // END
            {0x00, 0x10, 0x20, DRIVER_ADDR_2},  // PGDWN
        },
        {
            // ROW 4 [DRV 2]
            {0x2f, 0x0f, 0x1f, DRIVER_ADDR_2},  // CAPS
            {0x0e, 0x1e, 0x2e, DRIVER_ADDR_2},  // A
            {0x1d, 0x2d, 0x0d, DRIVER_ADDR_2},  // S
            {0x2c, 0x0c, 0x1c, DRIVER_ADDR_2},  // D
            {0x0b, 0x1b, 0x2b, DRIVER_ADDR_2},  // F
            {0x1a, 0x2a, 0x0a, DRIVER_ADDR_2},  // G
            {0x29, 0x09, 0x19, DRIVER_ADDR_2},  // H
            {0x08, 0x18, 0x28, DRIVER_ADDR_2},  // J
            {0x17, 0x27, 0x07, DRIVER_ADDR_2},  // K
            {0x26, 0x06, 0x16, DRIVER_ADDR_2},  // L
            {0x05, 0x15, 0x25, DRIVER_ADDR_2},  // ;
            {0x14, 0x24, 0x04, DRIVER_ADDR_2},  // '
            {0xff, 0xff, 0xff, DRIVER_UNUSED},  // not used
            {0x02, 0x12, 0x22, DRIVER_ADDR_2},  // ENTER
            {0xff, 0xff, 0xff, DRIVER_UNUSED},  // not used
            {0xff, 0xff, 0xff, DRIVER_UNUSED},  // not used
            {0xff, 0xff, 0xff, DRIVER_UNUSED},  // not used
        },
        {
            // ROW 5 [DRV 2]
            {0x8f, 0x6f, 0x7f, DRIVER_ADDR_2},  // LSHIFT
            {0x6e, 0x7e, 0x8e, DRIVER_UNUSED},  // ghost
            {0x7d, 0x8d, 0x6d, DRIVER_ADDR_2},  // Z
            {0x8c, 0x6c, 0x7c, DRIVER_ADDR_2},  // X
            {0x6b, 0x7b, 0x8b, DRIVER_ADDR_2},  // C
            {0x7a, 0x8a, 0x6a, DRIVER_ADDR_2},  // V
            {0x89, 0x69, 0x79, DRIVER_ADDR_2},  // B
            {0x68, 0x78, 0x88, DRIVER_ADDR_2},  // N
            {0x77, 0x87, 0x67, DRIVER_ADDR_2},  // M
            {0x86, 0x66, 0x76, DRIVER_ADDR_2},  // ,
            {0x65, 0x75, 0x85, DRIVER_ADDR_2},  // .
            {0x74, 0x84, 0x64, DRIVER_ADDR_2},  // /
            {0x83, 0x63, 0x73, DRIVER_UNUSED},  // ghost
            {0x62, 0x72, 0x82, DRIVER_ADDR_2},  // RSHIFT
            {0xff, 0xff, 0xff, DRIVER_UNUSED},  // not used
            {0x80, 0x60, 0x70, DRIVER_ADDR_2},  // UARROW
            {0xff, 0xff, 0xff, DRIVER_UNUSED},  // not used
        },
        {
            // ROW 6 [DRV 2]
            {0x5f, 0x3f, 0x4f, DRIVER_ADDR_2},  // LCTRL
            {0x3e, 0x4e, 0x5e, DRIVER_ADDR_2},  // LGUI
            {0x4d, 0x5d, 0x3d, DRIVER_ADDR_2},  // LALT
            {0x5c, 0x3c, 0x4c, DRIVER_UNUSED},  // ghost
            {0x3b, 0x4b, 0x5b, DRIVER_UNUSED},  // ghost
            {0x4a, 0x5a, 0x3a, DRIVER_UNUSED},  // ghost
            {0x59, 0x39, 0x49, DRIVER_ADDR_2},  // SPACE
            {0x38, 0x48, 0x58, DRIVER_UNUSED},  // ghost
            {0x47, 0x57, 0x37, DRIVER_UNUSED},  // ghost
            {0x56, 0x36, 0x46, DRIVER_UNUSED},  // ghost
            {0x35, 0x45, 0x55, DRIVER_ADDR_2},  // RALT
            {0x44, 0x54, 0x34, DRIVER_ADDR_2},  // RGUI
            {0x53, 0x33, 0x43, DRIVER_ADDR_2},  // FN
            {0x32, 0x42, 0x52, DRIVER_ADDR_2},  // RCTRL
            {0x41, 0x51, 0x31, DRIVER_ADDR_2},  // LARROW
            {0x50, 0x30, 0x40, DRIVER_ADDR_2},  // DARROW
            {0x03, 0x13, 0x23, DRIVER_ADDR_2},  // RARROW
        },
    };

    static const uint8_t keycodes[][MATRIX_W] = {
        {
            // ROW 1
            0x29,  // ESC
            0x3a,  // F1
            0x3b,  // F2
            0x3c,  // F3
            0x3d,  // F4
            0x3e,  // F5
            0x3f,  // F6
            0x40,  // F7
            0x41,  // F8
            0x42,  // F9
            0x43,  // F10
            0x44,  // F11
            0x45,  // F12
            NOKC,  // ghost
            0x46,  // SCREENSHOT
            NOKC,  // MIC
            NOKC,  // LIGHT_EFFECT
        },
        {
            // ROW 2
            0x35,  // `
            0x1e,  // 1
            0x1f,  // 2
            0x20,  // 3
            0x21,  // 4
            0x22,  // 5
            0x23,  // 6
            0x24,  // 7
            0x25,  // 8
            0x26,  // 9
            0x27,  // 0
            0x2d,  // -
            0x2e,  // =
            0x2a,  // BACKSPACE
            0x49,  // INS
            0x4a,  // HOME
            0x4b,  // PGUP
        },
        {
            // ROW 3
            0x2b,  // TAB
            0x14,  // Q
            0x1a,  // W
            0x08,  // E
            0x15,  // R
            0x17,  // T
            0x1c,  // Y
            0x18,  // U
            0x0c,  // I
            0x12,  // O
            0x13,  // P
            0x2f,  // [
            0x30,  // ]
            0x31,  // BACKSLASH
            0x4c,  // DEL
            0x4d,  // END
            0x4e,  // PGDWN
        },
        {
            // ROW 4
            0x39,  // CAPS
            0x04,  // A
            0x16,  // S
            0x07,  // D
            0x09,  // F
            0x0a,  // G
            0x0b,  // H
            0x0d,  // J
            0x0e,  // K
            0x0f,  // L
            0x33,  // ;
            0x34,  // '
            NOKC,  // not used
            0x28,  // ENTER
            NOKC,  // not used
            NOKC,  // not used
            NOKC,  // not used
        },
        {
            // ROW 5
            LSHF,  // LSHIFT
            NOKC,  // ghost
            0x1d,  // Z
            0x1b,  // X
            0x06,  // C
            0x19,  // V
            0x05,  // B
            0x11,  // N
            0x10,  // M
            0x36,  // ,
            0x37,  // .
            0x38,  // /
            NOKC,  // ghost
            RSHF,  // RSHIFT
            NOKC,  // not used
            0x52,  // UARROW
            NOKC,  // not used
        },
        {
            // ROW 6
            LCTL,  // LCTRL
            LGUI,  // LGUI
            LALT,  // LALT
            NOKC,  // ghost
            NOKC,  // ghost
            NOKC,  // ghost
            0x2c,  // SPACE
            NOKC,  // ghost
            NOKC,  // ghost
            NOKC,  // ghost
            RALT,  // RALT
            RGUI,  // RGUI
            NOKC,  // FN   (place 5,12)
            RCTL,  // RCTRL
            0x50,  // LARROW
            0x51,  // DARROW
            0x4f,  // RARROW
        },
    };

    void initLedDriver();

    void initLedMatrix();

    void setLed(uint8_t row, uint8_t col, uint8_t r, uint8_t g, uint8_t b);

    void startup(setupInstructions setup);

    void loop();

#ifdef __cplusplus
}
#endif

#endif /* FIRMWARE_H */
