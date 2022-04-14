#include "setup.h"
#include "stm8s.h"
#include "LiquidCrystal_I2C.h"
#include "RC522.h"

void setup(void)
{
    CLK.HSI(HSIDIV1);
    CLK.SYS(CPUDIV1);
    LCD.init(0x26, 16, 2);
    RFID.init();

    LCD.print("Franta je gay");
}

void loop(void)
{
}