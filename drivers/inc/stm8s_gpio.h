/**
  ******************************************************************************
  * @file    stm8s_gpio.h
  * @author  MCD Application Team
  * @version V2.3.0
  * @date    16-June-2017
  * @brief   This file contains all functions prototype and macros for the GPIO peripheral.
   ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM8S_GPIO_H
#define __STM8S_GPIO_H

/* Includes ------------------------------------------------------------------*/
#include "stm8s.h"

/* Exported variables ------------------------------------------------------- */
/* Exported types ------------------------------------------------------------*/

/** @addtogroup GPIO_Exported_Types
  * @{
  */

/**
  * @brief  GPIO modes
  *
  * Bits definitions:
  * - Bit 7: 0 = INPUT mode
  *          1 = OUTPUT mode
  *          1 = PULL-UP (input) or PUSH-PULL (output)
  * - Bit 5: 0 = No external interrupt (input) or No slope control (output)
  *          1 = External interrupt (input) or Slow control enabled (output)
  * - Bit 4: 0 = Low level (output)
  *          1 = High level (output push-pull) or HI-Z (output open-drain)
  */
typedef enum
{
  INPUT_FL_NO_IT      = (uint8_t)0x00,  /*!< Input floating, no external interrupt */
  INPUT_PU_NO_IT      = (uint8_t)0x40,  /*!< Input pull-up, no external interrupt */
  INPUT_FL_IT         = (uint8_t)0x20,  /*!< Input floating, external interrupt */
  INPUT_PU_IT         = (uint8_t)0x60,  /*!< Input pull-up, external interrupt */
  OUTPUT_OD_LOW_FAST  = (uint8_t)0xA0,  /*!< Output open-drain, low level, 10MHz */
  OUTPUT_PP_LOW_FAST  = (uint8_t)0xE0,  /*!< Output push-pull, low level, 10MHz */
  OUTPUT_OD_LOW_SLOW  = (uint8_t)0x80,  /*!< Output open-drain, low level, 2MHz */
  OUTPUT_PP_LOW_SLOW  = (uint8_t)0xC0,  /*!< Output push-pull, low level, 2MHz */
  OUTPUT_OD_HIZ_FAST  = (uint8_t)0xB0,  /*!< Output open-drain, high-impedance level,10MHz */
  OUTPUT_PP_HIGH_FAST = (uint8_t)0xF0,  /*!< Output push-pull, high level, 10MHz */
  OUTPUT_OD_HIZ_SLOW  = (uint8_t)0x90,  /*!< Output open-drain, high-impedance level, 2MHz */
  OUTPUT_PP_HIGH_SLOW = (uint8_t)0xD0   /*!< Output push-pull, high level, 2MHz */
} GPIO_Mode;

/**
  * @brief  Definition of the GPIO pins. Used by the @ref GPIO_Init function in
  * order to select the pins to be initialized.
  */

typedef enum
{
  PIN_0    = ((uint8_t)0x01),  /*!< Pin 0 selected */
  PIN_1    = ((uint8_t)0x02),  /*!< Pin 1 selected */
  PIN_2    = ((uint8_t)0x04),  /*!< Pin 2 selected */
  PIN_3    = ((uint8_t)0x08),   /*!< Pin 3 selected */
  PIN_4    = ((uint8_t)0x10),  /*!< Pin 4 selected */
  PIN_5    = ((uint8_t)0x20),  /*!< Pin 5 selected */
  PIN_6    = ((uint8_t)0x40),  /*!< Pin 6 selected */
  PIN_7    = ((uint8_t)0x80),  /*!< Pin 7 selected */
  PIN_LNIB = ((uint8_t)0x0F),  /*!< Low nibble pins selected */
  PIN_HNIB = ((uint8_t)0xF0),  /*!< High nibble pins selected */
  PIN_ALL  = ((uint8_t)0xFF)   /*!< All pins selected */
} GPIO_Pin;



/**
  * @brief Definition on the GPIO Pin struct to easly work with pins
  */
typedef struct {
  GPIO_TypeDef* port;
  GPIO_Pin pin;
  uint8_t value, counter;
} Pin;

/**
  * @brief  Definition of the GPIO struct to work with ports.
  */

typedef struct {
    Pin* (*init)(GPIO_TypeDef*, GPIO_Pin, GPIO_Mode);
    void (*write)(Pin*, uint8_t);
    void (*writeLow)(Pin*);
    void (*writeHigh)(Pin*);
    void (*writeReverse)(Pin*);
    bool (*read)(Pin*);
    uint8_t (*readOutputData)(Pin*);
    uint8_t (*readInputData)(Pin*);
} GPIO_Module;


/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/

/** @addtogroup GPIO_Private_Macros
  * @{
  */

/**
  * @brief  Macro used by the assert function to check the different functions parameters.
  */

/**
  * @brief  Macro used by the assert function in order to check the different
  * values of GPIOMode_TypeDef.
  */
#define IS_GPIO_MODE(MODE) \
  (((MODE) == INPUT_FL_NO_IT)    || \
   ((MODE) == INPUT_PU_NO_IT)    || \
   ((MODE) == INPUT_FL_IT)       || \
   ((MODE) == INPUT_PU_IT)       || \
   ((MODE) == OUTPUT_OD_LOW_FAST)  || \
   ((MODE) == OUTPUT_PP_LOW_FAST)  || \
   ((MODE) == OUTPUT_OD_LOW_SLOW)  || \
   ((MODE) == OUTPUT_PP_LOW_SLOW)  || \
   ((MODE) == OUTPUT_OD_HIZ_FAST)  || \
   ((MODE) == OUTPUT_PP_HIGH_FAST)  || \
   ((MODE) == OUTPUT_OD_HIZ_SLOW)  || \
   ((MODE) == OUTPUT_PP_HIGH_SLOW))

/**
  * @brief  Macro used by the assert function in order to check the different
  * values of GPIO_Pins.
  */
#define IS_GPIO_PIN(PIN)  ((PIN) != (uint8_t)0x00)

/**
  * @}
  */

/* Exported functions ------------------------------------------------------- */
/** @addtogroup GPIO_Exported_Functions
  * @{
  */

void GPIO_DeInit(GPIO_TypeDef* GPIOx);
Pin* GPIO_Init(GPIO_TypeDef* GPIOx, GPIO_Pin GPIO_pin, GPIO_Mode Mode);
void GPIO_Write(Pin* pin, uint8_t PortVal);
void GPIO_WriteHigh(Pin* pin);
void GPIO_WriteLow(Pin* pin);
void GPIO_WriteReverse(Pin* pin);
uint8_t GPIO_ReadInputData(Pin* pin);
uint8_t GPIO_ReadOutputData(Pin* pin);
bool GPIO_ReadInputPin(Pin* pin);
void GPIO_ExternalPullUpConfig(GPIO_TypeDef* GPIOx, GPIO_Pin GPIO_Pin, FunctionalState NewState);
extern const GPIO_Module GPIO;
/**
  * @}
  */

#endif /* __STM8L_GPIO_H */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
