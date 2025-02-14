#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>

#include "Core/Inc/main.h"
#include "usbd_def.h"

#ifdef __cplusplus
extern "C"
{
#endif

    enum consumer_usages
    {
        // 15.5 Display Controls
        SNAPSHOT               = 0x065,
        BRIGHTNESS_UP          = 0x06F,  // https://www.usb.org/sites/default/files/hutrr41_0.pdf
        BRIGHTNESS_DOWN        = 0x070,
        // 15.7 Transport Controls
        TRANSPORT_RECORD       = 0x0B2,
        TRANSPORT_FAST_FORWARD = 0x0B3,
        TRANSPORT_REWIND       = 0x0B4,
        TRANSPORT_NEXT_TRACK   = 0x0B5,
        TRANSPORT_PREV_TRACK   = 0x0B6,
        TRANSPORT_STOP         = 0x0B7,
        TRANSPORT_EJECT        = 0x0B8,
        TRANSPORT_RANDOM_PLAY  = 0x0B9,
        TRANSPORT_STOP_EJECT   = 0x0CC,
        TRANSPORT_PLAY_PAUSE   = 0x0CD,
        // 15.9.1 Audio Controls - Volume
        AUDIO_MUTE             = 0x0E2,
        AUDIO_VOL_UP           = 0x0E9,
        AUDIO_VOL_DOWN         = 0x0EA,
        // 15.15 Application Launch Buttons
        AL_CC_CONFIG           = 0x183,
        AL_EMAIL               = 0x18A,
        AL_CALCULATOR          = 0x192,
        AL_LOCAL_BROWSER       = 0x194,
        AL_LOCK                = 0x19E,
        AL_CONTROL_PANEL       = 0x19F,
        AL_ASSISTANT           = 0x1CB,
        AL_KEYBOARD_LAYOUT     = 0x1AE,
        // 15.16 Generic GUI Application Controls
        AC_NEW                 = 0x201,
        AC_OPEN                = 0x202,
        AC_CLOSE               = 0x203,
        AC_EXIT                = 0x204,
        AC_MAXIMIZE            = 0x205,
        AC_MINIMIZE            = 0x206,
        AC_SAVE                = 0x207,
        AC_PRINT               = 0x208,
        AC_PROPERTIES          = 0x209,
        AC_UNDO                = 0x21A,
        AC_COPY                = 0x21B,
        AC_CUT                 = 0x21C,
        AC_PASTE               = 0x21D,
        AC_SELECT_ALL          = 0x21E,
        AC_FIND                = 0x21F,
        AC_SEARCH              = 0x221,
        AC_HOME                = 0x223,
        AC_BACK                = 0x224,
        AC_FORWARD             = 0x225,
        AC_STOP                = 0x226,
        AC_REFRESH             = 0x227,
        AC_BOOKMARKS           = 0x22A,
        AC_MISSION_CONTROL     = 0x29F,
        AC_LAUNCHPAD           = 0x2A0
    };

    enum desktop_usages
    {
        // 4.5.1 System Controls - Power Controls
        SYSTEM_POWER_DOWN             = 0x81,
        SYSTEM_SLEEP                  = 0x82,
        SYSTEM_WAKE_UP                = 0x83,
        SYSTEM_RESTART                = 0x8F,
        // 4.10 System Display Controls
        SYSTEM_DISPLAY_TOGGLE_INT_EXT = 0xB5
    };

    typedef struct
    {
        I2C_HandleTypeDef* i2ch;
        RNG_HandleTypeDef* rngh;
        USBD_HandleTypeDef* usbdh;
        UART_HandleTypeDef* uarth;
    } setupInstructions;

    typedef struct
    {
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t driver;
    } rgbledconf;

    typedef struct
    {
        uint32_t row[6];
    } keymap;

    typedef struct
    {
        uint8_t numlock : 1;
        uint8_t capslock : 1;
        uint8_t scrolllock : 1;
        uint8_t compose : 1;
        uint8_t kana : 1;
        uint8_t spare1 : 1;
        uint8_t spare2 : 1;
        uint8_t spare3 : 1;
    } __attribute__((packed)) KBLeds;

    typedef union
    {
        uint8_t raw[16];
        struct
        {
            uint8_t mods;
            uint8_t reserved;
            uint8_t keys[6];
        } std;

        struct nkro_report
        {
            uint8_t mods;
            uint8_t bits[15];
        } nkro;

    } __attribute__((packed)) report_keyboard_t;

    typedef struct
    {
        uint8_t report_id;
        uint16_t usage;
    } __attribute__((packed)) report_extra_t;

    typedef struct
    {
        uint8_t report_id;
        uint32_t usage;
    } __attribute__((packed)) report_programmable_button_t;

    typedef struct
    {
        uint8_t hostIndex;
        uint16_t timeout;    /* Pairing timeout, valid value range from 30 to 3600 seconds, 0 for default */
        uint8_t pairingMode; /* 0: default, 1: Just Works, 2: Passkey Entry */
        uint8_t BRorLE;      /* Only available for dual mode module. Keep 0 for single mode module */
        uint8_t txPower;     /* Only available for BLE module */
        const char* leName;  /* Only available for BLE module */
    } pairing_param_t;

    typedef struct
    {
        uint8_t model_name[11];
        uint8_t mode;
        uint8_t bluetooth_version;
        uint8_t firmware_version[11];
        uint8_t hardware_version[11];
        uint16_t cmd_set_verson;
    } __attribute__((packed)) module_info_t;

    typedef struct
    {
        uint8_t event_mode; /* Must be 0x02 */
        uint16_t connected_idle_timeout;
        uint16_t pairing_timeout;   /* Range: 30 ~ 3600 second, 0 for default */
        uint8_t pairing_mode;       /* 0: default, 1: Just Works, 2: Passkey Entry */
        uint16_t reconnect_timeout; /* 0: default, 0xFF: Unlimited time, 2 ~ 254 seconds */
        uint8_t report_rate;        /* 90 or 133 */
        uint8_t rsvd1;
        uint8_t rsvd2;
        uint8_t vendor_id_source; /* 0: From Bluetooth SIG, 1: From USB-IF */
        uint16_t verndor_id;      /* No effect, the vendor ID is 0x3434 */
        uint16_t product_id;
        /* Below parametes is only available for BLE module  */
        uint16_t le_connection_interval_min;
        uint16_t le_connection_interval_max;
        uint16_t le_connection_interval_timeout;
    } __attribute__((packed)) module_param_t;

    typedef enum
    {
        BDS_Disconnected,
        BDS_Connected,
        BDS_Pairing,
    } BluetoothDeviceStatus;

    typedef enum
    {
        KBS_NONE,
        KBS_FN_1,    // bluetooth slot 1
        KBS_FN_2,    // bluetooth slot 2
        KBS_FN_3,    // bluetooth slot 3
        KBS_FN_4,    // bluetooth discoverable
        KBS_FN_ESC,  // bluetooth device factory reset
    } KBShortcut;

    typedef struct
    {
        GPIO_TypeDef* port;
        uint16_t pin;
        bool inverted;
    } pin;

    static const pin syspins[] = {
        // OUTPUTS:
        {LEDSD_GPIO_Port, LEDSD_Pin, false},
        {BATLOW_GPIO_Port, BATLOW_Pin, false},
        {CAPS_GPIO_Port, CAPS_Pin, false},
        {SHCP_GPIO_Port, SHCP_Pin, false},
        {SRDS_GPIO_Port, SRDS_Pin, false},
        {STCP_GPIO_Port, STCP_Pin, false},
        {COL_0_GPIO_Port, COL_0_Pin, false},
        {WUP_BTMOD_GPIO_Port, WUP_BTMOD_Pin, false},
        {BTLED_GPIO_Port, BTLED_Pin, false},
        {BTRESET_GPIO_Port, BTRESET_Pin, false},

        // INPUTS:
        {ROW_0_GPIO_Port, ROW_0_Pin, true},
        {ROW_1_GPIO_Port, ROW_1_Pin, true},
        {ROW_2_GPIO_Port, ROW_2_Pin, true},
        {ROW_3_GPIO_Port, ROW_3_Pin, true},
        {ROW_4_GPIO_Port, ROW_4_Pin, true},  // used for debug probe
        {ROW_5_GPIO_Port, ROW_5_Pin, true},  // used for debug probe
        {BTMODE_GPIO_Port, BTMODE_Pin, true},
        {MACMODE_GPIO_Port, MACMODE_Pin, false},
        {BATPOW_GPIO_Port, BATPOW_Pin, true},
    };

// OUTPUTS:
#define MATRIX_ENA 0  // LED matrix enable (write low = shutdown)
#define LED_BATLOW 1  // Battery low LED indicator
#define LED_CAPS 2    // Caps LED
#define SRCLK 3       // Shift register clock
#define SRDAT 4       // Shift register data
#define SRSTG 5       // Shift register storage register clock
#define COL_0 6       // Column 0 enable pin
#define WUP_BT 7      // Wakeup pin of the bluetooth module
#define LED_BT 8      // Bluetooth LED indicator
#define BTRESET 9     // Bluetooth module reset

// INPUTS:
#define ROW_0 10
#define ROW_1 11
#define ROW_2 12
#define ROW_3 13
#define ROW_4 14
#define ROW_5 15
#define SW_BTMODE 16
#define SW_MACMODE 17
#define BATPOW 18

#ifdef __cplusplus
}
#endif

#endif /* TYPES_H */
