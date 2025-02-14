/*
 * Copyright (c) 2017 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
#include "../keyboardhid.h"
#include "stm32l4xx.h"
#include "stm32l4xx_hal.h"
#include "usbd_core.h"
#include "usbd_def.h"

void Error_Handler(void);

extern USBD_StatusTypeDef USBD_LL_BatteryCharging(USBD_HandleTypeDef *pdev);

static void SystemClockConfig_Resume(void);

extern void SystemClock_Config(void);

/*******************************************************************************
                       LL Driver Callbacks (PCD -> USB Device Library)
*******************************************************************************/

// void HAL_PCD_MspInit(PCD_HandleTypeDef* pcdHandle)
// {
//   if(pcdHandle->Instance==USB)
//   {
//     __HAL_RCC_USB_CLK_ENABLE();

//     // Enable VDDUSB
//     if(__HAL_RCC_PWR_IS_CLK_DISABLED())
//     {
//       __HAL_RCC_PWR_CLK_ENABLE();
//       HAL_PWREx_EnableVddUSB();
//       __HAL_RCC_PWR_CLK_DISABLE();
//     }
//     else HAL_PWREx_EnableVddUSB();

//     HAL_NVIC_SetPriority(USB_IRQn, 0, 0);
//     HAL_NVIC_EnableIRQ(USB_IRQn);
//   }
// }

// void HAL_PCD_MspDeInit(PCD_HandleTypeDef* pcdHandle)
// {
//   if(pcdHandle->Instance==USB)
//   {
//     __HAL_RCC_USB_CLK_DISABLE();
//     HAL_NVIC_DisableIRQ(USB_IRQn);
//   }
// }

void HAL_PCD_SetupStageCallback(PCD_HandleTypeDef *hpcd)
{
    USBD_LL_SetupStage((USBD_HandleTypeDef *)hpcd->pData, (uint8_t *)hpcd->Setup);
}

void HAL_PCD_DataOutStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
{
    USBD_LL_DataOutStage((USBD_HandleTypeDef *)hpcd->pData, epnum, hpcd->OUT_ep[epnum].xfer_buff);
}

void HAL_PCD_DataInStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
{
    USBD_LL_DataInStage((USBD_HandleTypeDef *)hpcd->pData, epnum, hpcd->IN_ep[epnum].xfer_buff);
}

void HAL_PCD_SOFCallback(PCD_HandleTypeDef *hpcd) { USBD_LL_SOF((USBD_HandleTypeDef *)hpcd->pData); }

void HAL_PCD_ResetCallback(PCD_HandleTypeDef *hpcd)
{
    USBD_SpeedTypeDef speed = USBD_SPEED_FULL;

    if (hpcd->Init.speed != PCD_SPEED_FULL)
    {
        Error_Handler();
    }
    /* Set Speed. */
    USBD_LL_SetSpeed((USBD_HandleTypeDef *)hpcd->pData, speed);

    /* Reset Device. */
    USBD_LL_Reset((USBD_HandleTypeDef *)hpcd->pData);
}

void HAL_PCD_SuspendCallback(PCD_HandleTypeDef *hpcd)
{
    /* Inform USB library that core enters in suspend Mode. */
    USBD_LL_Suspend((USBD_HandleTypeDef *)hpcd->pData);
    /* Enter in STOP mode. */
    /* USER CODE BEGIN 2 */
    if (hpcd->Init.low_power_enable)
    {
        /* Set SLEEPDEEP bit and SleepOnExit of Cortex System Control Register. */
        SCB->SCR |= (uint32_t)((uint32_t)(SCB_SCR_SLEEPDEEP_Msk | SCB_SCR_SLEEPONEXIT_Msk));
    }
    /* USER CODE END 2 */
}

void HAL_PCD_ResumeCallback(PCD_HandleTypeDef *hpcd)
{
    /* USER CODE BEGIN 3 */
    if (hpcd->Init.low_power_enable)
    {
        /* Reset SLEEPDEEP bit of Cortex System Control Register. */
        SCB->SCR &= (uint32_t) ~((uint32_t)(SCB_SCR_SLEEPDEEP_Msk | SCB_SCR_SLEEPONEXIT_Msk));
        SystemClockConfig_Resume();
    }
    /* USER CODE END 3 */
    USBD_LL_Resume((USBD_HandleTypeDef *)hpcd->pData);
}

