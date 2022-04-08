/**
  ******************************************************************************
  * @file    stm8s_delay.c
  * @author  Matěj Křenek
  * @version V1.0.0
  * @date    8-March-2020
  * @brief   This file contains all helper functions for the HELPERS peripheral.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm8s.h"

/** @addtogroup STM8S_StdPeriph_Driver
  * @{
  */
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/* Public functions ----------------------------------------------------------*/

/**
  * @addtogroup HELPERS_Public_Functions
  * @{
  */

void delay_cycles(uint32_t cycles) {
  for (uint32_t i = 0; i < cycles; i++);  
}

void delay_ms(uint32_t ms) {
    TIM4.init(TIM4_PRESCALER_128, 255);
    TIM4.enable();

    if (TIM4_GetFlagStatus(TIM4_FLAG_UPDATE) == SET)
    {
        TIM4_ClearFlag(TIM4_FLAG_UPDATE);
    }
}

const Delay_Module delay = {
  .cycles = delay_cycles,
  .ms = delay_ms,
};

/**
  * @}
  */
  
/**
  * @}
  */
  

/*****************************END OF FILE****/
