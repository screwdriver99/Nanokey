#include "firmware.h"

#include <math.h>
#include <string.h>

#include "Core/Inc/main.h"
#include "battery.h"
#include "bt.h"
#include "hal.h"
#include "keyboardhid.h"
#include "keymap.h"
#include "ledmatrix.h"
#include "nvmem.h"

bool btMode                    = false;
BluetoothDeviceStatus btstatus = BDS_Disconnected;

uint8_t animation[MATRIX_H][MATRIX_W];
uint8_t brightness_step = 2;
uint8_t brightness      = 255;

#define BRIGHTNESS_NVS_ID 0

uint8_t map(uint8_t val, uint8_t min, uint8_t max, uint8_t newmin, uint8_t newmax)
{
    return (val - min) * (newmax - newmin) / (max - min) + newmin;
}

void updateBrightnessRaw() { brightness = map(brightness_step, 1, 10, 10, 255); }

void flashLeds()
{
    bool l1 = readPin(LED_BATLOW);
    bool l2 = readPin(LED_BT);
    bool l3 = readPin(LED_CAPS);

    writePin(LED_BATLOW, true);
    writePin(LED_BT, true);
    writePin(LED_CAPS, true);

    delayms(200);

    writePin(LED_BATLOW, l1);
    writePin(LED_BT, l2);
    writePin(LED_CAPS, l3);
}

void newHostPressed(uint8_t idx)
{
    if (btstatus == BDS_Connected)
    {
        btDisconnect();
        btConnect(idx, 2);
    }
    else if (btstatus == BDS_Disconnected)
        btConnect(idx, 2);
}

void changeBrightness(bool inc)
{
    if (inc && brightness_step < 10)
        brightness_step++;
    else if (!inc && brightness_step > 1)
        brightness_step--;

    // update non volatile storage
    nvWrite(BRIGHTNESS_NVS_ID, brightness_step);
    updateBrightnessRaw();
}

// implement 'keep pressed' shortcuts
void kpShortcuts(KBShortcut sh)
{
    static uint8_t shCounter = 0;

    if (sh == KBS_NONE)
        // keep reset
        shCounter = 0;
    else if (shCounter < 100)
    {
        // mask and increment
        sh = KBS_NONE;
        shCounter++;
    }
    else if (shCounter == 100)
        // increment only
        shCounter++;
    else  // mask only
        sh = KBS_NONE;

    // process:

    if (btMode)
    {
        if (sh == KBS_FN_4)
        {
            if (btstatus == BDS_Disconnected)
                btSetDiscoverable(btGetHostIndex());

            else if (btstatus == BDS_Connected)
                btDisconnect();
        }
        else if (sh == KBS_FN_ESC)
        {
            btModuleFactoryReset();
        }
    }

    switch (sh)
    {
        case KBS_FN_INS:
            writePin(MATRIX_ENA, false);  // switch off led matrix to give feedback
            JumpToBootloader();           // go to DFU
            break;

        case KBS_FN_DEL:
            nvFormat();   // restore to factory settings
            flashLeds();  // flash all 3 leds to give feedback
            break;

        default:
            break;
    }
}

// implement 'pressed once' shortcuts
void onceShortcuts(KBShortcut sh)
{
    static KBShortcut old = KBS_NONE;

    if (sh != old)
        old = sh;
    else
        sh = KBS_NONE;  // mask

    if (sh == KBS_NONE) return;

    // process:

    if (btMode)
    {
        uint8_t idx = 0;

        switch (sh)
        {
            case KBS_FN_1:
                idx = 1;
                break;

            case KBS_FN_2:
                idx = 2;
                break;

            case KBS_FN_3:
                idx = 3;
                break;

            default:
                break;
        }

        if (idx != 0) newHostPressed(idx);
    }

    switch (sh)
    {
        case KBS_FN_PGDN:
            changeBrightness(false);
            break;

        case KBS_FN_PGUP:
            changeBrightness(true);
            break;

        default:
            break;
    }
}

void decrement(uint8_t* val)
{
    uint8_t step = ceil(((float)256 - (float)*val) / 15.0f);

    if (*val >= step)
        (*val) -= step;
    else
        *val = 0;
}

