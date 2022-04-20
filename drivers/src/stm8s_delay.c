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
#include "stm8s_delay.h"

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
  for (uint32_t i = 0; i < value; i++)
  {
    nop();
  }
}

void delay_ms(uint32_t ms)
{
  uint32_t clock_freq = CLK_GetClockFreq();

  for (uint32_t i = 0; i < ((clock_freq / 18 / 1000UL) * ms); i++)
  {
    nop();
  }
}

void delay_us(uint32_t us)
{
  uint32_t clock_freq = CLK_GetClockFreq();

  for (uint32_t i = 0; i < ((clock_freq / 18 / 1000UL) * (us / 1000)); i++)
  {
    nop();
  }
}

const Delay_Module delay = {
    .cycles = delay_cycles,
    .ms = delay_ms,
    .us = delay_us,
};

/**
 * @}
 */

/**
 * @}
 */

/*****************************END OF FILE****/
