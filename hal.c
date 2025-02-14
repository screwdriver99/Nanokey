#include "hal.h"

#include "cmsis_gcc.h"
#include "keyboardhid.h"
#include "stm32l4xx_hal_i2c.h"
#include "stm32l4xx_hal_pcd.h"
#include "stm32l4xx_hal_rng.h"
#include "stm32l4xx_hal_uart.h"
#include "types.h"

#define TXRX_BUF_SIZE 128

static setupInstructions _setup;
uint8_t rxBuf[TXRX_BUF_SIZE];
uint8_t txBuf[TXRX_BUF_SIZE];
uint16_t rxBufSize   = 0;
uint16_t txBufSize   = 0;
uint8_t rxByte       = 0;
uint16_t rxBufCursor = 0;
bool txInProgress;

void clearBuf();
void rxEnable();

//=======================GENERAL===========================

void HALsetup(setupInstructions setup)
{
    _setup = setup;

    clearBuf();
    rxEnable();
}

void delayms(uint32_t ms) { HAL_Delay(ms); }

void deinitHAL()
{
    HAL_I2C_DeInit(_setup.i2ch);
    HAL_RNG_DeInit(_setup.rngh);
    HAL_PCD_DeInit((PCD_HandleTypeDef*)(_setup.usbdh->pData));
    HAL_UART_DeInit(_setup.uarth);
}

//=========================GPIO============================

