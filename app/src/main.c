#include "stm8s.h"

int value = 0;

INTERRUPT_HANDLER(EXTI_PORTE_IRQHandler, 7)
{
    value = 1;
}

void main(void)
{
    // frequency
    CLK.HSI(HSIDIV1);

    // initialize pins
    Pin *Button = GPIO.init(GPIOE, PIN_4, INPUT_PU_IT);
    Pin *LED = GPIO.init(GPIOC, PIN_5, OUTPUT_PP_LOW_SLOW);

    EXTI.setSensitivity(EXTI_GPIOE, FALL_ONLY);
    ITC.setPriority(ITC_IRQ_GPIOE, LEVEL_3);

    while (1)
    {
        if (value == 1)
        {
            GPIO.writeHigh(LED);
        }
    }
}
