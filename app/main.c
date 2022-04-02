#include "stm8s.h"

Pin *LED;
Pin *Button1;
Pin *Button2;

onInterrupt(EXTI_PORTE_IRQHandler, 7)
{

    if (GPIO.read(Button1))
    {
        GPIO.writeHigh(LED);
    }
}

void main(void)
{
    CLK.HSI(HSIDIV1);
    EXTI.setSensitivity(EXTI_GPIOE, FALL_ONLY);
    ITC.setPriority(ITC_IRQ_GPIOE, LEVEL_0);
    ITC.enable();

    Button1 = GPIO.init(GPIOE, PIN_0, INPUT_PU_IT);
    Button2 = GPIO.init(GPIOE, PIN_5, INPUT_PU_IT);
    LED = GPIO.init(GPIOC, PIN_5, OUTPUT_PP_LOW_SLOW);

    while (1)
    {
        if (!GPIO.read(Button1))
        {
            GPIO.writeReverse(LED);
            delay.cycles(500000);
        }
    }
}