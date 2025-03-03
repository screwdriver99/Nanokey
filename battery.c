#include "battery.h"

#include "bt.h"
#include "hal.h"

void _poweroff()
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
        _poweroff();
    }
}