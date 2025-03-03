#include "ledmatrix.h"

#include <string.h>

#include "hal.h"

void _initLedDriver()
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
    for (int i = 0; i < 0x18; i++) i2cWriteReg(i, 0x00);

    i2cSelectBank(0x01);

    // set light intensity to zero
    for (int i = 0; i < 0xBF; i++) i2cWriteReg(i, 0x00);

    i2cSelectBank(0x04);

    // set LEDs current source to 8,8mA to preserve LED life
    uint8_t buf[12];
    memset(buf, 0x38, sizeof(buf));

    for (int i = 0; i < 12; i++) i2cWriteReg(i, buf[i]);

    i2cSelectBank(0x00);

    // enable all LEDs
    for (int i = 0; i < 0x18; i++) i2cWriteReg(i, 0xff);

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
    _initLedDriver();

    i2csetSlaveAddr(DRIVER_ADDR_2);
    _initLedDriver();
}

void setLed(uint8_t row, uint8_t col, rgb color)
{
    if (ledconf[row][col].driver == DRIVER_UNUSED) return;
    i2csetSlaveAddr(ledconf[row][col].driver);

    // if we write red channel first, the driver stops working
    // we don't know why.. :/
    i2cWriteReg(ledconf[row][col].g, color.g);
    i2cWriteReg(ledconf[row][col].r, color.r);
    i2cWriteReg(ledconf[row][col].b, color.b);
}