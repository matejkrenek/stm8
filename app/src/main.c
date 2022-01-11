#include "gpio.c"
#include "clk.c"
#include "delay.c"

#define len(arr) sizeof(arr)/sizeof(arr[0])

void main(void)
{    
    clk.deinit();
    gpio.deinit();

    clk.hsi(0);
    clk.hsi_config(hsi8);

    Pin Led1 = gpio.pin(C, _5, OUTPUT);
    Pin Led2 = gpio.pin(C, _7, OUTPUT);
    Pin Led3 = gpio.pin(C, _6, OUTPUT);
    Pin Led4 = gpio.pin(C, _4, OUTPUT);
    Pin Led5 = gpio.pin(C, _3, OUTPUT);
    Pin Led6 = gpio.pin(C, _2, OUTPUT);
    Pin ActiveLed;
    Pin Leds[] = {Led1, Led2, Led3, Led4, Led5, Led6};

    Pin Button = gpio.pin(E, _4, INPUT);

    while(1) {

        for (int i = 0; i < len(Leds); i++)
        {
            delay(100);
            gpio.reverse(C, Leds[i]);
            delay(100);
            gpio.reverse(C, Leds[i]);
            ActiveLed = Leds[i];    

            if(gpio.read(E, Button)) {
                if(ActiveLed) {
                    for (int i = 0; i < len(Leds); i++)
                    {
                        gpio.low(C, Leds[i]);
                    }
                    
                    for (int i = 0; i < 12; i++) {
                        delay(100);
                        gpio.reverse(C, ActiveLed);
                    }

                    gpio.low(C, ActiveLed);
                    break;
                }
            }
        }
    }
}