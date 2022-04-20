#include "stm8s.h"

Pin *LED;
uint8_t tim_counter = 0;

void main(void)
{
    CLK.HSI(HSIDIV8);

    LED = GPIO.init(GPIOC, PIN_5, OUTPUT_PP_LOW_SLOW);

    while (1)
    {
        

    }
}