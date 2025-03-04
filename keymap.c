#include "keymap.h"

void addKeyCode(report_keyboard_t* report, uint8_t kc)
{
    // find first available slot
    for (uint8_t i = 0; i < 6; i++)
        if (report->std.keys[i] == NOKC)
        {
            report->std.keys[i] = kc;
            return;
        }
}

uint8_t keyToBitField(uint16_t kc)
{
    switch (kc)
    {
        case RGUI:
            return RGUIB;
        case RALT:
            return RALTB;
        case RSHF:
            return RSHFB;
        case RCTL:
            return RCTLB;
        case LGUI:
            return LGUIB;
        case LALT:
            return LALTB;
        case LSHF:
            return LSHFB;
        case LCTL:
            return LCTLB;

        default:
            return 0x00;
    }
}

KBShortcut getShortcut(keymap* km, bool* fn, uint8_t kcmode)
{
    uint16_t key = NOKC;
    bool fnkey   = false;

    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < 17; j++)
        {
            if (km->row[i] & ((uint32_t)0x1 << j))
            {
                if (i == 5 && j == 12)  // FN pressed
                    fnkey = true;
                else if (!keyToBitField(keycodes[i][j][kcmode]))  // not a modifier key
                {
                    key = keycodes[i][j][kcmode];
                    if (fnkey) break;  // already detected
                }
            }
        }
        if (fnkey && key != NOKC) break;  // already detected
    }

    if (fn) *fn = fnkey;

    // shortcut state machine

    static bool waitforkey = false;

    if (!fnkey)
        waitforkey = false;

    else if (!waitforkey && key == NOKC)  // FN key pressed, not pressed before, no other key pressed
        waitforkey = true;

    else if (waitforkey && key != NOKC)  // FN key pressed, pressed before and also other key pressed
    {
        switch (key)
        {
            case 0x1e:
                return KBS_FN_1;
            case 0x1f:
                return KBS_FN_2;
            case 0x20:
                return KBS_FN_3;
            case 0x21:
                return KBS_FN_4;
            case 0x29:
                return KBS_FN_ESC;
            case 0x4b:
                return KBS_FN_PGUP;
            case 0x4e:
                return KBS_FN_PGDN;
            case 0x49:
                return KBS_FN_INS;
            case 0x4c:
                return KBS_FN_DEL;
            case M1:
                return KBS_FN_M1;
            case M2:
                return KBS_FN_M2;
            case M3:
                return KBS_FN_M3;
        }
    }

    return KBS_NONE;
}