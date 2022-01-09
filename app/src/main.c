#include "gpio.c"
#include "clk.c"
#include "delay.c"

void main(void)
{    
    clk.deinit();
    gpio.deinit();

    clk.hsi(0);
    clk.hsi_config(hsi8);

    Pin Led1 = gpio.pin(C, _7, OUTPUT);
    Pin Led2 = gpio.pin(C, _5, OUTPUT);
    Pin Button = gpio.pin(E, _4, INPUT);

    while(1) {
        delay(1000);
        gpio.reverse(C, Led2);
    }
}
