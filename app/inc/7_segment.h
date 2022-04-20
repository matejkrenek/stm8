/**
 ****************************************************************************************************
 * @file    7_segment.h
 * @author  Matěj Křenek
 * @version V1.0.0
 * @date    8-March-2020
 * @brief   This file contains all helper functions prototype and macros for the PWM peripheral.
 ***************************************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SEVEN_SEGMENT_H
#define __SEVEN_SEGMENT_H

/* Includes ------------------------------------------------------------------*/
#include "stm8s.h"

/* Exported types ------------------------------------------------------------*/

/** @addtogroup PWM_Exported_Types
 * @{
 */

typedef struct
{
  Pin *(*init)(Pin *, uint8_t);
  void (*write)(Pin *, uint8_t);
} Seven_Segment_Module;

/**
 * @}
 */

/* Exported constants --------------------------------------------------------*/

/** @addtogroup PWM_Exported_Constants
 * @{
 */

/**
 * @}
 */

/* Exported macros -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/

/** @addtogroup PWM_Private_Macros
 * @{
 */

/**
 * @}
 */

/* Exported functions ------------------------------------------------------- */

/** @addtogroup PWM_Exported_Functions
 * @{
 */

Pin *Seven_Segment_Init(Pin *, uint8_t);
void Seven_Segment_Write(Pin *, uint8_t);
extern const Seven_Segment_Module SevenSegment;

/**
 * @}
 */

#endif /* __PWM_H */