void HAL_PCD_ISOOUTIncompleteCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
{
    USBD_LL_IsoOUTIncomplete((USBD_HandleTypeDef *)hpcd->pData, epnum);
}

void HAL_PCD_ISOINIncompleteCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
{
    USBD_LL_IsoINIncomplete((USBD_HandleTypeDef *)hpcd->pData, epnum);
}

void HAL_PCD_ConnectCallback(PCD_HandleTypeDef *hpcd)
{
    USBD_LL_DevConnected((USBD_HandleTypeDef *)hpcd->pData);
}

void HAL_PCD_DisconnectCallback(PCD_HandleTypeDef *hpcd)
{
    USBD_LL_DevDisconnected((USBD_HandleTypeDef *)hpcd->pData);
}

/*******************************************************************************
                LL Driver Interface (USB Device Library --> PCD)
*******************************************************************************/

USBD_StatusTypeDef USBD_LL_Init(USBD_HandleTypeDef *pdev)
{
    /* Init USB Ip. */
    /* Enable USB power on Pwrctrl CR2 register. */
    HAL_PWREx_EnableVddUSB();

    // PCD_HandleTypeDef* pcd = (PCD_HandleTypeDef*) pdev->pData;

    // pcd->Instance = USB;
    // pcd->Init.dev_endpoints = 8;
    // pcd->Init.speed = PCD_SPEED_FULL;
    // pcd->Init.phy_itface = PCD_PHY_EMBEDDED;
    // pcd->Init.Sof_enable = DISABLE;
    // pcd->Init.low_power_enable = DISABLE;
    // pcd->Init.lpm_enable = DISABLE;
    // pcd->Init.battery_charging_enable = DISABLE;

    // if (HAL_PCD_Init(pcd) != HAL_OK)
    // {
    //   Error_Handler();
    // }

    HAL_PCDEx_PMAConfig((PCD_HandleTypeDef *)pdev->pData, 0x00, PCD_SNG_BUF, 0x18);
    HAL_PCDEx_PMAConfig((PCD_HandleTypeDef *)pdev->pData, 0x80, PCD_SNG_BUF, 0x58);

    // HID
    HAL_PCDEx_PMAConfig((PCD_HandleTypeDef *)pdev->pData, 0x81, PCD_SNG_BUF, 0x100);
    return USBD_OK;
}

USBD_StatusTypeDef USBD_LL_DeInit(USBD_HandleTypeDef *pdev)
{
    HAL_StatusTypeDef hal_status  = HAL_OK;
    USBD_StatusTypeDef usb_status = USBD_OK;

    hal_status = HAL_PCD_DeInit(pdev->pData);

    switch (hal_status)
    {
        case HAL_OK:
            usb_status = USBD_OK;
            break;
        case HAL_ERROR:
            usb_status = USBD_FAIL;
            break;
        case HAL_BUSY:
            usb_status = USBD_BUSY;
            break;
        case HAL_TIMEOUT:
            usb_status = USBD_FAIL;
            break;
        default:
            usb_status = USBD_FAIL;
            break;
    }
    return usb_status;
}

USBD_StatusTypeDef USBD_LL_Start(USBD_HandleTypeDef *pdev)
{
    HAL_StatusTypeDef hal_status  = HAL_OK;
    USBD_StatusTypeDef usb_status = USBD_OK;

    hal_status = HAL_PCD_Start(pdev->pData);

    switch (hal_status)
    {
        case HAL_OK:
            usb_status = USBD_OK;
            break;
        case HAL_ERROR:
            usb_status = USBD_FAIL;
            break;
        case HAL_BUSY:
            usb_status = USBD_BUSY;
            break;
        case HAL_TIMEOUT:
            usb_status = USBD_FAIL;
            break;
        default:
            usb_status = USBD_FAIL;
            break;
    }
    return usb_status;
}

USBD_StatusTypeDef USBD_LL_Stop(USBD_HandleTypeDef *pdev)
{
    HAL_StatusTypeDef hal_status  = HAL_OK;
    USBD_StatusTypeDef usb_status = USBD_OK;

    hal_status = HAL_PCD_Stop(pdev->pData);

    switch (hal_status)
    {
        case HAL_OK:
            usb_status = USBD_OK;
            break;
        case HAL_ERROR:
            usb_status = USBD_FAIL;
            break;
        case HAL_BUSY:
            usb_status = USBD_BUSY;
            break;
        case HAL_TIMEOUT:
            usb_status = USBD_FAIL;
            break;
        default:
            usb_status = USBD_FAIL;
            break;
    }
    return usb_status;
}