bool isBluetoothIndicator(uint8_t i, uint8_t j)
{
    if (i != 1) return false;

    return j == 1 || j == 2 || j == 3;
}

void setBluetoothIndicatorLed(uint8_t i, uint8_t j, rgb color)
{
    if (btGetHostIndex() == j)
    {
        color.r = 0;
        color.g = 0;
        color.b = 255;
    }

    setLed(i, j, color);
}

rgb lightEffect(uint8_t animation)
{
    return (rgb){
        .r = map(animation, 0, 255, brightness, 255),
        .g = map(animation, 0, 255, brightness, 0),
        .b = map(animation, 0, 255, brightness, 0),
    };
}

void startup(setupInstructions setup)
{
    HALsetup(setup);

    // check DFU mode
    if (isESCPressed()) JumpToBootloader();

    for (int i = 0; i < MATRIX_H; i++)
        for (int j = 0; j < MATRIX_W; j++) animation[i][j] = 0;

    nvInit();

    uint64_t b = 0;

    if (nvRead(BRIGHTNESS_NVS_ID, &b) != 0)
        brightness_step = 2;
    else if (b >= 1 && b <= 10)
        brightness_step = b;

    updateBrightnessRaw();

    btInit();
    initLedMatrix();

    delayms(2);  // wait for switch voltage to settle
    btMode = readPin(SW_BTMODE);
}

void loop()
{
    static keymap km;
    static report_keyboard_t kbReport;
    static KBLeds kbLeds;
    static uint8_t blinker   = 0;
    static uint32_t timeTick = 0;

    timeTick = getCurrentTicks();

    bool fnkey = false;

    getKeys(&km);

    KBShortcut shcut = getShortcut(&km, &fnkey);

    memset(&kbReport, 0x00, sizeof(report_keyboard_t));
    memset(&kbLeds, 0x00, sizeof(KBLeds));

    // find up to 6 pressed keys
    uint8_t c   = 0;
    uint8_t mod = 0;
    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < 17; j++)
        {
            if (km.row[i] & ((uint32_t)0x1 << j))
            {
                mod = keytobf(keycodes[i][j]);
                if (mod)
                    kbReport.std.mods |= mod;
                else
                {
                    addKeyCode(&kbReport, keycodes[i][j]);
                    c++;
                    if (c == 6) break;
                }
            }
        }
        if (c == 6) break;
    }

    btstatus = btGetStatus();

    btTask();

    checkBattery();

    kpShortcuts(shcut);
    onceShortcuts(shcut);

    if (btMode)
    {
        static uint8_t retryConnection = 0;

        if (btstatus == BDS_Connected)
        {
            blinker = 0;
            writePin(LED_BT, true);
            kbLeds = btGetLeds();
            btSendKeys(&kbReport);
        }
        else if (btstatus == BDS_Disconnected)
        {
            blinker = 0;
            writePin(LED_BT, false);

            // roughly half a second after power on
            if (retryConnection == 50) btConnect(btGetHostIndex(), 2);
            if (retryConnection <= 50) retryConnection++;
        }
        else  // pairing
        {
            writePin(LED_BT, blinker++ < 10);
            if (blinker == 20) blinker = 0;
        }
    }
    else
    {
        if (!fnkey) usbSendKeys(&kbReport);
        kbLeds = usbGetLeds();
    }

    //
    //============= LEDs
    //

    writePin(LED_CAPS, kbLeds.capslock);
    rgb color;

    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < 17; j++)
        {
            // animation

            if (km.row[i] & ((uint32_t)0x1 << j))  // pressed
            {
                animation[i][j] = 0xff;
            }
            else if (animation[i][j] != 0)  // not pressed
            {
                decrement(&animation[i][j]);
            }

            // colors

            color = lightEffect(animation[i][j]);

            if (btMode && btstatus == BDS_Pairing && isBluetoothIndicator(i, j))
                setBluetoothIndicatorLed(1, j, color);
            else
                setLed(i, j, color);
        }
    }

    // loop time is 12ms for bluetooth 90Hz, or 1ms for USB 1kHz
    pauseUntil(timeTick, btMode ? 12 : 1);
}
