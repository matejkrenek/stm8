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

void delay_cycles(uint32_t cycles) {
  for (uint32_t i = 0; i < cycles; i++);  
}

const Delay_Module delay = {
  .cycles = delay_cycles,
};

/**
  * @}
  */
  
/**
  * @}
  */
  

/*****************************END OF FILE****/
