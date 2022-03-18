#include "stm8s.h"
#include "stm8s_delay.h"
#include "7_segment.h"

void main(void)
{
    CLK.HSI(HSIDIV1);

    Pin* Button1 = GPIO.init(GPIOE, PIN_4, INPUT_PU_NO_IT);
    Pin* Segment7 = GPIO.init(GPIOB, PIN_ALL, OUTPUT_PP_LOW_SLOW);
    Segment7 = SevenSegment.init(Segment7, 0);
    
    while (1)
    {
        if(GPIO.read(Button1)) {
            Button1->now = 1;
        } else {
            Button1->now = 0;
        }

        if(Button1->now && !Button1->prev) {
            Segment7->value++;
        }

        SevenSegment.write(Segment7, Segment7->value);
        delay.cycles(500000);
    }
}
