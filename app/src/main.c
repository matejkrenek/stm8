#include "gpiolib.c"
#include "delay.c"

void main(void)
{    
    Pin Led1 = gpio.pin(C, _7, OUTPUT);
    Pin Led2 = gpio.pin(C, _5, OUTPUT);
    Pin Button = gpio.pin(E, _4, INPUT);

    while(1) {

        if(gpio.read(E, Button)) {
            gpio.reverse(C, Led2);
        }
    }
}
