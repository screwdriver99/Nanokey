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
#ifndef __USB_REQUEST_H
#define __USB_REQUEST_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "usbd_def.h"

    USBD_StatusTypeDef USBD_StdDevReq(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);
    USBD_StatusTypeDef USBD_StdItfReq(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);
    USBD_StatusTypeDef USBD_StdEPReq(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);

    void USBD_CtlError(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);
    void USBD_ParseSetupRequest(USBD_SetupReqTypedef *req, uint8_t *pdata);
    void USBD_GetString(uint8_t *desc, uint8_t *unicode, uint16_t *len);

#ifdef __cplusplus
}
#endif

#endif /* __USB_REQUEST_H */
