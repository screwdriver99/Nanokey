#ifndef HAL_H
#define HAL_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "types.h"

    //==========================
    void HALsetup(setupInstructions setup);

    void delayms(uint32_t ms);

    void deinitHAL();

    //==========GPIO============
    void writePin(uint8_t pin, bool state);

    bool readPin(uint8_t pin);

    //==========I2C=============
    void i2csetSlaveAddr(uint8_t addr);

    bool i2cWriteReg(uint8_t reg, uint8_t data);

    void i2cSelectBank(uint8_t bank);

    //==========RNG=============
    uint32_t getRandom();

    //=========KEYS=============
    void getKeys(keymap* km);

    //==========USB=============
    void usbSendKeys(report_keyboard_t* report);

    //==========UART============

    // Block-based interface:

    // Move the rxbuffer content into the given buffer and return the number of moved bytes.
    // The operation limits the number of moved bytes to 'size' bytes.
    // The operation clears the rxbuffer
    uint16_t uartRecvBlock(uint8_t* buf, uint16_t size);

    // Send out a block of data on the serial interface
    void uartSendBlock(uint8_t* buf, uint16_t size);

    // Char-based interface:

    // Retrieve a single character from the internal buffer and
    // advance the cursor to the next element. The character
    // will be put into c pointed char. If no chars are available
    // for reading, false will be returned
    bool uartRecvChar(uint8_t* c);

    //========BOOTLOADER========
    void JumpToBootloader();

#ifdef __cplusplus
}
#endif

#endif /* HAL_H */
