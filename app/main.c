#include "stm8s.h"

Pin *LED;

void main(void)
{
    CLK.HSI(HSIDIV1);

    LED = GPIO.init(GPIOC, PIN_5, OUTPUT_PP_LOW_SLOW);

    while (1)
    {
    }
}