/**
 ******************************************************************************
 * @file    stm8s_gpio.c
 * @author  MCD Application Team
 * @version V2.3.0
 * @date    16-June-2017
 * @brief   This file contains all the functions for the GPIO peripheral.
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

/* Includes ------------------------------------------------------------------*/
#include "stm8s_gpio.h"

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
 * @addtogroup GPIO_Public_Functions
 * @{
 */

/**
 * @brief  Deinitializes the GPIOx peripheral registers to their default reset values.
 * @param  GPIOx: Select the GPIO peripheral number (x = A to I).
 * @retval None
 */
void GPIO_DeInit(GPIO_TypeDef *GPIOx)
{
  GPIOx->ODR = GPIO_ODR_RESET_VALUE; /* Reset Output Data Register */
  GPIOx->DDR = GPIO_DDR_RESET_VALUE; /* Reset Data Direction Register */
  GPIOx->CR1 = GPIO_CR1_RESET_VALUE; /* Reset Control Register 1 */
  GPIOx->CR2 = GPIO_CR2_RESET_VALUE; /* Reset Control Register 2 */
}

/**
 * @brief  Initializes the GPIOx according to the specified parameters.
 * @param  GPIOx : Select the GPIO peripheral number (x = A to I).
 * @param  Pin : This parameter contains the pin number, it can be any value
 *         of the @ref GPIO_Pin enumeration.
 * @param  Mode : This parameter can be a value of the
 *         @ref GPIO_Mode enumeration.
 * @retval None
 */

Pin *GPIO_Init(GPIO_TypeDef *GPIOx, GPIO_Pin GPIO_pin, GPIO_Mode Mode)
{
  /*----------------------*/
  /* Check the parameters */
  /*----------------------*/

  assert_param(IS_GPIO_MODE(Mode));
  assert_param(IS_GPIO_PIN(GPIO_pin));

  /* Reset corresponding bit to GPIO_Pin in CR2 register */
  GPIOx->CR2 &= (uint8_t)(~(GPIO_pin));

  /*-----------------------------*/
  /* Input/Output mode selection */
  /*-----------------------------*/

  if (((uint8_t)Mode & (uint8_t)0x80) != (uint8_t)0x00)
  {
    if ((((uint8_t)(Mode)) & (uint8_t)0x10) != (uint8_t)0x00) /* High level */
    {
      GPIOx->ODR |= (uint8_t)GPIO_pin;
    }
    else /* Low level */
    {
      GPIOx->ODR &= (uint8_t)(~(GPIO_pin));
    }
    /* Set Output mode */
    GPIOx->DDR |= (uint8_t)GPIO_pin;
  }
  else /* Input mode */
  {
    /* Set Input mode */
    GPIOx->DDR &= (uint8_t)(~(GPIO_pin));
  }

  /*------------------------------------------------------------------------*/
  /* Pull-Up/Float (Input) or Push-Pull/Open-Drain (Output) modes selection */
  /*------------------------------------------------------------------------*/

  if ((((uint8_t)(Mode)) & (uint8_t)0x40) != (uint8_t)0x00) /* Pull-Up or Push-Pull */
  {
    GPIOx->CR1 |= (uint8_t)GPIO_pin;
  }
  else /* Float or Open-Drain */
  {
    GPIOx->CR1 &= (uint8_t)(~(GPIO_pin));
  }

  /*-----------------------------------------------------*/
  /* Interrupt (Input) or Slope (Output) modes selection */
  /*-----------------------------------------------------*/

  if ((((uint8_t)(Mode)) & (uint8_t)0x20) != (uint8_t)0x00) /* Interrupt or Slow slope */
  {
    GPIOx->CR2 |= (uint8_t)GPIO_pin;
  }
  else /* No external interrupt or No slope control */
  {
    GPIOx->CR2 &= (uint8_t)(~(GPIO_pin));
  }

  Pin *pin = (Pin *)malloc(sizeof(Pin));
  pin->port = GPIOx;
  pin->pin = GPIO_pin;
  pin->mode = Mode;
  pin->now = 0;
  pin->prev = 0;
  pin->power = 10;
  pin->value = 0;
  pin->counter = 0;

  return pin;
}

/**
 * @brief  Writes data to the specified GPIO data port.
 * @note   The port must be configured in output mode.
 * @param  GPIOx : Select the GPIO peripheral number (x = A to I).
 * @param  PortVal : Specifies the value to be written to the port output
 *         data register.
 * @retval None
 */
void GPIO_Write(Pin *pin, uint8_t PortVal)
{
  pin->port->ODR = PortVal;
}

/**
 * @brief  Writes high level to the specified GPIO pins.
 * @note   The port must be configured in output mode.
 * @param  GPIOx : Select the GPIO peripheral number (x = A to I).
 * @param  PortPins : Specifies the pins to be turned high to the port output.
 *         data register.
 * @retval None
 */
void GPIO_WriteHigh(Pin *pin)
{
  pin->port->ODR |= (uint8_t)pin->pin;
}

/**
 * @brief  Writes low level to the specified GPIO pins.
 * @note   The port must be configured in output mode.
 * @param  GPIOx : Select the GPIO peripheral number (x = A to I).
 * @param  PortPins : Specifies the pins to be turned low to the port output.
 *         data register.
 * @retval None
 */
void GPIO_WriteLow(Pin *pin)
{
  pin->port->ODR &= (uint8_t)(~pin->pin);
}

/**
 * @brief  Writes reverse level to the specified GPIO pins.
 * @note   The port must be configured in output mode.
 * @param  GPIOx : Select the GPIO peripheral number (x = A to I).
 * @param  PortPins : Specifies the pins to be reversed to the port output.
 *         data register.
 * @retval None
 */
void GPIO_WriteReverse(Pin *pin)
{
  pin->port->ODR ^= (uint8_t)pin->pin;
}

/**
 * @brief  Reads the specified GPIO output data port.
 * @note   The port must be configured in input mode.
 * @param  GPIOx : Select the GPIO peripheral number (x = A to I).
 * @retval GPIO output data port value.
 */
uint8_t GPIO_ReadOutputData(Pin *pin)
{
  return ((uint8_t)pin->port->ODR);
}

/**
 * @brief  Reads the specified GPIO input data port.
 * @note   The port must be configured in input mode.
 * @param  GPIOx : Select the GPIO peripheral number (x = A to I).
 * @retval GPIO input data port value.
 */
uint8_t GPIO_ReadInputData(Pin *pin)
{
  return ((uint8_t)pin->port->IDR);
}

/**
 * @brief  Reads the specified GPIO input data pin.
 * @param  GPIOx : Select the GPIO peripheral number (x = A to I).
 * @param  GPIO_Pin : Specifies the pin number.
 * @retval BitStatus : GPIO input pin status.
 */
bool GPIO_ReadInputPin(Pin *pin)
{
  return (pin->port->IDR & (uint8_t)pin->pin) == 0x00;
}

/**
 * @brief  Configures the external pull-up on GPIOx pins.
 * @param  GPIOx : Select the GPIO peripheral number (x = A to I).
 * @param  GPIO_Pin : Specifies the pin number
 * @param  NewState : The new state of the pull up pin.
 * @retval None
 */
void GPIO_ExternalPullUpConfig(GPIO_TypeDef *GPIOx, GPIO_Pin GPIO_Pin, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_GPIO_PIN(GPIO_Pin));
  assert_param(IS_FUNCTIONALSTATE_OK(NewState));

  if (NewState != DISABLE) /* External Pull-Up Set*/
  {
    GPIOx->CR1 |= (uint8_t)GPIO_Pin;
  }
  else /* External Pull-Up Reset*/
  {
    GPIOx->CR1 &= (uint8_t)(~(GPIO_Pin));
  }
}

const GPIO_Module GPIO = {
    .init = GPIO_Init,
    .write = GPIO_Write,
    .writeLow = GPIO_WriteLow,
    .writeHigh = GPIO_WriteHigh,
    .writeReverse = GPIO_WriteReverse,
    .read = GPIO_ReadInputPin,
    .readOutputData = GPIO_ReadOutputData,
    .readInputData = GPIO_ReadInputData,
};

/**
 * @}
 */

/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
