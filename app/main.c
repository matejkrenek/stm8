#include "stm8s.h"
#include "Serial.h"
#include "encoder.h"
#include "LiquidCrystal_I2C.h"

uint8_t counter = 0;
uint8_t counter_str[10];

void main(void)
{
    CLK.HSI(HSIDIV1);
    Serial.begin(9600);
    LCD.init(0x26, 16, 2);
    Encoder.init();

    while (1)
    {
        sprintf(counter_str, "%u", Encoder.read());
        LCD.clear();
        LCD.setCursor(0, 0);
        LCD.print(counter_str);
    }
}