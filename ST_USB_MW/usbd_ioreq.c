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
#include "usbd_ioreq.h"

USBD_StatusTypeDef USBD_CtlSendData(USBD_HandleTypeDef *pdev, uint8_t *pbuf, uint32_t len)
{
    /* Set EP0 State */
    pdev->ep0_state             = USBD_EP0_DATA_IN;
    pdev->ep_in[0].total_length = len;

#ifdef USBD_AVOID_PACKET_SPLIT_MPS
    pdev->ep_in[0].rem_length = 0U;
#else
    pdev->ep_in[0].rem_length = len;
#endif /* USBD_AVOID_PACKET_SPLIT_MPS */

    /* Start the transfer */
    (void)USBD_LL_Transmit(pdev, 0x00U, pbuf, len);

    return USBD_OK;
}

USBD_StatusTypeDef USBD_CtlContinueSendData(USBD_HandleTypeDef *pdev, uint8_t *pbuf, uint32_t len)
{
    /* Start the next transfer */
    (void)USBD_LL_Transmit(pdev, 0x00U, pbuf, len);

    return USBD_OK;
}

USBD_StatusTypeDef USBD_CtlPrepareRx(USBD_HandleTypeDef *pdev, uint8_t *pbuf, uint32_t len)
{
    /* Set EP0 State */
    pdev->ep0_state              = USBD_EP0_DATA_OUT;
    pdev->ep_out[0].total_length = len;

#ifdef USBD_AVOID_PACKET_SPLIT_MPS
    pdev->ep_out[0].rem_length = 0U;
#else
    pdev->ep_out[0].rem_length = len;
#endif /* USBD_AVOID_PACKET_SPLIT_MPS */

    /* Start the transfer */
    (void)USBD_LL_PrepareReceive(pdev, 0U, pbuf, len);

    return USBD_OK;
}

USBD_StatusTypeDef USBD_CtlContinueRx(USBD_HandleTypeDef *pdev, uint8_t *pbuf, uint32_t len)
{
    (void)USBD_LL_PrepareReceive(pdev, 0U, pbuf, len);

    return USBD_OK;
}

USBD_StatusTypeDef USBD_CtlSendStatus(USBD_HandleTypeDef *pdev)
{
    /* Set EP0 State */
    pdev->ep0_state = USBD_EP0_STATUS_IN;

    /* Start the transfer */
    (void)USBD_LL_Transmit(pdev, 0x00U, NULL, 0U);

    return USBD_OK;
}

USBD_StatusTypeDef USBD_CtlReceiveStatus(USBD_HandleTypeDef *pdev)
{
    /* Set EP0 State */
    pdev->ep0_state = USBD_EP0_STATUS_OUT;

    /* Start the transfer */
    (void)USBD_LL_PrepareReceive(pdev, 0U, NULL, 0U);

    return USBD_OK;
}

uint32_t USBD_GetRxCount(USBD_HandleTypeDef *pdev, uint8_t ep_addr)
{
    return USBD_LL_GetRxDataSize(pdev, ep_addr);
}
