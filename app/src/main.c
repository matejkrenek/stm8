#include "stm8s.h"

void main(void)
{
    CLK.HSI(HSIDIV1);

    // initialize pins
    Pin *Button = GPIO.init(GPIOE, PIN_4, INPUT_PU_IT);
    Pin *LED = GPIO.init(GPIOC, PIN_5, OUTPUT_PP_LOW_SLOW);
    EXTI.setup(GPIOE, FALL_ONLY, LEVEL_1);

    while (1)
    {
        if (value == 1)
        {
            GPIO.writeHigh(LED);
        }
    }
}
