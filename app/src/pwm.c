/**
  ******************************************************************************
  * @file    stm8s_pwm.c
  * @author  Matěj Křenek
  * @version V1.0.0
  * @date    8-March-2020
  * @brief   This file contains all pwm functions for the PWM peripheral.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "pwm.h"

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
  * @addtogroup PWM_Public_Functions
  * @{
  */

Pin* PWM_Create(GPIO_TypeDef* port, GPIO_Pin pin, uint8_t pwm_value) {
    Pin pwm_pin = {port, pin, pwm_value, 0};

    return &pwm_pin;
}

void PWM_Modulate(Pin* pwm_pin) {
    if(pwm_pin->value > pwm_pin->counter) {
        GPIO.writeHigh(pwm_pin);
    } else {
        GPIO.writeLow(pwm_pin);
    }

    for(uint32_t i = 0; i < 50; i++);
}

const PWM_Module PWM = {
  .create = PWM_Create,
  .modulate = PWM_Modulate
};

/**
  * @}
  */
  
/**
  * @}
  */
  

/*****************************END OF FILE****/
