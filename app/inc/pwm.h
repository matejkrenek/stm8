/**
  ****************************************************************************************************
  * @file    pwm.h
  * @author  Matěj Křenek
  * @version V1.0.0
  * @date    8-March-2020
  * @brief   This file contains all helper functions prototype and macros for the PWM peripheral.
   ***************************************************************************************************
  */


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PWM_H
#define __PWM_H

/* Includes ------------------------------------------------------------------*/
#include "stm8s.h"

/* Exported types ------------------------------------------------------------*/

/** @addtogroup PWM_Exported_Types
  * @{
  */


typedef struct {
  Pin* (*create)(GPIO_TypeDef*, GPIO_Pin, uint8_t);
  void (*modulate)(Pin*);
} PWM_Module;

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

Pin* PWM_Create(GPIO_TypeDef* port, GPIO_Pin pin, uint8_t pwm_value);
void PWM_Modulate(Pin* pwm_pin);
extern const PWM_Module PWM;

/**
  * @}
  */

#endif /* __PWM_H */
