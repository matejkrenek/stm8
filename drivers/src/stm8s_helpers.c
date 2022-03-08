/**
  ******************************************************************************
  * @file    stm8s_helpers.c
  * @author  Matěj Křenek
  * @version V1.0.0
  * @date    8-March-2020
  * @brief   This file contains all helper functions for the HELPERS peripheral.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm8s_helpers.h"

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

void delay(uint32_t iterations) {
    for (uint32_t i = 0; i < iterations; i++);
}

/**
  * @}
  */
  
/**
  * @}
  */
  

/*****************************END OF FILE****/
