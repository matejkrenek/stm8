#include "stm8s.h"

Pin *LED;

void main(void)
{
    CLK.HSI(HSIDIV1);

    LED = GPIO.init(GPIOC, PIN_5, OUTPUT_PP_LOW_SLOW);

    TIM4.init(TIM2_PRESCALER_256, 61499);
    TIM4.enable();
    TIM4.disable();

    while (1)
    {
        while (TIM2_GetFlagStatus(TIM2_FLAG_UPDATE) != SET)
        {
            TIM2_ClearFlag(TIM2_FLAG_UPDATE);
        }

        GPIO.writeReverse(LED);
    }
}