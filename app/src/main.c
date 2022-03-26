#include "stm8s.h"
<<<<<<< HEAD

int value = 0;

INTERRUPT_HANDLER(EXTI_PORTE_IRQHandler, 7)
{
    value = 1;
}
=======
#include "stm8s_delay.h"
#include "7_segment.h"
>>>>>>> b4872c2c852ba1330324b9df3d04f4af84540fea

void main(void)
{
    CLK.HSI(HSIDIV1);

<<<<<<< HEAD
    // initialize pins
    Pin *Button = GPIO.init(GPIOE, PIN_4, INPUT_PU_IT);
    Pin *LED = GPIO.init(GPIOC, PIN_5, OUTPUT_PP_LOW_SLOW);

    EXTI.setSensitivity(EXTI_GPIOE, FALL_ONLY);
    ITC.setPriority(ITC_IRQ_GPIOE, LEVEL_3);

=======
    Pin* Button1 = GPIO.init(GPIOE, PIN_4, INPUT_PU_NO_IT);
    Pin* Segment7 = GPIO.init(GPIOB, PIN_ALL, OUTPUT_PP_LOW_SLOW);
    Segment7 = SevenSegment.init(Segment7, 0);
    
>>>>>>> b4872c2c852ba1330324b9df3d04f4af84540fea
    while (1)
    {
        if (value == 1)
        {
            GPIO.writeHigh(LED);
        }
<<<<<<< HEAD
=======

        if(Button1->now && !Button1->prev) {
            Segment7->value++;
        }

        SevenSegment.write(Segment7, Segment7->value);
        delay.cycles(500000);
>>>>>>> b4872c2c852ba1330324b9df3d04f4af84540fea
    }
}