USBD_StatusTypeDef USBD_LL_OpenEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr, uint8_t ep_type, uint16_t ep_mps)
{
    HAL_StatusTypeDef hal_status  = HAL_OK;
    USBD_StatusTypeDef usb_status = USBD_OK;

    hal_status = HAL_PCD_EP_Open(pdev->pData, ep_addr, ep_mps, ep_type);

    switch (hal_status)
    {
        case HAL_OK:
            usb_status = USBD_OK;
            break;
        case HAL_ERROR:
            usb_status = USBD_FAIL;
            break;
        case HAL_BUSY:
            usb_status = USBD_BUSY;
            break;
        case HAL_TIMEOUT:
            usb_status = USBD_FAIL;
            break;
        default:
            usb_status = USBD_FAIL;
            break;
    }
    return usb_status;
}

USBD_StatusTypeDef USBD_LL_CloseEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr)
{
    HAL_StatusTypeDef hal_status  = HAL_OK;
    USBD_StatusTypeDef usb_status = USBD_OK;

    hal_status = HAL_PCD_EP_Close(pdev->pData, ep_addr);

    switch (hal_status)
    {
        case HAL_OK:
            usb_status = USBD_OK;
            break;
        case HAL_ERROR:
            usb_status = USBD_FAIL;
            break;
        case HAL_BUSY:
            usb_status = USBD_BUSY;
            break;
        case HAL_TIMEOUT:
            usb_status = USBD_FAIL;
            break;
        default:
            usb_status = USBD_FAIL;
            break;
    }
    return usb_status;
}

USBD_StatusTypeDef USBD_LL_FlushEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr)
{
    HAL_StatusTypeDef hal_status  = HAL_OK;
    USBD_StatusTypeDef usb_status = USBD_OK;

    hal_status = HAL_PCD_EP_Flush(pdev->pData, ep_addr);

    switch (hal_status)
    {
        case HAL_OK:
            usb_status = USBD_OK;
            break;
        case HAL_ERROR:
            usb_status = USBD_FAIL;
            break;
        case HAL_BUSY:
            usb_status = USBD_BUSY;
            break;
        case HAL_TIMEOUT:
            usb_status = USBD_FAIL;
            break;
        default:
            usb_status = USBD_FAIL;
            break;
    }
    return usb_status;
}

USBD_StatusTypeDef USBD_LL_StallEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr)
{
    HAL_StatusTypeDef hal_status  = HAL_OK;
    USBD_StatusTypeDef usb_status = USBD_OK;

    hal_status = HAL_PCD_EP_SetStall(pdev->pData, ep_addr);

    switch (hal_status)
    {
        case HAL_OK:
            usb_status = USBD_OK;
            break;
        case HAL_ERROR:
            usb_status = USBD_FAIL;
            break;
        case HAL_BUSY:
            usb_status = USBD_BUSY;
            break;
        case HAL_TIMEOUT:
            usb_status = USBD_FAIL;
            break;
        default:
            usb_status = USBD_FAIL;
            break;
    }
    return usb_status;
}

USBD_StatusTypeDef USBD_LL_ClearStallEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr)
{
    HAL_StatusTypeDef hal_status  = HAL_OK;
    USBD_StatusTypeDef usb_status = USBD_OK;

    hal_status = HAL_PCD_EP_ClrStall(pdev->pData, ep_addr);

    switch (hal_status)
    {
        case HAL_OK:
            usb_status = USBD_OK;
            break;
        case HAL_ERROR:
            usb_status = USBD_FAIL;
            break;
        case HAL_BUSY:
            usb_status = USBD_BUSY;
            break;
        case HAL_TIMEOUT:
            usb_status = USBD_FAIL;
            break;
        default:
            usb_status = USBD_FAIL;
            break;
    }
    return usb_status;
}

uint8_t USBD_LL_IsStallEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr)
{
    PCD_HandleTypeDef *hpcd = (PCD_HandleTypeDef *)pdev->pData;

    if ((ep_addr & 0x80) == 0x80)
    {
        return hpcd->IN_ep[ep_addr & 0x7F].is_stall;
    }
    else
    {
        return hpcd->OUT_ep[ep_addr & 0x7F].is_stall;
    }
}