void writePin(uint8_t pin, bool state)
{
    state = syspins[pin].inverted ? !state : state;
    HAL_GPIO_WritePin(syspins[pin].port, syspins[pin].pin, state ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

bool readPin(uint8_t pin)
{
    return HAL_GPIO_ReadPin(syspins[pin].port, syspins[pin].pin) ==
           (syspins[pin].inverted ? GPIO_PIN_RESET : GPIO_PIN_SET);
}

//===========================I2C============================

static uint8_t i2cHALSlaveAddr;

void i2csetSlaveAddr(uint8_t addr) { i2cHALSlaveAddr = addr; }

bool i2cWriteReg(uint8_t reg, uint8_t data)
{
    uint8_t d[2] = {reg, data};
    return HAL_I2C_Master_Transmit(_setup.i2ch, i2cHALSlaveAddr << 1, &d[0], 2, 100) == HAL_OK;
}

void i2cSelectBank(uint8_t bank) { i2cWriteReg(0xFD, bank); }

//========================RANDOM============================

uint32_t getRandom()
{
    uint32_t ret = 0;
    HAL_RNG_GenerateRandomNumber(_setup.rngh, &ret);
    return ret;
}

//==========================KEYS============================

// one unit is approx 100ns @ 80MHz
void _delay(uint16_t n)
{
    while (n-- > 0)
    {
        asm volatile("nop" ::: "memory");
    }
}

void clock()
{
    writePin(SRCLK, true);
    _delay(50);
    writePin(SRCLK, false);
    _delay(50);
}

void show()
{
    // clock on the SR storage clock pin
    writePin(SRSTG, true);
    _delay(50);
    writePin(SRSTG, false);
    _delay(50);
}

void selectFirstCol()
{
    writePin(SRDAT, true);
    writePin(COL_0, true);
    _delay(10);

    // set the whole shift register
    for (uint8_t i = 0; i < 16; i++) clock();

    // clock in one '0'
    writePin(SRDAT, false);
    _delay(100);
    clock();
    writePin(SRDAT, true);

    _delay(50);
    show();
}

void nextCol()
{
    clock();
    _delay(10);
    show();
}

void setBit(uint32_t* data, uint8_t index, bool state)
{
    if (state)
        *data |= (0x00000001) << index;
    else
        *data &= ~((0x00000001) << index);
}

void getKeys(keymap* km)
{
    selectFirstCol();

    for (uint8_t i = 1; i < 17; i++)
    {
        setBit(&(km->row[0]), i, readPin(ROW_0));
        setBit(&(km->row[1]), i, readPin(ROW_1));
        setBit(&(km->row[2]), i, readPin(ROW_2));
        setBit(&(km->row[3]), i, readPin(ROW_3));
        setBit(&(km->row[4]), i, readPin(ROW_4));
        setBit(&(km->row[5]), i, readPin(ROW_5));

        nextCol();
        _delay(50);
    }

    writePin(COL_0, false);
    _delay(10);

    setBit(&(km->row[0]), 0, readPin(ROW_0));
    setBit(&(km->row[1]), 0, readPin(ROW_1));
    setBit(&(km->row[2]), 0, readPin(ROW_2));
    setBit(&(km->row[3]), 0, readPin(ROW_3));
    setBit(&(km->row[4]), 0, readPin(ROW_4));
    setBit(&(km->row[5]), 0, readPin(ROW_5));

    writePin(COL_0, true);
}

//========================USB===============================

void usbSendKeys(report_keyboard_t* report)
{
    USBD_HID_SendReport(_setup.usbdh, (uint8_t*)(&(report->std)), 8);
}

//========================UART==============================

void wwait()
{
    uint32_t c = 250000;
    while (c) c--;
}

void rxEnable() { HAL_UART_Receive_IT(_setup.uarth, &rxByte, 1); }

// Interrupt handler for receive one byte and manage rx buffer.
// This function is called by the ST UART stack
void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart)
{
    (void)huart;
    if (rxBufSize == TXRX_BUF_SIZE) clearBuf();  // limit reached, auto-clear
    rxBuf[rxBufSize] = rxByte;
    rxBufSize++;
    rxEnable();
}

// Interrupt handler to signal completed transmissions.
// This function is called by the ST UART stack
void HAL_UART_TxCpltCallback(UART_HandleTypeDef* huart)
{
    (void)huart;
    txInProgress = false;
}

uint16_t uartRecvBlock(uint8_t* buf, uint16_t size)
{
    // enter critical section
    HAL_NVIC_DisableIRQ(USART2_IRQn);

    uint16_t s = (rxBufSize < size ? rxBufSize : size);

    for (uint16_t i = 0; i < s; i++) buf[i] = rxBuf[i];

    clearBuf();

    // exit critical section
    HAL_NVIC_EnableIRQ(USART2_IRQn);

    return s;
}

void uartSendBlock(uint8_t* buf, uint16_t size)
{
    if (size == 0) return;

    txBufSize = (TXRX_BUF_SIZE < size ? TXRX_BUF_SIZE : size);

    for (uint16_t i = 0; i < txBufSize; i++) txBuf[i] = buf[i];

    txInProgress = true;

    HAL_UART_Transmit_IT(_setup.uarth, txBuf, txBufSize);

    while (txInProgress) __NOP();
}

bool uartRecvChar(uint8_t* c)
{
    // enter critical section
    HAL_NVIC_DisableIRQ(USART2_IRQn);

    if (!rxBufSize)
    {
        // exit critical section
        HAL_NVIC_EnableIRQ(USART2_IRQn);
        return false;
    }

    *c = rxBuf[rxBufCursor];
    rxBufCursor++;

    if (rxBufCursor == rxBufSize) clearBuf();  // autoclear to save time

    // exit critical section
    HAL_NVIC_EnableIRQ(USART2_IRQn);

    return true;
}

void clearBuf()
{
    for (int i = 0; i < TXRX_BUF_SIZE; i++) rxBuf[i] = 0;
    rxBufSize   = 0;
    rxBufCursor = 0;
}

//==========================================================

#define BOOT_ADDR 0x1FFF0000
#define MCU_IRQS 70u

struct boot_vectable_
{
    uint32_t Initial_SP;
    void (*Reset_Handler)(void);
};

#define BOOTVTAB ((struct boot_vectable_*)BOOT_ADDR)

void JumpToBootloader()
{
    /* Disable all interrupts */
    __disable_irq();

    // Reset USB
    USB->CNTR = 0x0003;

    deinitHAL();

    /* Disable Systick timer */
    SysTick->CTRL = 0;

    /* Set the clock to the default state */
    HAL_RCC_DeInit();

    /* Clear Interrupt Enable Register & Interrupt Pending Register */
    for (uint8_t i = 0; i < (MCU_IRQS + 31u) / 32; i++)
    {
        NVIC->ICER[i] = 0xFFFFFFFF;
        NVIC->ICPR[i] = 0xFFFFFFFF;
    }

    /* Re-enable all interrupts */
    __enable_irq();

    // Set the MSP
    __set_MSP(BOOTVTAB->Initial_SP);

    // Jump to app firmware
    BOOTVTAB->Reset_Handler();
}
