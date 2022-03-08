#include "stm8s.h"
#include "pwm.h"

void DELAY(uint32_t iterations) {
    for(uint32_t i = 0; i < iterations; i++);
}

void main(void)
{
    // frequency
    CLK.HSI(HSIDIV1);

    // initialize pins
    Pin* LED = GPIO.init(GPIOC, PIN_5, OUTPUT_PP_HIGH_SLOW);
    Pin* Button = GPIO.init(GPIOE, PIN_4, INPUT_FL_NO_IT);
    

    while (1)
    {
        if(GPIO.read(Button)) {
            GPIO.writeLow(LED);
        }

        delay.ms(1000);
    }
}
