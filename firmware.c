#include "firmware.h"

#include <math.h>
#include <string.h>

#include "bt.h"
#include "hal.h"
#include "keyboardhid.h"
#include "main.h"
#include "types.h"

bool btMode                    = false;
BluetoothDeviceStatus btstatus = BDS_Disconnected;
uint8_t animation[MATRIX_H][MATRIX_W];
bool firstCycle = true;

void initLedDriver()
{
    i2cSelectBank(0x03);

    // go to shutdown mode
    i2cWriteReg(0x00, 0x00);
    // set internal pullup and pulldown
    i2cWriteReg(0x13, 0xAA);
    // set number of scan phases
    i2cWriteReg(0x14, 0x00);
    // set PWM delay phase
    i2cWriteReg(0x15, 0x04);
    // set sink/source slew rates
    i2cWriteReg(0x16, 0xC0);
    // disable IREF mode
    i2cWriteReg(0x1A, 0x00);

    i2cSelectBank(0x00);

    // disable all LEDs
    for (int i = 0; i < 0x18; i++)
    {
        i2cWriteReg(i, 0x00);
    }

    i2cSelectBank(0x01);

    // set light intensity to zero
    for (int i = 0; i < 0xBF; i++)
    {
        i2cWriteReg(i, 0x00);
    }

    i2cSelectBank(0x04);

    // set LEDs current source to 8,8mA to preserve LED life
    uint8_t buf[12];
    memset(buf, 0x38, sizeof(buf));
    for (int i = 0; i < 12; i++)
    {
        i2cWriteReg(i, buf[i]);
    }

    i2cSelectBank(0x00);

    // enable all LEDs
    for (int i = 0; i < 0x18; i++)
    {
        i2cWriteReg(i, 0xff);
    }

    i2cSelectBank(0x03);
    // exit from shutdown mode
    i2cWriteReg(0x00, 0x01);
    // leave PWM control register selected
    i2cSelectBank(0x01);
}

void initLedMatrix()
{
    // disable hardware shutdown if enabled
    writePin(MATRIX_ENA, true);

    delayms(25);

    // init both LED drivers
    i2csetSlaveAddr(DRIVER_ADDR_1);
    initLedDriver();

    i2csetSlaveAddr(DRIVER_ADDR_2);
    initLedDriver();
}

void setLed(uint8_t row, uint8_t col, uint8_t r, uint8_t g, uint8_t b)
{
    if (ledconf[row][col].driver == DRIVER_UNUSED) return;
    i2csetSlaveAddr(ledconf[row][col].driver);

    // if we write red channel first, the driver stops working
    // we don't know why.. :/
    i2cWriteReg(ledconf[row][col].g, g);
    i2cWriteReg(ledconf[row][col].r, r);
    i2cWriteReg(ledconf[row][col].b, b);
}

void startup(setupInstructions setup)
{
    for (int i = 0; i < MATRIX_H; i++)
        for (int j = 0; j < MATRIX_W; j++) animation[i][j] = 0;

    HALsetup(setup);
    btInit();
    initLedMatrix();

    delayms(100);  // wait for switch voltage to settle

    btMode = readPin(SW_BTMODE);
}

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

uint8_t keytobf(uint8_t kc)
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

KBShortcut getShortcut(keymap* km, bool* fn)
{
    uint8_t key = NOKC;
    bool fnkey  = false;

    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < 17; j++)
        {
            if (km->row[i] & ((uint32_t)0x1 << j))
            {
                if (i == 5 && j == 12)  // FN pressed
                {
                    fnkey = true;
                }
                else if (!keytobf(keycodes[i][j]))  // not a modifier key
                {
                    key = keycodes[i][j];
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
        }
    }

    return KBS_NONE;
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
}

void poweroff(void)
{
    PWR->SCR = PWR_SCR_CWUF;
    PWR->CR1 |= PWR_CR1_LPMS_SHUTDOWN;
    (void)PWR->CR1;
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
    DBGMCU->CR = 0;
    // Enter low-power mode
    while (true)
    {
        __DSB();
        __WFI();
    }
}

void checkBattery()
{
    uint16_t batteryVoltage = btGetBatteryVoltage();
    if (batteryVoltage == 0) return;  // protocol error / not yet read

    static uint8_t blinker = 0;

    if (batteryVoltage >= BATTERY_FULL)  // battery completely charged
    {
        writePin(LED_BATLOW, false);
        blinker = 0;
    }
    else if (batteryVoltage >= BATTERY_LOW)  // battery normal
    {
        writePin(LED_BATLOW, true);
        blinker = 0;
    }
    else if (batteryVoltage > BATTERY_SHD)  // battery low, not yet critical
    {
        // blink red
        writePin(LED_BATLOW, blinker++ < 10);
        if (blinker == 20) blinker = 0;
    }
    else if (readPin(BATPOW))  // battery critical, goto low power to preserve battery
    {
        // enable LEDs hardware shutdown
        writePin(MATRIX_ENA, false);
        poweroff();
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

void setBluetoothIndicatorLed(uint8_t j, uint8_t r, uint8_t g, uint8_t b)
{
    if (btGetHostIndex() == j)
        setLed(1, j, 0, 0, 255);
    else
        setLed(1, j, r, g, b);
}

void loop()
{
    static keymap km;
    static report_keyboard_t kbReport;
    static KBLeds kbLeds;
    static uint8_t blinker = 0;

    bool fnkey = false;

    getKeys(&km);

    if (firstCycle)
        if (km.row[0] & (uint32_t)0x1) JumpToBootloader();  // DFU with ESC key

    firstCycle = false;

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
        static uint8_t btSlower        = 0;

        if (btstatus == BDS_Connected)
        {
            blinker = 0;
            writePin(LED_BT, true);
            kbLeds = btGetLeds();
            btSendKeys(&kbReport);  // BT report rate is lower than USB
            if (!fnkey && btSlower == 11)
            {
                btSlower = 0;
            }
            btSlower++;
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

    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < 17; j++)
        {
            if (km.row[i] & ((uint32_t)0x1 << j))  // pressed
            {
                animation[i][j] = 0xff;
            }
            else if (animation[i][j] != 0)  // not pressed
            {
                decrement(&animation[i][j]);
            }

            // bluetooth indicators

            if (btMode && btstatus == BDS_Pairing && isBluetoothIndicator(i, j))
                setBluetoothIndicatorLed(j, 255, 255 - animation[i][j], 255 - animation[i][j]);
            else
                setLed(i, j, 255, 255 - animation[i][j], 255 - animation[i][j]);
        }
    }

    delayms(1);
}