USBD_StatusTypeDef USBD_LL_SetUSBAddress(USBD_HandleTypeDef *pdev, uint8_t dev_addr)
{
    HAL_StatusTypeDef hal_status  = HAL_OK;
    USBD_StatusTypeDef usb_status = USBD_OK;

    hal_status = HAL_PCD_SetAddress(pdev->pData, dev_addr);

    switch (hal_status)
    {
        case HAL_OK:
            usb_status = USBD_OK;
            break;
        case HAL_ERROR:
            usb_status = USBD_FAIL;
            break;
        case HAL_BUSY:
            usb_status = USBD_BUSY;
            break;
        case HAL_TIMEOUT:
            usb_status = USBD_FAIL;
            break;
        default:
            usb_status = USBD_FAIL;
            break;
    }
    return usb_status;
}

USBD_StatusTypeDef USBD_LL_Transmit(USBD_HandleTypeDef *pdev, uint8_t ep_addr, uint8_t *pbuf, uint32_t size)
{
    HAL_StatusTypeDef hal_status  = HAL_OK;
    USBD_StatusTypeDef usb_status = USBD_OK;

    hal_status = HAL_PCD_EP_Transmit(pdev->pData, ep_addr, pbuf, size);

    switch (hal_status)
    {
        case HAL_OK:
            usb_status = USBD_OK;
            break;
        case HAL_ERROR:
            usb_status = USBD_FAIL;
            break;
        case HAL_BUSY:
            usb_status = USBD_BUSY;
            break;
        case HAL_TIMEOUT:
            usb_status = USBD_FAIL;
            break;
        default:
            usb_status = USBD_FAIL;
            break;
    }
    return usb_status;
}

USBD_StatusTypeDef USBD_LL_PrepareReceive(USBD_HandleTypeDef *pdev, uint8_t ep_addr, uint8_t *pbuf,
                                          uint32_t size)
{
    HAL_StatusTypeDef hal_status  = HAL_OK;
    USBD_StatusTypeDef usb_status = USBD_OK;

    hal_status = HAL_PCD_EP_Receive(pdev->pData, ep_addr, pbuf, size);

    switch (hal_status)
    {
        case HAL_OK:
            usb_status = USBD_OK;
            break;
        case HAL_ERROR:
            usb_status = USBD_FAIL;
            break;
        case HAL_BUSY:
            usb_status = USBD_BUSY;
            break;
        case HAL_TIMEOUT:
            usb_status = USBD_FAIL;
            break;
        default:
            usb_status = USBD_FAIL;
            break;
    }
    return usb_status;
}

uint32_t USBD_LL_GetRxDataSize(USBD_HandleTypeDef *pdev, uint8_t ep_addr)
{
    return HAL_PCD_EP_GetRxCount((PCD_HandleTypeDef *)pdev->pData, ep_addr);
}

void HAL_PCDEx_LPM_Callback(PCD_HandleTypeDef *hpcd, PCD_LPM_MsgTypeDef msg)
{
    switch (msg)
    {
        case PCD_LPM_L0_ACTIVE:
            if (hpcd->Init.low_power_enable)
            {
                SystemClockConfig_Resume();

                /* Reset SLEEPDEEP bit of Cortex System Control Register. */
                SCB->SCR &= (uint32_t) ~((uint32_t)(SCB_SCR_SLEEPDEEP_Msk | SCB_SCR_SLEEPONEXIT_Msk));
            }
            USBD_LL_Resume(hpcd->pData);
            break;

        case PCD_LPM_L1_ACTIVE:
            USBD_LL_Suspend(hpcd->pData);

            /* Enter in STOP mode. */
            if (hpcd->Init.low_power_enable)
            {
                /* Set SLEEPDEEP bit and SleepOnExit of Cortex System Control Register. */
                SCB->SCR |= (uint32_t)((uint32_t)(SCB_SCR_SLEEPDEEP_Msk | SCB_SCR_SLEEPONEXIT_Msk));
            }
            break;
    }
}

void USBD_LL_Delay(uint32_t Delay) { HAL_Delay(Delay); }

void *USBD_static_malloc(uint32_t size)
{
    (void)size;
    static uint32_t mem[(sizeof(USBD_HID_HandleTypeDef) / 4) + 1]; /* On 32-bit boundary */
    return mem;
}

void USBD_static_free(void *p) { (void)p; }

static void SystemClockConfig_Resume(void) { SystemClock_Config(); }
