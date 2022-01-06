#include "stm8s.h"
#include "__assert__.h"

void main(void)
{
    GPIO_Init(GPIOC, GPIO_PIN_5, GPIO_MODE_OUT_PP_HIGH_SLOW);

    while(1) {
        GPIO_WriteLow(GPIOC, GPIO_PIN_5);
    }
}

