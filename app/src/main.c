#include "stm8s.h"

void main(void)
{
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
    gpio.init(GPIOC, PIN_5, OUTPUT_PP_HIGH_FAST);

    while (1)
    {
        for (uint32_t i = 0; i < 100000; i++)
            ;
    }
}
