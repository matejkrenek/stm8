#include "stm8s.h"
#include "Serial.h"
#include "LiquidCrystal_I2C.h"

Pin *Button1;

void main(void)
{
    CLK.HSI(HSIDIV1);
    Serial.begin(9600);
    ITC.enable();

    while (1)
    {
        Serial.write("running \n");
        delay.ms(500);
    }
}