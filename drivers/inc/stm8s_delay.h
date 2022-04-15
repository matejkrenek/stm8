/**
 ****************************************************************************************************
 * @file    stm8s_delay.h
 * @author  Matěj Křenek
 * @version V1.0.0
 * @date    8-March-2020
 * @brief   This file contains all helper functions prototype and macros for the HELPERS peripheral.
 ***************************************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM8S_HELPERS_H
#define __STM8S_HELPERS_H

/* Includes ------------------------------------------------------------------*/
#include "stm8s.h"

/* Exported types ------------------------------------------------------------*/

/** @addtogroup HELPERS_Exported_Types
 * @{
 */
typedef struct
{
  void (*cycles)(uint32_t);
  void (*ms)(uint32_t);
  void (*us)(uint32_t);
} Delay_Module;

/**
 * @}
 */

/* Exported constants --------------------------------------------------------*/

/** @addtogroup HELPERS_Exported_Constants
 * @{
 */

/**
 * @}
 */

/* Exported macros -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/

/** @addtogroup HELPERS_Private_Macros
 * @{
 */

/**
 * @}
 */

/* Exported functions ------------------------------------------------------- */

/** @addtogroup HELPERS_Exported_Functions
 * @{
 */

void delay_cycles(uint32_t value);
void delay_ms(uint32_t ms);
void delay_us(uint32_t us);
extern const Delay_Module delay;

/**
 * @}
 */

#endif /* __STM8S_HELPERS_H */
