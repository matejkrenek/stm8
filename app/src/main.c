#include "stm8s.h"

Pin *LED;
Pin *Button;

onInterrupt(EXTI_PORTE_IRQHandler, 7)
{
    GPIO.writeHigh(LED);
}

void main(void)
{
    CLK.HSI(HSIDIV1);
    EXTI.setSensitivity(EXTI_GPIOE, FALL_ONLY);
    ITC.setPriority(ITC_IRQ_GPIOE, LEVEL_0);
    ITC.enable();

    Button = GPIO.init(GPIOE, PIN_0, INPUT_PU_IT);
    LED = GPIO.init(GPIOC, PIN_5, OUTPUT_PP_LOW_SLOW);

    while (1)
    {

    }
}
