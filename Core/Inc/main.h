/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

    /* Private includes ----------------------------------------------------------*/
    /* USER CODE BEGIN Includes */

    /* USER CODE END Includes */

    /* Exported types ------------------------------------------------------------*/
    /* USER CODE BEGIN ET */

    /* USER CODE END ET */

    /* Exported constants --------------------------------------------------------*/
    /* USER CODE BEGIN EC */

    /* USER CODE END EC */

    /* Exported macro ------------------------------------------------------------*/
    /* USER CODE BEGIN EM */

    /* USER CODE END EM */

    /* Exported functions prototypes ---------------------------------------------*/
    void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LEDSD_Pin GPIO_PIN_14
#define LEDSD_GPIO_Port GPIOC
#define SRDS_Pin GPIO_PIN_15
#define SRDS_GPIO_Port GPIOC
#define STCP_Pin GPIO_PIN_0
#define STCP_GPIO_Port GPIOA
#define SHCP_Pin GPIO_PIN_1
#define SHCP_GPIO_Port GPIOA
#define BATLOW_Pin GPIO_PIN_4
#define BATLOW_GPIO_Port GPIOA
#define WUP_BTMOD_Pin GPIO_PIN_5
#define WUP_BTMOD_GPIO_Port GPIOA
#define WUP_MCU_Pin GPIO_PIN_6
#define WUP_MCU_GPIO_Port GPIOA
#define CAPS_Pin GPIO_PIN_7
#define CAPS_GPIO_Port GPIOA
#define COL_0_Pin GPIO_PIN_0
#define COL_0_GPIO_Port GPIOB
#define BATPOW_Pin GPIO_PIN_1
#define BATPOW_GPIO_Port GPIOB
#define MACMODE_Pin GPIO_PIN_8
#define MACMODE_GPIO_Port GPIOA
#define BTRESET_Pin GPIO_PIN_9
#define BTRESET_GPIO_Port GPIOA
#define BTMODE_Pin GPIO_PIN_10
#define BTMODE_GPIO_Port GPIOA
#define ROW_5_Pin GPIO_PIN_13
#define ROW_5_GPIO_Port GPIOA
#define ROW_4_Pin GPIO_PIN_14
#define ROW_4_GPIO_Port GPIOA
#define ROW_3_Pin GPIO_PIN_15
#define ROW_3_GPIO_Port GPIOA
#define ROW_2_Pin GPIO_PIN_3
#define ROW_2_GPIO_Port GPIOB
#define ROW_1_Pin GPIO_PIN_4
#define ROW_1_GPIO_Port GPIOB
#define ROW_0_Pin GPIO_PIN_5
#define ROW_0_GPIO_Port GPIOB
#define BTLED_Pin GPIO_PIN_3
#define BTLED_GPIO_Port GPIOH

    /* USER CODE BEGIN Private defines */

    /* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
