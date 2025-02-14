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
#ifndef __USBD_CONF__H__
#define __USBD_CONF__H__

#ifdef __cplusplus
extern "C"
{
#endif

    // #include <stdio.h>
    // #include <stdlib.h>
    // #include <string.h>
#include <stdint.h>

#include "main.h"
#include "stm32l4xx.h"
#include "stm32l4xx_hal.h"

#define USBD_MAX_NUM_INTERFACES 1U
#define USBD_MAX_NUM_CONFIGURATION 1U
#define USBD_MAX_STR_DESC_SIZ 512U
#define USBD_LPM_ENABLED 1U
#define USBD_SELF_POWERED 1U
#define HID_FS_BINTERVAL 0x01U
#define DEVICE_FS 0

#define USBD_malloc (void *)USBD_static_malloc
#define USBD_free USBD_static_free
#define USBD_memset memset
#define USBD_memcpy memcpy
#define USBD_Delay HAL_Delay

    void *USBD_static_malloc(uint32_t size);
    void USBD_static_free(void *p);

#ifdef __cplusplus
}
#endif

#endif /* __USBD_CONF__H__ */
