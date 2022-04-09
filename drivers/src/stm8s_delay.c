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

void delay_cycles(uint32_t value)
{
  uint32_t loops = (dly_const * value);

  while (loops)
  {
    _asm("nop");
    loops--;
  };
}

void delay_ms(uint32_t ms)
{
  while (ms)
  {
    delay.cycles(1000);
    ms--;
  };
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
