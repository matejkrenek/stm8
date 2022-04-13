#include "setup.h"
#include "stm8s.h"
#include "LiquidCrystal_I2C.h"

Pin *LED_BUILTIN;

void setup(void)
{
    GPIO_DeInit;
    CLK_DeInit();
    I2C.deinit();
    CLK.HSI(HSIDIV1);

    LCD.init(0x26, 16, 2);
    LCD.setCursor(0, 0);
    LCD.print("Franta");
    LCD.setCursor(0, 1);
    LCD.print("je gay");
}

void loop(void)
{
}