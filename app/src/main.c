#include "stm8s.h"
#include "pwm.h"

void main(void)
{
    // frequency
    CLK.HSI(HSIDIV1);

    // initialize pins
    Pin* Button1 = GPIO.init(GPIOB, PIN_0, INPUT_PU_NO_IT);
    Pin* Button2 = GPIO.init(GPIOB, PIN_1, INPUT_PU_NO_IT);
    Pin* LED = GPIO.init(GPIOB, PIN_2, OUTPUT_PP_HIGH_SLOW);
    LED = PWM.create(LED, 50);

    while (1)
    {
        if(GPIO.read(Button1)) {
            Button1->now = 1;
        } else {
            Button1->now = 0;
        }

        if(GPIO.read(Button2)) {
            Button2->now = 1;
        } else {
            Button2->now = 0;
        }

        if(Button1->now && !Button1->prev) {
            LED->value += Button1->power;

            if(LED->value >= 255) {
                LED->value = 0;
            }

            LED->counter = 0;
        }

        if(Button2->now && !Button2->prev) {
            LED->value -= Button2->power;

            if(LED->value <= 0) {
                LED->value = 255;
            }

            LED->counter = 0;
        }

        PWM.modulate(LED, Button1->power);
        Button1->prev = Button1->now;
        Button2->prev = Button2->now;
    }
}
