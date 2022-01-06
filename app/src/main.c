#include "stm8s.h"
#include "__assert__.h"

#define LED_PORT (GPIOC)
#define LED1 (GPIO_PIN_5)
#define LED2 (GPIO_PIN_7)
#define LED3 (GPIO_PIN_6)
#define LED4 (GPIO_PIN_4)
#define LED5 (GPIO_PIN_3)
#define LED6 (GPIO_PIN_2)

void Delay (uint32_t count);

void main(void)
{
    GPIO_Pin_TypeDef Leds[6] = {LED1, LED2, LED3, LED4, LED5, LED6};

    for(int i = 0; i < 6; i++) {
        GPIO_Init(LED_PORT, Leds[i], GPIO_MODE_OUT_PP_HIGH_SLOW);
    }

    while(1) {
        for(int i = 0; i < 6; i++) {
            Delay(10000);
            GPIO_WriteReverse(LED_PORT, Leds[i]);
        }
    }
}

void Delay(uint32_t count)
{
  while (count != 0)
  {
    count--;
  }
}

